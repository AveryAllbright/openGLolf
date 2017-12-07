#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{	

	m_sProgrammer = "Par for the Course";

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	BuildCourse(m_nCourseNumber);		

	//generate our clock
	m_uClock= m_pSystem->GenClock();

	m_uOctantLevels = 4;

	m_pRoot = new MyOctant(m_uOctantLevels, 5);

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

	vector3 ballPosition = m_pEntityMngr->GetPosition(ballName);
	
	float ballWidth = m_pEntityMngr->GetRigidBody(ballName)->GetHalfWidth().x * 2.0f;
	if (ballPosition.y <= m_pEntityMngr->GetRigidBody()->GetHalfWidth().y) {
		ballPosition.y = m_pEntityMngr->GetRigidBody()->GetHalfWidth().y+0.4;
	}

	
	matrix4 mBall = glm::translate(ballPosition);
	m_pEntityMngr->GetModel(ballName)->SetModelMatrix(mBall);
	m_pEntityMngr->GetRigidBody(ballName)->SetModelMatrix(mBall * glm::rotate(180.0f + glm::degrees(cameraRadian), AXIS_Y)); // *glm::rotate(rotBy, AXIS_Z));

	
	//Set model matrix of arrow
	m_pEntityMngr->SetPosition(ballPosition - hitDirection, arrowName);
	m_pEntityMngr->SetPosition(ballPosition, arrowName);
	vector3 arrowPosition = m_pEntityMngr->GetPosition(arrowName);
	vector3 arrowScale = vector3(0.5f, 0.2f, m_fHitPower*0.75);
	if (m_fHitPowerOffset == 0.0f) {
		arrowScale = ZERO_V3;
	}
	matrix4 mArrow = glm::translate(arrowPosition - (hitDirection * m_fHitPower)+ vector3(0.0f,1.0f,0.0f))
		* glm::rotate(180.0f + glm::degrees(cameraRadian), AXIS_Y)
		* glm::scale(arrowScale);
	m_pEntityMngr->GetModel(arrowName)->SetModelMatrix(mArrow);
	m_pEntityMngr->GetRigidBody(arrowName)->SetModelMatrix(mArrow);

	//Set model matrix of hole
	
	vector3 holePosition = m_pEntityMngr->GetPosition(holeName);
	std::cout << holePosition.x << "," << holePosition.y << "," << holePosition.z << "\n";
	matrix4 mHole = glm::translate(holePosition)
		 *glm::scale(.2f, 0.05f, .2f);
	m_pEntityMngr->GetModel(holeName)->SetModelMatrix(mHole);
	m_pEntityMngr->GetRigidBody(holeName)->SetModelMatrix(mHole);

	

	//Set the position and target of the camera
	//if we are playing the target is the ball
	static float count = 0.0f;
	if (m_lsLevelState == Play) {

		m_pCameraMngr->SetPositionTargetAndUp(
			ballPosition + cameraOffset, //Position
			ballPosition,	//Target
			AXIS_Y);		//Up
	}else {
		count += m_fDeltaTime;

		m_pCameraMngr->SetPositionTargetAndUp(
			holePosition + cameraOffset, //Position
			holePosition,	//Target
			AXIS_Y);		//Up 

		if (count > 3.0f) {
			m_nCourseNumber++;
			m_nBallCount = m_nCourseNumber;
			m_nArrowCount = m_nCourseNumber;
			m_nHoleCount = m_nCourseNumber;
			if (m_nCourseNumber == 7) { m_nCourseNumber = 1; }
			m_pEntityMngr->GetEntity(0)->GetMySolver()->SetPlay(true);
			BuildCourse(m_nCourseNumber);

			count = 0;
			m_lsLevelState = Play;
			m_iShotsTaken = 0;

			return;
		}
	}
	


	//Set model matrix of planes
	for (int i = 0; i < m_nPlaneCount; i++)
	{
		std::string planeName = "Plane" + std::to_string(i);

		matrix4 mPlane = glm::translate(m_pEntityMngr->GetPosition(planeName))*glm::scale(vector3(2.17f, 1.0f, 2.17f));
		m_pEntityMngr->GetModel(planeName)->SetModelMatrix(mPlane);
		m_pEntityMngr->GetRigidBody(planeName)->SetModelMatrix(mPlane);
	}

	for (int i = 0; i < m_nObstCount; i++)
	{
		std::string crateName = "Crate" + std::to_string(i);

		matrix4 mCrate = glm::translate(m_pEntityMngr->GetPosition(crateName));
		m_pEntityMngr->GetModel(crateName)->SetModelMatrix(mCrate);
		m_pEntityMngr->GetRigidBody(crateName)->SetModelMatrix(mCrate);
	}
	
	for (int i = 0; i < m_nWallCount; i++)
	{
		std::string wallName = "Wall" + std::to_string(i);
		CourseBuilder::CourseControl temp = m_oaWalls[i];

		matrix4 mWall = glm::translate(m_pEntityMngr->GetPosition(wallName)) 
			* glm::rotate((temp.bRot90 ? 0.0f : 90.0f), AXIS_Y)
			* glm::scale(vector3(5.f, 1.0f, .28f));
		m_pEntityMngr->GetModel(wallName)->SetModelMatrix(mWall);
		m_pEntityMngr->GetRigidBody(wallName)->SetModelMatrix(mWall);
	}
	
	//Update Entity Manager
	m_pEntityMngr->Update(ballName, holeName, arrowName);

	m_nCurrentBallOct = m_pRoot->FindBall(m_nBallId);

	if (m_nCurrentBallOct != m_nPreviousBallOct)
	{
		m_nPreviousBallOct = m_nCurrentBallOct;
		m_pRoot = new MyOctant(m_uOctantLevels, 5);
	}
	

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);

	
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	m_pRoot->DisplayLeafs();

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

