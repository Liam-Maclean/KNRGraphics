#include "KNR_ShaderInterop.h"

struct VertexInput
{
    float4 Position     : POSITION;
    float4 Normal       : NORMAL;
    float2 TexCoord     : TEXCOORD;
};

struct VertexInputSkinned
{
    float4 Position     : POSITION;
    float4 BlendWeights : BLENDWEIGHT;
    float4 BlendIndices : BLENDINDICES;
    float4 Normal       : NORMAL;
    float2 TexCoord     : TEXCOORD;
};

struct VertexOutput
{
    float4 Position     : SV_Position;
    float2 TexCoord     : TEXCOORD;
    float4 Normal       : NORMAL;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

ConstantBuffer CameraMatrices
{
    float4x4 g_MVPMatrix : MVPMatrix;
}

BindlessTexture g_diffuseTexture : DiffuseTexture;

VertexOutput VS_Draw(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, g_MVPMatrix);
    output.TexCoord = input.TexCoord;
    output.Normal = input.Normal;

    return output;
}

VertexOutput VS_DrawSkinned(VertexInputSkinned input)
{
    VertexOutput output;
    output.Position = mul(input.Position, g_MVPMatrix);
    output.TexCoord = input.TexCoord;
    output.Normal = input.Normal;

    return output;
}

PixelOutput PS_Draw(VertexOutput input)
{
    PixelOutput output;
    output.Color = float4(1.0f, 0.0f, 0.0f, 0.0f);

    return output;
}