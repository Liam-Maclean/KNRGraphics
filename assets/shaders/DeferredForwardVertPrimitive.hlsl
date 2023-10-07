struct VS_INPUT
{
	float4 pos : POSITION0;
	float4 norm : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 debugColor : WORLD0;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL;
	float2 texCoord : TEXCOORD;
	float2 texcoord2 : WORLD0;
};

cbuffer constantBuffer : register(b0)
{
	matrix  viewMat;
	matrix  projMat;
	matrix  modelMat;	
	matrix  inverseViewMat;
	matrix  inverseProjMat;
	matrix  invertedTransposeModelMat;
	float4 cameraPos;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.pos.w = 1.0f;
	output.pos.w = 1.0f;
	output.pos = mul(output.pos, modelMat);
	output.pos = mul(output.pos, viewMat);
	output.pos = mul(output.pos, projMat);

	output.norm = mul(input.norm, invertedTransposeModelMat);
	output.texCoord = input.texCoord;
	return output;
}