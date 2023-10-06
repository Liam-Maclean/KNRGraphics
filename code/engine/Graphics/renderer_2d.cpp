#include <qlpch.h>
#include "renderer_2d.h"

#include "ecs/component.h"

#include "graphics/renderer_api.h"
#include "graphics/render_commands.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "graphics/vertex.h"
#include "graphics/vertex_array.h"
#include "graphics/buffer.h"
#include "graphics/uniform_buffer.h"
#include "graphics/camera.h"

#include "assets/asset.h"
#include "assets/asset_database.h"

#include "maths/maths.h"

namespace QRender
{
	void Renderer2D::Initialise()
	{
		
	}

	void Renderer2D::Load(const AssetList& assetList, AssetDatabase& assetDatabase)
	{
		
	}

	void Renderer2D::Begin(Camera& camera)
	{
		
	}

	void Renderer2D::Draw(Ref<Texture2D> texture, const Vector3f& position, const Vector3f& rotation, const Vector3f& scale, const Vector4f& color)
	{
	
	}

	void Renderer2D::Draw(const uint32_t& textureID, const Vector3f& position, const QMaths::Rectf& source, const Vector3f& rotation, const Vector3f& scale, const Vector4f& color)
	{
		
	}

	void Renderer2D::Draw(const uint32_t& textureID, const Vector3f& position, const Vector3f& rotation, const Vector3f& scale, const Vector4f& color)
	{
	
	}

	void Renderer2D::End()
	{
		
	}

	void Renderer2D::NextBatch()
	{
		
	}

	void Renderer2D::Flush()
	{
	
	}
}