
//All the textures we need
Texture2D Tex2DArray[] : register(t5, space1);
SamplerState s1 : register(s0);
SamplerState anisotropicSampler : register(s1);

struct VS_INPUT
{
	float3 pos : POSITION0;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
	float texindex : TEXINDEX;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
	float texindex : TEXINDEX;
};

cbuffer CameraBuffer : register (b0)
{
	matrix viewProjection;
	float3 u_CameraPosition;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
	output.color = input.color;
	output.texcoord = input.texcoord;
	output.texindex = input.texindex;
	output.pos.w = 1.0f;
	//asd
	output.pos = mul(viewProjection, input.pos);

	return output;
}