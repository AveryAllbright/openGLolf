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

	//Make Arrow
	m_pEntityMngr->AddEntity("openGLolf\\arrow.obj", "Arrow");
	m_pEntityMngr->UsePhysicsSolver(false, "Arrow");
	vector3 arrowPosition = ballPosition;
	m_pEntityMngr->SetPosition(arrowPosition, "Arrow");

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		ballPosition + cameraOffset, //Position
		ballPosition,	//Target
		AXIS_Y);		//Up

	//Make a hole
	m_pEntityMngr->AddEntity("openGLolf\\Cup.obj", "Hole");
	m_pEntityMngr->UsePhysicsSolver(false, "Hole");
	vector3 holePosition = vector3(0.0f, 0.1f, -30.0f);
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
	vector3 arrowScale = vector3(1.0f, 0.2f, m_fHitPower);
	if (m_fHitPowerOffset == 0.0f) {
		arrowScale = ZERO_V3;
	}
	matrix4 mArrow = glm::translate(arrowPosition - (hitDirection * m_fHitPower))
		* glm::rotate(180.0f + glm::degrees(cameraRadian), AXIS_Y)
		* glm::scale(arrowScale);
	m_pEntityMngr->GetModel("Arrow")->SetModelMatrix(mArrow);
	m_pEntityMngr->GetRigidBody("Arrow")->SetModelMatrix(mArrow);

	//Set model matrix of hole
	vector3 holePosition = m_pEntityMngr->GetPosition("Hole");
	matrix4 mHole = glm::translate(holePosition) 
		* glm::scale(0.2f, 0.01f, 0.2f);
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