void Application::BuildCourse(int a_nCourseNumber)
{

	while (m_pEntityMngr->GetEntityCount() != 0) m_pEntityMngr->RemoveEntity(0);

	/*
	int temp = m_pEntityMngr->GetEntityCount();

	for (uint i = 0; i < temp; i++)
	{
		m_pEntityMngr->RemoveEntity(i);
	}	
	*/

	/*
	m_nBridgeCount = 0;
	m_nPortalCount = 0;
	m_nWallCount = 0;
	m_nPlaneCount = 0;
	m_nWaterCount = 0;
	m_nSlideCount = 0;
	m_nObstCount = 0;
	*/
	//InitMap
	CourseBuilder TestBuilder;
	std::string loc = "Data\\Courses\\map" + std::to_string(a_nCourseNumber) + ".txt"; //Load First Map
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
		vector3 waterPosition;
		vector3 portalPosition;
		vector3 slidePosition;
		vector3 bridgePosition;

		std::string planeName;
		std::string wallName;
		std::string crateName;
		std::string waterName;
		std::string portalName;
		std::string slideName;
		std::string bridgeName;
		//std::string ballName;
#pragma endregion

		//iterate through the course data and spawn the course
		for (int i = 0; i < m_oaCourseData.size(); i++)
		{
			CourseBuilder::CourseControl temp = m_oaCourseData[i];
			switch (temp.type)
			{
				//Green
			case 0:
				planeName = "Plane" + std::to_string(m_nPlaneCount);
				m_pEntityMngr->AddEntity("openGLolf\\plane.obj", planeName);
				m_pEntityMngr->UsePhysicsSolver(false, planeName);
				planePosition = vector3(temp.x, -0.7f, -temp.z);
				m_pEntityMngr->SetPosition(planePosition, planeName);
				m_nPlaneCount++;
				break;

				//Hole
			case 1:

				holeName = "Hole" + std::to_string(m_nHoleCount);
				m_pEntityMngr->AddEntity("openGLolf\\Cup.obj", holeName);
				m_pEntityMngr->UsePhysicsSolver(false, holeName);
				holePosition = vector3(temp.x, .0f, -temp.z);
				m_pEntityMngr->SetPosition(holePosition, holeName);

				break;

				//Wall
			case 2:
				wallName = "Wall" + std::to_string(m_nWallCount);
				m_pEntityMngr->AddEntity("openGLolf\\wall.obj", wallName);
				m_pEntityMngr->UsePhysicsSolver(false, wallName);
				wallPosition = vector3(temp.x, -1.0f, -temp.z);
				m_pEntityMngr->SetPosition(wallPosition, wallName);
				m_nWallCount++;
				m_oaWalls.push_back(temp);
				break;

				//Obstacle
			case 3:
				crateName = "Crate" + std::to_string(m_nObstCount);
				m_pEntityMngr->AddEntity("openGLolf\\Pollard.obj", crateName);
				m_pEntityMngr->UsePhysicsSolver(false, crateName);
				cratePosition = vector3(temp.x, -.6f, -temp.z);
				m_pEntityMngr->SetPosition(cratePosition, crateName);
				m_nObstCount++;
				break;

				//Spawn Ball
			case 4:
				ballName = "Ball" + std::to_string(m_nBallCount);
				m_pEntityMngr->AddEntity("openGLolf\\GolfBall.obj", ballName);
				m_pEntityMngr->UsePhysicsSolver(true, ballName);
				ballPosition = vector3(temp.x, 0.0f, -temp.z);
				m_pEntityMngr->SetPosition(ballPosition, ballName);

				//Make Arrow
				arrowName = "Arrow" + std::to_string(m_nArrowCount);
				m_pEntityMngr->AddEntity("openGLolf\\arrow.obj", arrowName);
				m_pEntityMngr->UsePhysicsSolver(false, arrowName);
				arrowPosition = ballPosition;
				m_pEntityMngr->SetPosition(arrowPosition, arrowName);

				//Set the position and target of the camera
				m_pCameraMngr->SetPositionTargetAndUp(
					ballPosition + cameraOffset, //Position
					ballPosition,	//Target
					AXIS_Y);		//Up

				m_nBallId = m_pEntityMngr->GetEntityIndex(ballName);
				break;

				//Slide
				//Rules for Sliding Obstacles : Will always move from Left to Right, and will always start on the left side
			case 5:
				slideName = "Slider" + std::to_string(m_nSlideCount);
				m_pEntityMngr->AddEntity("openGLolf\\Slider.obj", slideName);
				m_pEntityMngr->UsePhysicsSolver(false, slideName);
				slidePosition = vector3(temp.x, -.6f, -temp.z);
				m_pEntityMngr->SetPosition(slidePosition, slideName);
				break;

				//Bridge
				//Rules for Bridges : Bridges will be 2 x 1 tiles, will find their closest neighbor and use, a single double length plane model, will spin about its new center
			case 6:
				bridgeName = "Bridge" + std::to_string(m_nBridgeCount);
				m_pEntityMngr->AddEntity("openGLolf\\Bridge.obj", bridgeName);
				m_pEntityMngr->UsePhysicsSolver(false, bridgeName);
				bridgePosition = vector3(temp.x, -.6f, -temp.z);
				m_pEntityMngr->SetPosition(bridgePosition, bridgeName);
				break;

				//Portal
				//Rules for Portals : For simplicity, only two portals will exist in a given map. The portals will "link" together, and balls can traverse between them in either direction
			case 7:
				portalName = "Portal" + std::to_string(m_nPortalCount);
				m_pEntityMngr->AddEntity("openGLolf\\Portal.obj", portalName);
				m_pEntityMngr->UsePhysicsSolver(false, portalName);
				portalPosition = vector3(temp.x, -.6f, -temp.z);
				m_pEntityMngr->SetPosition(portalPosition, portalName);
				break;

				//Water
				//Rules for Water : If a ball collides with a water tile, it will count as an extra hit and the ball will be placed onto the nearest non-water tile
				//If we have time, switch this to return to the last non water tile the ball was on, as the current, easier method can be used to get across gaps. 
			case 8:
				waterName = "Water" + std::to_string(m_nWaterCount);
				m_pEntityMngr->AddEntity("openGLolf\\Water.obj", waterName);
				m_pEntityMngr->UsePhysicsSolver(false, waterName);
				waterPosition = vector3(temp.x, -.6f, -temp.z);
				m_pEntityMngr->SetPosition(waterPosition, waterName);
				break;

			} //end switch

		} //end for

		m_pRoot = new MyOctant(m_uOctantLevels, 5);
		m_nCurrentBallOct = m_pRoot->FindBall(m_nBallId);
		m_nPreviousBallOct = m_nCurrentBallOct;

	} //end if loaded

}