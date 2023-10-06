#pragma once

#include "maths/maths.h"
#include <events/input_events.h>
#include <events/event.h>
#include "events/application_events.h"
#include "maths/maths_utils.hpp"

struct InstancedCamera;

namespace QRender
{
	enum class EProjectionType
	{
		Orthographic,
		Perspective,
	};

	class TAASystem;
	class FSR2System;
	class DLSSSystem;
	class EditorCamera
	{
	public:
		EditorCamera(EProjectionType projectionType = EProjectionType::Perspective);

		void OnUpdate(const GameTime& gameTime);
		void OnEvent(QCore::Event& event);

		void UpdateView();
		void UpdateProjection();

		inline void SetDistance(float distance) { m_distance = distance; }
		inline float GetDistance() { return m_distance; }

		inline void Translate(const Vector3f& translation)
		{
			m_focalPoint += translation;
		}

		inline Quaternion GetOrientation() const
		{
			return Quaternion(Vector3f(-m_pitch, -m_yaw, -m_roll));
		}

		inline void SetViewportSize(Vector2f size) {
			m_viewportSize = size;
			UpdateProjection();
		}

		inline const EProjectionType GetProjectionType() { return m_projectionType; }
		inline void SetProjectionType(const EProjectionType projectionType) { m_projectionType = projectionType; }

		inline float GetNear() const { return m_near; } 
		inline float GetFar() const { return m_far; }

		inline const Mat4x4& GetTransform() const { return  m_transform; }
		inline const Mat4x4& GetView() const { return m_view; }
		inline const Mat4x4& GetProjection() const { return m_projection; }

		inline Mat4x4 GetViewProjection() { return m_projection * m_view; }

		inline Vector2f GetCurrentJitter() { return m_currentJitter; }
		inline Vector2f GetPreviousJitter() { return m_prevJitter; }

		inline const Vector3f& GetPosition() { return m_position; }
		inline void SetPosition(Vector3f position) { m_position = position; }

		const Vector3f GetScreenToWorldPos(const Vector2f& screenPos) const;

		void SetTransform(const Mat4x4& transform)
		{
			m_transform = transform;
			UpdateProjection();
		}

		Vector3f GetUpVector() const;
		Vector3f GetRightVector() const;
		Vector3f GetForwardVector() const;
		void Resize(float width, float height);

		inline void SetRotation(Vector3f rotation)
		{
			m_pitch = rotation.x * QMaths::Deg2Rad;
			m_yaw = rotation.y * QMaths::Deg2Rad;
			m_roll = rotation.z * QMaths::Deg2Rad;
		}

		inline void SetForwardVector(Vector3f forward) { m_forward = forward; }
		inline void SetRightVector(Vector3f right) { m_right = right; }
		inline void SetTAASystem(Ref<QRender::TAASystem> inTAASystem) { m_taaSystem = inTAASystem; }
		inline void SetFSR2System(Ref<QRender::FSR2System> inFSR2System) { m_fsr2System = inFSR2System; }
		inline void SetDLSSSystem(Ref<QRender::DLSSSystem> inDLSSSystem) { m_dlssSystem = inDLSSSystem; }
	
		static EditorCamera* GetMain()
		{
			return m_mainCamera;
		}

		static void SetMain(EditorCamera* camera)
		{
			m_mainCamera = camera;
		}

		static EditorCamera* m_mainCamera;
	
	private:


		void ResizeEvent(const QCore::WindowResizeEvent& event);

		EProjectionType m_projectionType;

		// MVP
		Mat4x4 m_transform;
		Mat4x4 m_view;
		Mat4x4 m_projection;

		float m_pitch = 0, m_yaw = 0, m_roll = 0;

		// Transform
		Vector3f m_position{ 0,20,-100.0f };
		Vector3f m_rotation{ 0,0,0 };
		Vector3f m_scale{ 1,1,1 };
		Vector3f m_focalPoint{ 0,0,0 };
		Vector3f m_forward{ 0, 0, 1 };
		Vector3f m_right{ 1, 0, 0 };
		float m_distance = 10;

		Vector2f m_viewportSize{ 1280, 720 };

		Vector2f m_prevJitter;
		Vector2f m_currentJitter;

		float m_zoom = 8;
		float m_aspectRatio = 1.0f;
		float m_near = 1.0f, m_far = 10000.0f;
		float m_fov = 90.0f;


		Ref<QRender::TAASystem> m_taaSystem;
		Ref<QRender::FSR2System> m_fsr2System;
		Ref<QRender::DLSSSystem> m_dlssSystem;
	};


	class SceneCamera
	{
	public:
		SceneCamera() {};
		SceneCamera(const InstancedCamera& instancedCamera);
		~SceneCamera();

		inline const Mat4x4& GetView() const { return m_view; }
		inline const Mat4x4& GetProjection() const { return m_projection; }

		inline const Mat4x4& GetInverseView() const { return glm::inverse(m_projection); }
		inline const Mat4x4& GetInverseProjection() const { return glm::inverse(m_projection); }

		void Update(const InstancedCamera& instancedCamera);
	protected:

	private:
		Vector4f m_position{ 0.0f, 0.0f, 0.0f, 1.0f };
		Vector4f m_rotation{ 0.0f, 0.0f, 0.0f, 1.0f };
		Vector4f m_scale{ 1.0f, 1.0f, 1.0f, 1.0f };

		// MVP
		Mat4x4 m_transform;
		Mat4x4 m_view;
		Mat4x4 m_projection;

		float m_zoom = 8;
		float m_aspectRatio = 1.0f;
		float m_near = 1.0f, m_far = 10000.0f;
		float m_fov = 90.0f;

		Vector4f m_forward{ 0.0f, 0.0f, 1.0f, 1.0f };
		Vector4f m_viewportRect{ 0.0f, 0.0f, 1280, 720 };
		//Projection type
		EProjectionType m_projectionType;
	};

}