#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{

	/*
	**	For GUI
	*/

	m_sProgrammer = "Par for the Course";

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	//TODO : Move the following to new method == InstantiateMap();
	//InitMap
	CourseBuilder TestBuilder;
	std::string loc = "Data\\Courses\\map1.txt"; //Load First Map
	bool bDidRead = TestBuilder.ReadMap(loc); //assure map has loaded
	if (bDidRead) 
	{ 
		TestBuilder.BuildMap(); //build the map from the files
		m_oaCourseData = TestBuilder.m_oaCourse; //set the course data to be available locally


#pragma region init posVec
		vector3 planePosition;
		vector3 holePosition;
		vector3 cratePosition;
		vector3 ballPosition;
		vector3 arrowPosition;	
		vector3 wallPosition;
		std::string planeName;
		std::string wallName;
		std::string crateName;
#pragma endregion

		


		//iterate through the course data and spawn the course
		for (int i = 0; i < m_oaCourseData.size(); i++)
		{
			CourseBuilder::CourseControl temp = m_oaCourseData[i];
			
			switch (temp.type)
			{
				//Green
			case 0:
				planeName = "Plane" + std::to_string(planeCount);
				m_pEntityMngr->AddEntity("openGLolf\\plane.obj", planeName);
				m_pEntityMngr->UsePhysicsSolver(false, planeName);
				planePosition = vector3(temp.x, -0.7f, -temp.z);
				m_pEntityMngr->SetPosition(planePosition, planeName);
				planeCount++;
				break;

				//Hole
			case 1:
				
				m_pEntityMngr->AddEntity("openGLolf\\Cup.obj", "Hole");
				m_pEntityMngr->UsePhysicsSolver(false, "Hole");
				holePosition = vector3(temp.x, .0f, -temp.z);
				m_pEntityMngr->SetPosition(holePosition, "Hole");
				
				break;

				//Wall
			case 2:
				wallName = "Wall" + std::to_string(wallCount);
				m_pEntityMngr->AddEntity("openGLolf\\wall.obj", wallName);
				m_pEntityMngr->UsePhysicsSolver(false, wallName);
				wallPosition = vector3(temp.x, -1.0f, -temp.z);
				m_pEntityMngr->SetPosition(wallPosition, wallName);
				wallCount++; 
				walls.push_back(temp);
				break;

				//Obstacle
			case 3:
				crateName = "Crate" + std::to_string(obCount);
				m_pEntityMngr->AddEntity("openGLolf\\Pollard.obj", crateName);
				m_pEntityMngr->UsePhysicsSolver(false, crateName);
				cratePosition = vector3(temp.x, -.6f, -temp.z);
				m_pEntityMngr->SetPosition(cratePosition, crateName);
				obCount++;				
				break;

				//Spawn Ball
			case 4:
				m_pEntityMngr->AddEntity("openGLolf\\GolfBall.obj", "Ball");
				m_pEntityMngr->UsePhysicsSolver(true, "Ball");
				ballPosition = vector3(temp.x, 0.0f, -temp.z);
				m_pEntityMngr->SetPosition(ballPosition, "Ball");

				//Make Arrow
				m_pEntityMngr->AddEntity("openGLolf\\arrow.obj", "Arrow");
				m_pEntityMngr->UsePhysicsSolver(false, "Arrow");
				arrowPosition = ballPosition;
				m_pEntityMngr->SetPosition(arrowPosition, "Arrow");

				//Set the position and target of the camera
				m_pCameraMngr->SetPositionTargetAndUp(
					ballPosition + cameraOffset, //Position
					ballPosition,	//Target
					AXIS_Y);		//Up
				ballId = m_pEntityMngr->GetEntityIndex("Ball");
				break;

			} //end switch
		} //end for
	} //end if loaded

	//END TODO

	

	//generate our clock
	m_uClock= m_pSystem->GenClock();
}
void Application::Update(void)
{
	//if the ball is in the whole set the state to win and the ball's physic solver to have no minimum y
	if (m_pEntityMngr->GetInHole()) {
		m_lsLevelState = Win;
		m_pEntityMngr->GetEntity(0)->GetMySolver()->SetPlay(false);
		
	}
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();
	m_fDeltaTime = m_pSystem->GetDeltaTime(m_uClock);

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Set model matrix of ball
	vector3 ballPosition = m_pEntityMngr->GetPosition("Ball");
	
	float ballWidth = m_pEntityMngr->GetRigidBody("Ball")->GetHalfWidth().x * 2.0f;
	if (ballPosition.y <= m_pEntityMngr->GetRigidBody()->GetHalfWidth().y) {
		ballPosition.y = m_pEntityMngr->GetRigidBody()->GetHalfWidth().y+0.4;
	}
	matrix4 mBall = glm::translate(ballPosition);
	m_pEntityMngr->GetModel("Ball")->SetModelMatrix(mBall);
	m_pEntityMngr->GetRigidBody("Ball")->SetModelMatrix(mBall * glm::rotate(180.0f + glm::degrees(cameraRadian), AXIS_Y));

	//Set model matrix of arrow
	m_pEntityMngr->SetPosition(ballPosition - hitDirection, "Arrow");
	m_pEntityMngr->SetPosition(ballPosition, "Arrow");
	vector3 arrowPosition = m_pEntityMngr->GetPosition("Arrow");
	vector3 arrowScale = vector3(0.5f, 0.2f, m_fHitPower*0.75);
	if (m_fHitPowerOffset == 0.0f) {
		arrowScale = ZERO_V3;
	}
	matrix4 mArrow = glm::translate(arrowPosition - (hitDirection * m_fHitPower)+ vector3(0.0f,1.0f,0.0f))
		* glm::rotate(180.0f + glm::degrees(cameraRadian), AXIS_Y)
		* glm::scale(arrowScale);
	m_pEntityMngr->GetModel("Arrow")->SetModelMatrix(mArrow);
	m_pEntityMngr->GetRigidBody("Arrow")->SetModelMatrix(mArrow);

	//Set model matrix of hole
	
	vector3 holePosition = m_pEntityMngr->GetPosition("Hole");
	std::cout << holePosition.x << "," << holePosition.y << "," << holePosition.z << "\n";
	matrix4 mHole = glm::translate(holePosition)
		 *glm::scale(.2f, 0.05f, .2f);
	m_pEntityMngr->GetModel("Hole")->SetModelMatrix(mHole);
	m_pEntityMngr->GetRigidBody("Hole")->SetModelMatrix(mHole);

	

	//Set the position and target of the camera
	//if we are playing the target is the ball
	if (m_lsLevelState == Play) {

		m_pCameraMngr->SetPositionTargetAndUp(
			ballPosition + cameraOffset, //Position
			ballPosition,	//Target
			AXIS_Y);		//Up
	}else {
		
		m_pCameraMngr->SetPositionTargetAndUp(
			holePosition + cameraOffset, //Position
			holePosition,	//Target
			AXIS_Y);		//Up 
	}
	


	//Set model matrix of planes
	for (int i = 0; i < planeCount; i++)
	{
		std::string planeName = "Plane" + std::to_string(i);

		matrix4 mPlane = glm::translate(m_pEntityMngr->GetPosition(planeName))*glm::scale(vector3(2.17f, 1.0f, 2.17f));
		m_pEntityMngr->GetModel(planeName)->SetModelMatrix(mPlane);
		m_pEntityMngr->GetRigidBody(planeName)->SetModelMatrix(mPlane);
	}

	for (int i = 0; i < obCount; i++)
	{
		std::string crateName = "Crate" + std::to_string(i);

		matrix4 mCrate = glm::translate(m_pEntityMngr->GetPosition(crateName));
		m_pEntityMngr->GetModel(crateName)->SetModelMatrix(mCrate);
		m_pEntityMngr->GetRigidBody(crateName)->SetModelMatrix(mCrate);
	}
	
	for (int i = 0; i < wallCount; i++)
	{
		std::string wallName = "Wall" + std::to_string(i);
		CourseBuilder::CourseControl temp = walls[i];

		matrix4 mWall = glm::translate(m_pEntityMngr->GetPosition(wallName)) 
			* glm::rotate((temp.bRot90 ? 0.0f : 90.0f), AXIS_Y)
			* glm::scale(vector3(.2f, 1.0f, .275f));
		m_pEntityMngr->GetModel(wallName)->SetModelMatrix(mWall);
		m_pEntityMngr->GetRigidBody(wallName)->SetModelMatrix(mWall);
	}
	
	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui,
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}

/*
**	Helper Functions
*/

std::string Application::PrintVector3(vector3 v) {
	return std::to_string(v.x) + " " + std::to_string(v.y) + " " + std::to_string(v.z);
}