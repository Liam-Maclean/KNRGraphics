#ifndef __KNR_SHADER_INTEROP_INCLUDE
#define __KNR_SHADER_INTEROP_INCLUDE

/* Shared constant structs with the compiler and C++ */
#if __cplusplus
/// <C++ redefines>
///
/// Do C++ redefines here if you're using an external library for math, want to do bit wise optimisations
/// 
/// </summary>
#include <DirectXMath.h>
using namespace DirectX;
#define float4x4 XMMATRIX
#define float4 XMFLOAT4
#define float3 XMFLOAT3
#define float2 XMFLOAT2

//Wrap in a namespace for C++ namespace cleanup
namespace KNR
{

#endif

/// <Shared HLSL Defines>
///
/// These types, classes and structs are shared between HLSL and C++
/// if you are sending data to a shader, you must add it here, not anywhere else
/// 
/// You can add a new Getter for buffer types by using the templated load function
/// 
/// </summary>

/* Camera data */
struct CameraData
{
    float4x4 MVPMatrix;
    float4x4 ModelMatrix;
    float4x4 InverseViewMatrix;
    float4x4 InverseProjectionMatrix;
    float4 EyePos;
    float z_far;
    float z_near;
};

/* Eight material indices should be more than enough. First four named, rest are misc */
struct MaterialIndexingData
{
    int textureIndexDiffuse;
    int textureIndexNormalMap;
    int textureIndexRoughness;
    int textureIndexMetallic;
    int textureIndexMisc0;
    int textureIndexMisc1;
    int textureIndexMisc2;
    int textureIndexMisc3;
};

struct ConstantIndexingData
{
    int constantIndexCameraDataBuffer;
    int constantIndexFrameDataBuffer;
};

struct FrameData
{
    float deltaTime;
    int frameCount;
};

#if __cplusplus
}

/* shader specific code */ 
#else !__cplusplus 

/// <Shared specific code>
///
/// Shader specific HLSL code, registers and macros 
/// 
/// </summary>

#define PASTE1(a, b) a##b
#define PASTE(a, b) PASTE1(a, b)
#define CBUFFER(name, slot) cbuffer name : register(PASTE(b, slot))
#define CONSTANTBUFFER(name, type, slot) ConstantBuffer< type > name : register(PASTE(b, slot))

//Registers
Texture1D bindlessTextures1D[] : register(t0, space0);
Texture2D bindlessTextures[] : register(t0, space1);
Texture2DArray bindlessTextures2DArray[] : register(t0, space2);
TextureCube bindlessCubeMaps[] : register(t0, space3);
TextureCubeArray bindlessCubearrays[] : register(t0, space4);
Texture3D bindlessTextures3D[] : register(t0, space5);
ByteAddressRegister bindlessBuffers[] : register(t0, space6);
CONSTANTBUFFER(TextureIndexingBuffer, MaterialIndexingData, 0);  //b0
CONSTANTBUFFER(ConstantIndexingBuffer, ConstantIndexingData, 1); //b1

Texture2D GetDiffuse() { return bindlessTextures[TextureIndexingBuffer.textureIndexDiffuse];}
Texture2D GetNormal() { return bindlessTextures[TextureIndexingBuffer.textureIndexDiffuse];}
Texture2D GetRoughness() { return bindlessTextures[TextureIndexingBuffer.textureIndexRoughness];}
Texture2D GetMetallic() { return bindlessTextures[TextureIndexingBuffer.textureIndexMetallic];}
Texture2D GetMisc0() { return bindlessTextures[TextureIndexingBuffer.textureIndexMisc0];}
Texture2D GetMisc1() { return bindlessTextures[TextureIndexingBuffer.textureIndexMisc1];}
Texture2D GetMisc2() { return bindlessTextures[TextureIndexingBuffer.textureIndexMisc2];}
Texture2D GetMisc3() { return bindlessTextures[TextureIndexingBuffer.textureIndexMisc3];}
CameraData GetCamera() { return bindlessBuffers[ConstantIndexingBuffer.constantIndexCameraDataBuffer];}
FrameData GetFrame() { return bindlessBuffers[ConstantIndexingBuffer.constantIndexFrameDataBuffer];}

#endif

#endif //__KNR_SHADER_INTEROP_INCLUDE