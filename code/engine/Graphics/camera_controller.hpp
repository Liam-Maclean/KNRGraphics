#pragma once

#include "camera.h"

#include "events/event.h"
#include "events/input_events.h"
#include "events/application_events.h"


class EditorCameraController
{
public:
	EditorCameraController();
	~EditorCameraController();

	void SetCamera(Ref<QRender::EditorCamera> camera) { m_camera = camera; }
	Ref<QRender::EditorCamera> getCamera() { return m_camera; }

	void Update(float deltaTime);
	void OnEvent(QCore::Event& e);
	bool OnMouseScrolled(QCore::MouseScrolledEvent& e);
	bool OnWindowResized(QCore::WindowResizeEvent& e);
	void Resize(float width, float height);
private:
	float m_aspectRatio = 1.0f;
	float m_zoomLevel = 1.0f;
	float m_screenWidth = 0;
	float m_screenHeight = 0;
	float mouseSpeed = 2.0f;
	float horizontalAngle = 90.0f;
	float verticalAngle = 0.0f;


	Ref<QRender::EditorCamera> m_camera = nullptr;
};