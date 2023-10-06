//All the textures we need
Texture2D Tex2DArray[] : register(t0, space1);
SamplerState s1 : register(s0);
SamplerState anisotropicSampler : register(s1);

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
	float texindex : TEXINDEX;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float4 texCol = input.color * Tex2DArray[input.texindex].Sample(anisotropicSampler,float2(input.texcoord)); 
	//float3 normal = normalize(input.norm);
	return float4(1,0,0,1);
	// texCol;
	//for(int i= 0; i < NumUsedLights; i++)
	//{
	//	vec3 lightDir = normalize(u_Lights[i].Position - Input.Position);
	//	float diff = max(dot(normal, lightDir), 0.0);
	//	vec4 diffuse = diff * vec4(1,1,1,1);
	//	color += clamp(diffuse, 0.0, 1.0);
	//
	//}
}