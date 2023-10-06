#include <qlpch.h>
#include "camera_controller.hpp"
#include "input/input.h"

#include "events/application_events.h"
#include "events/input_events.h"


EditorCameraController::EditorCameraController()
{

}

EditorCameraController::~EditorCameraController()
{
}

void EditorCameraController::Update(float deltaTime)
{
	if (m_camera == nullptr)
		return;

	if (InputHandler::IsKeyDown(Keys::Space))
	{
		constexpr float speed = 1.0f;
		// Get mouse position
		double xpos, ypos;
		POINT point;
		GetCursorPos(&point);
		xpos = point.x;
		ypos = point.y;
		SetCursorPos(m_screenWidth / 2, m_screenHeight / 2);
		float xoffSet = ((float)xpos - (m_screenWidth / 2)) * mouseSpeed * 0.016f;
		float yoffSet = ((float)ypos - (m_screenHeight / 2)) * mouseSpeed * 0.016f;

		horizontalAngle += xoffSet;
		verticalAngle -= yoffSet;

		glm::vec3 up = m_camera->GetUpVector();
		glm::vec3 cameraTranslation = m_camera->GetPosition();
		//Direction we're facing based on the mouse angle 
		glm::vec3 direction(
			cos(glm::radians(horizontalAngle)) * cos(glm::radians(verticalAngle)),
			sin(glm::radians(verticalAngle)),
			sin(glm::radians(horizontalAngle)) * cos(glm::radians(verticalAngle))
		);
		direction = glm::normalize(direction);
		glm::vec3 translateRight = glm::cross(direction, up);

		if (InputHandler::IsKeyDown(Keys::W))
		{
			cameraTranslation += direction * speed * deltaTime;
		}
		else if (InputHandler::IsKeyDown(Keys::S))
		{
			cameraTranslation += -direction * speed * deltaTime;
		}
		if (InputHandler::IsKeyDown(Keys::A))
		{
			cameraTranslation += -translateRight * speed * deltaTime;
		}
		else if (InputHandler::IsKeyDown(Keys::D))
		{
			cameraTranslation += translateRight * speed * deltaTime;
		}

		//Set the camera up with the new position and forward vectors from the controller
		m_camera->SetPosition(cameraTranslation);
		m_camera->SetForwardVector(direction);
		m_camera->SetRightVector(translateRight);
	}
}

void EditorCameraController::OnEvent(QCore::Event& e)
{
	//QCore::EventDispatcher dispatcher(e);
	//dispatcher.Dispatch<QCore::MouseScrolledEvent>(BIND_EVENT_FN(CameraController::OnMouseScrolled));
	//dispatcher.Dispatch<QCore::WindowResizeEvent>(BIND_EVENT_FN(CameraController::OnWindowResized));
}

bool EditorCameraController::OnMouseScrolled(QCore::MouseScrolledEvent& e)
{
	float zoomScale = 0.1f;
	float zoomMin = 0.1f, zoomMax = 2.0f;

	/*CameraSettings& settings = m_camera->getSettings();

	settings.Zoom += e.GetYOffset() * zoomScale;
	settings.Zoom = settings.Zoom < zoomMin ? zoomMin: settings.Zoom;
	settings.Zoom = settings.Zoom > zoomMax ? zoomMax : settings.Zoom;*/

	return false;
}

bool EditorCameraController::OnWindowResized(QCore::WindowResizeEvent& e)
{
	float width = (float)e.Width;
	float height = (float)e.Height;
	float aspect = width / height;

	return false;
}

void EditorCameraController::Resize(float width, float height)
{
	m_screenWidth = width;
	m_screenHeight = height;
}