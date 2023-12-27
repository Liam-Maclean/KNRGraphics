#include "KNR_ShaderInterop.h"

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

PixelOutput PS_Draw(VertexOutput input)
{
    PixelOutput output;
    output.Color = float4(1.0f, 0.0f, 0.0f, 0.0f);

    return output;
}