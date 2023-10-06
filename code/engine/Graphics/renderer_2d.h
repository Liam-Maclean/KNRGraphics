#pragma once

#include "core/core.h"
#include "entt/entt.hpp"

#include "assets/asset_database.h"

#include "maths/maths.h"
#include "maths/geometry.hpp"

#include "graphics/vertex.h"

#include <memory>
#include <map>

class AssetDatabase;
struct SpriteComponent
{
	uint32_t TextureID = 0;
	QMaths::Rectf Source;
};


//Liam - Defunct class now, doesn't work as of (02/02/2023)
//If you want to use this, you'll have to revert back to OGL in the git history
//to a time pre-dx12 integration
namespace QRender
{
	class Texture2D;
	class EditorCamera;
	class Renderer2D
	{
	public:
		static void Initialise();
		static void Load(const AssetList& assetList, AssetDatabase& assetDatabase);
		static void Begin(EditorCamera& camera);
		static void Draw(const uint32_t& textureID, const Vector3f& position, const QMaths::Rectf& source, const Vector3f& rotation, const Vector3f& scale, const Vector4f& color);
		static void Draw(const uint32_t& textureID, const Vector3f& position, const Vector3f& rotation, const Vector3f& scale, const Vector4f& color);
		static void Draw(Ref<Texture2D> texture, const Vector3f& position, const Vector3f& rotation, const Vector3f& scale, const Vector4f& color);
		static void End();
	private:
		static void NextBatch();
		static void Flush();
	};
}