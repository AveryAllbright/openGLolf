#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{

	/*
	**	For GUI
	*/

	m_sProgrammer = "openGLolf";

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	//Make Ball
	m_pEntityMngr->AddEntity("openGLolf\\GolfBall.obj", "Ball");
	m_pEntityMngr->UsePhysicsSolver(true, "Ball");
	vector3 ballPosition = vector3(0.0f, 0.0f, -10.0f);
	m_pEntityMngr->SetPosition(ballPosition, "Ball");

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		ballPosition + cameraOffset, //Position
		ballPosition,	//Target
		AXIS_Y);		//Up

	//Make a hole
	m_pEntityMngr->AddEntity("openGLolf\\GolfBall.obj", "Hole");
	m_pEntityMngr->UsePhysicsSolver(false, "Hole");
	vector3 holePosition = vector3(0.0f, 0.0f, -30.0f);
	m_pEntityMngr->SetPosition(holePosition, "Hole");

	//Make Plane
	m_pEntityMngr->AddEntity("openGLolf\\GolfPlane.obj", "Plane");
	m_pEntityMngr->UsePhysicsSolver(false, "Plane");
	vector3 planePosition = vector3(0.0f, -0.01f, -30.0f);
	m_pEntityMngr->SetPosition(planePosition, "Plane");

	//Make Crate
	m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "Crate");
	m_pEntityMngr->UsePhysicsSolver(false, "Crate");
	vector3 cratePosition = vector3(0.0f, 0.0f, -20.0f);
	m_pEntityMngr->SetPosition(cratePosition, "Crate");


	//generate our clock
	m_uClock= m_pSystem->GenClock();
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();
	m_fDeltaTime = m_pSystem->GetDeltaTime(m_uClock);

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Set model matrix of ball
	vector3 ballPosition = m_pEntityMngr->GetPosition("Ball");
	if (ballPosition.y <= m_pEntityMngr->GetRigidBody()->GetHalfWidth().y) {
		ballPosition.y = m_pEntityMngr->GetRigidBody()->GetHalfWidth().y+0.4;
	}
	matrix4 mBall = glm::translate(ballPosition);
	m_pEntityMngr->GetModel("Ball")->SetModelMatrix(mBall);
	m_pEntityMngr->GetRigidBody("Ball")->SetModelMatrix(mBall);

	//Set model matrix of hole
	vector3 holePosition = m_pEntityMngr->GetPosition("Hole");
	matrix4 mHole = glm::translate(holePosition) * glm::scale(1.0f, 0.001f, 1.0f);
	m_pEntityMngr->GetModel("Hole")->SetModelMatrix(mHole);
	m_pEntityMngr->GetRigidBody("Hole")->SetModelMatrix(mHole);

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		ballPosition + cameraOffset, //Position
		ballPosition,	//Target
		AXIS_Y);		//Up

	//Set model matrix of plane
	matrix4 mPlane = glm::translate(m_pEntityMngr->GetPosition("Plane"));
	m_pEntityMngr->GetModel("Plane")->SetModelMatrix(mPlane);
	m_pEntityMngr->GetRigidBody("Plane")->SetModelMatrix(mPlane);

	matrix4 mCrate = glm::translate(m_pEntityMngr->GetPosition("Crate"));
	m_pEntityMngr->GetModel("Crate")->SetModelMatrix(mCrate);
	m_pEntityMngr->GetRigidBody("Crate")->SetModelMatrix(mCrate);

	

	//Update Entity Manager
	m_pEntityMngr->Update();

	/*
	std::cout << PrintVector3(m_pEntityMngr->GetPosition("Ball")) << std::endl;
	std::cout << PrintVector3(m_pEntityMngr->GetPosition("Plane")) << std::endl;

	
	//check collision
	MyRigidBody* mBallRB = m_pEntityMngr->GetRigidBody("Ball");
	MyRigidBody* mPlaneRB = m_pEntityMngr->GetRigidBody("Plane");
	if (m_pEntityMngr->GetRigidBody("Ball")->IsColliding(mPlaneRB)) {
		//mBallRB->
	}
	*/

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