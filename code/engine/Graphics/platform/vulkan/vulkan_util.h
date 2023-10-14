namespace KNR
{
	namespace Util
	{
		static void ErrorCheck(VkResult result);
		static VkFormat GetVulkanFormatFromGenericType(FramebufferTextureFormat textureFormat);
	}
}