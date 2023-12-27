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