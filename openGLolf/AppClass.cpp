#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 0.0f, 10.0f), //Position
		vector3(0.0f, 0.0f, 9.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	//Make Ball
	m_pEntityMngr->AddEntity("openGLolf\\GolfBall.obj", "Ball");
	m_pEntityMngr->UsePhysicsSolver(true, "Ball");
	m_pEntityMngr->SetPosition(vector3(0.0f, 5.0f, -10.0f), "Ball");

	//Make Plane
	m_pEntityMngr->AddEntity("openGLolf\\GolfPlane.obj", "Plane");
	m_pEntityMngr->UsePhysicsSolver(false, "Plane");
	m_pEntityMngr->SetPosition(vector3(0.0f, -5.0f, -10.0f), "Plane");
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Set model matrix of ball
	matrix4 mBall = glm::translate(m_pEntityMngr->GetPosition("Ball"));
	m_pEntityMngr->GetModel("Ball")->SetModelMatrix(mBall);
	m_pEntityMngr->GetRigidBody("Ball")->SetModelMatrix(mBall);

	//Set model matrix of plane
	matrix4 mPlane = glm::translate(m_pEntityMngr->GetPosition("Plane"));
	m_pEntityMngr->GetModel("Plane")->SetModelMatrix(mPlane);
	m_pEntityMngr->GetRigidBody("Plane")->SetModelMatrix(mPlane);
	
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