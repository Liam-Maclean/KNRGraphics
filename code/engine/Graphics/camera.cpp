#include <qlpch.h>
#include "camera.h"
#include "render_types.h"
#include "graphics/systems/ffx_fsr2_system.h"
#include "graphics/systems/taa_system.h"
#include "graphics/systems/dlss_system.h"

#include "events/application_events.h"

QRender::EditorCamera* QRender::EditorCamera::m_mainCamera = nullptr;

//TODO(CK): Remove glm references, replace with interface from maths.h 
namespace QRender
{

	EditorCamera::EditorCamera(EProjectionType projectionType)
	{
		m_projectionType = projectionType;

		UpdateProjection();

		if (!GetMain())
		{
			SetMain(this);
		}
	}

	void EditorCamera::OnUpdate(const GameTime& gameTime)
	{
		UpdateProjection();
		UpdateView();

		Mat4x4 identity = glm::identity<Mat4x4>();
		m_transform = glm::translate(identity, m_position);
		//glm::rotate(GetOrientation(), m_rotation);
		//m_transform *= glm::scale(identity, m_scale);
	}

	void EditorCamera::OnEvent(QCore::Event& event)
	{

	}

	const Vector3f EditorCamera::GetScreenToWorldPos(const Vector2f& posInScreen) const
	{
		Vector3f worldPos = glm::unProject(Vector3f(posInScreen.x, posInScreen.y, 1.0f), GetView(), GetProjection(), Vector4f(0.0f, 0.0f, m_viewportSize.x, m_viewportSize.y));
		return Vector3f(worldPos.x, -worldPos.y, 0);
	}

	Vector3f EditorCamera::GetUpVector() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	Vector3f EditorCamera::GetRightVector() const
	{
		return m_right;
	}

	Vector3f EditorCamera::GetForwardVector() const
	{
		return m_forward;
	}

	void EditorCamera::ResizeEvent(const QCore::WindowResizeEvent& event)
	{

	}

	void EditorCamera::Resize(float width, float height)
	{
		m_viewportSize = { width, height };
	}

	void EditorCamera::UpdateProjection()
	{
		m_aspectRatio = m_viewportSize.x / m_viewportSize.y;
		float aspectRatioY = m_viewportSize.y / m_viewportSize.x;
		switch (m_projectionType)
		{
		case EProjectionType::Orthographic:
			//m_projection = glm::orthoRH(-(m_viewportSize.x/2.0f), (m_viewportSize.x /2.0f), -(m_viewportSize.y / 2.0f), (m_viewportSize.y /2.0f), -100.0f, 100.0f);
			m_projection = glm::orthoRH(-200.0f, 200.0f, -200.0f, 200.0f, 200.0f, 1.0f);
			break;
		case EProjectionType::Perspective:

			
			m_projection = glm::perspectiveRH(glm::radians(m_fov), (float)m_viewportSize.x / m_viewportSize.y, m_far, m_near);

			//if we're using TAA, grab the jitter formula from that system
			if (m_taaSystem != nullptr)
			{
				m_prevJitter = m_currentJitter;

				m_taaSystem->CalculateJitter();

				m_currentJitter = m_taaSystem->GetProjectionJitter();

				glm::mat4 jitterMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(m_currentJitter.x, m_currentJitter.y, 0.0f));

				m_projection = jitterMatrix * m_projection;
			}
			//if we're using FSR2, grab the jitter formula from that system
			else if (m_fsr2System != nullptr)
			{
				m_prevJitter = m_currentJitter;

				m_fsr2System->CalculateJitter();

				m_currentJitter = m_fsr2System->GetProjectionJitter();

				glm::mat4 jitterMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(m_currentJitter.x, m_currentJitter.y, 0.0f));

				m_projection = jitterMatrix * m_projection;
			}
			else if (m_dlssSystem != nullptr)
			{
				m_prevJitter = m_currentJitter;

				m_dlssSystem->CalculateJitter();

				m_currentJitter = m_dlssSystem->GetProjectionJitter();

				glm::mat4 jitterMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(m_currentJitter.x, m_currentJitter.y, 0.0f));

				m_projection = jitterMatrix * m_projection;
			}
			//m_projection = MakeInfReversedZProjRH(glm::radians(m_fov), (float)m_viewportSize.x / m_viewportSize.y, 0.01f);
			break;
		default:
			break;
		}
	}

	void EditorCamera::UpdateView()
	{
		switch (m_projectionType)
		{
		case EProjectionType::Orthographic:
			//if orthographic we want to be looking at the scenes' center point for now. I'll fix this later so we can get it more accurate
			m_view = glm::lookAtRH(m_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case EProjectionType::Perspective:
			m_view = glm::lookAtRH(m_position, m_position + GetForwardVector(), glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		default:
			break;
		}
	}


	SceneCamera::SceneCamera(const InstancedCamera& instancedCamera)
	{
		m_position = instancedCamera.position;
		m_zoom = instancedCamera.zoom;
		m_fov = instancedCamera.fov;
		m_near = instancedCamera.nearClip;
		m_far = instancedCamera.farClip;
		m_viewportRect = Vector4f(instancedCamera.viewportXY, instancedCamera.viewportWH);
		m_view = glm::lookAtRH(Vector3f(m_position), Vector3f(m_position + m_forward), glm::vec3(0.0f, 1.0f, 0.0f));

		if (instancedCamera.orthographic)
		{
			m_projection = glm::orthoRH(-200.0f, 200.0f, -200.0f, 200.0f, 200.0f, 1.0f);
			m_projectionType = EProjectionType::Orthographic;
		}
		else
		{
			m_projection = glm::perspectiveRH(glm::radians(m_fov), (float)m_viewportRect.z / m_viewportRect.w, m_far, m_near);
			m_projectionType = EProjectionType::Perspective;
		}
	}

	SceneCamera::~SceneCamera()
	{

	}

	void SceneCamera::Update(const InstancedCamera& instancedCamera)
	{
		m_position = instancedCamera.position;
		m_zoom = instancedCamera.zoom;
		m_fov = instancedCamera.fov;
		m_near = instancedCamera.nearClip;
		m_far = instancedCamera.farClip;
		m_view = glm::lookAtRH(Vector3f(m_position), Vector3f(m_position + m_forward), glm::vec3(0.0f, 1.0f, 0.0f));
		m_viewportRect = Vector4f(instancedCamera.viewportXY, instancedCamera.viewportWH);
		if (instancedCamera.orthographic)
		{
			m_projection = glm::orthoRH(-200.0f, 200.0f, -200.0f, 200.0f, 200.0f, 1.0f);
			m_projectionType = EProjectionType::Orthographic;
		}
		else
		{
			m_projection = glm::perspectiveRH(glm::radians(m_fov), (float)m_viewportRect.z / m_viewportRect.w, m_far, m_near);
			m_projectionType = EProjectionType::Perspective;
		}
	}

}