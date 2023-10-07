//G Buffer
#include "LightUtil.hlsli"

Texture2D gAlbedoRoughness : register(t0);
Texture2D gNormalMetallic : register(t1);
Texture2D gEmissive : register(t2);
Texture2D depthTx : register(t3);
TextureCube cubeMap : register(t4);
Texture2D brdfLUT : register(t5);
Texture2D shadowMapTx : register(t6);
SamplerState s1 : register(s0);
SamplerState anisotropicSampler : register(s1);

//Structure of the Cbuffer for materials
struct DirectionalLightCBuffer
{
	float3 lightPos;
	float3 lightDir;
	float3 color;
};

struct ShaderDebugValues
{
	int HDREnabled;
	int GammaCorrectionEnabled;
	int viewMode;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL;
	float2 texCoord : TEXCOORD;
};

cbuffer constantBuffer : register(b0)
{
	matrix  viewMat;
	matrix  projMat;
	matrix  modelMat;
	matrix  inverseViewMat;
	matrix  inverseProjMat;
	matrix  invertedTransposeModelMat;
	matrix  directionalLightMat;
	float4 cameraPos;
};

cbuffer DirectionalLightBuffer : register(b3)
{
	DirectionalLightCBuffer directionalLightBuffer;
}

cbuffer DebugShaderValues : register(b4)
{
	ShaderDebugValues debugValues;
}


float3 GetPositionVS(float2 texcoord, float depth)
{
	float4 clipSpaceLocation;
	clipSpaceLocation.x = texcoord.x * 2.0 - 1.0;
	clipSpaceLocation.y = (1 - texcoord.y) * 2.0 - 1.0;
	clipSpaceLocation.z = depth;
	clipSpaceLocation.w = 1.0f;
	float4 homogenousLocation = mul(clipSpaceLocation, inverseProjMat);

	//flip the z? why?
	//homogenousLocation.z = -homogenousLocation.z;

	//if (depth == 1)
	//{
		//return float3(0.0, 0.0, 0.0);
	//}
	return homogenousLocation.xyz /= homogenousLocation.w;
}

float3 GetPositionWS(float2 texcoord, float depth)
{
	float4 clipSpaceLocation;
	clipSpaceLocation.x = texcoord.x * 2.0 - 1.0;
	clipSpaceLocation.y = (1 - texcoord.y) * 2.0 - 1.0;
	clipSpaceLocation.z = depth;
	clipSpaceLocation.w = 1.0f;
	float4 homogenousLocation = mul(clipSpaceLocation, inverseProjMat);

	//flip the z? why?
	//homogenousLocation.z = -homogenousLocation.z;

	float4 worldLocation = mul(homogenousLocation, inverseViewMat);
	return worldLocation.xyz /= worldLocation.w;
}


float ShadowCalculation(float4 fragPosLightSpace)
{
	//Perform perspective divide
	float3 projCoords;
	projCoords.x = fragPosLightSpace.x / fragPosLightSpace.w / 2.0 + 0.5f;
	projCoords.y = -fragPosLightSpace.y / fragPosLightSpace.w / 2.0 + 0.5f;
	projCoords.z = fragPosLightSpace.z / fragPosLightSpace.w;

	//Get the closest depth from the lights perspective
	float closestDepth = shadowMapTx.Sample(s1, projCoords.xy).r;

	//depth bias
	closestDepth = closestDepth - 0.001f;

	//Get the depth of the current fragment from the lights perspective
	float currentDepth = projCoords.z;

	//Check whether the current position is in the shadow
	//liam reverse Z depth so this comparison is back to front 
	//(Depth starts at 0, goes to 1)
	float shadow = currentDepth > closestDepth ? 0.0 : 1.0;

	return shadow;
}


float4 main(VS_OUTPUT input) : SV_TARGET
{

	float perceptualRoughness;
	float3 diffuseColor;
	float4 baseColor;
	float shadow = 0;
	//G buffer
	float depth = depthTx.Sample(s1, input.texCoord).r;

	if (depth == 0.0f)	
	{
		//Get world view position from depth
		float3 worldPos = GetPositionWS(input.texCoord, depth);
		



		//worldPos = mul(worldPos, inverseViewMat);
		float3 samplePos = normalize(worldPos - cameraPos);
		float3 cubemapColor = cubeMap.Sample(anisotropicSampler, samplePos.xyz).xyz;
	
		return float4 (cubemapColor, 1.0);
	}

	

	float3 worldPos = GetPositionWS(input.texCoord, depth);
	float4 lightPosWorld = float4(worldPos.xyz, 1.0f);
	float4 lightSpaceShadowPos = mul(lightPosWorld, directionalLightMat);
	shadow = ShadowCalculation(lightSpaceShadowPos);
	

	float3 normal = gNormalMetallic.Sample(s1, input.texCoord).xyz;
	float metallic = gNormalMetallic.Sample(s1, input.texCoord).a;
	gEmissive.Sample(s1, input.texCoord).a;
	perceptualRoughness = gAlbedoRoughness.Sample(s1, input.texCoord).a;

	float3 f0 = float3(0.04, 0.04, 0.04);

	//baseColor = float4(1.0, 1.0, 1.0, 1.0);
	baseColor = SRGBtoLINEAR(gAlbedoRoughness.Sample(s1, input.texCoord));

	diffuseColor = baseColor.rgb * (float3(1.0, 1.0, 1.0) - f0);
	diffuseColor *= 1.0 - metallic;

	float alphaRoughness = perceptualRoughness * perceptualRoughness;

	float3 specularColor = lerp(f0, baseColor.rgb, metallic);

	// Compute reflectance.
	float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

	// For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
	// For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
	float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
	float3 specularEnvironmentR0 = specularColor.rgb;
	float3 specularEnvironmentR90 = float3(1.0, 1.0, 1.0) * reflectance90;

	float3 n = normal;
	float3 v = normalize(cameraPos - worldPos);    // Vector from surface point to camera
	float3 l = normalize(-directionalLightBuffer.lightDir);     // Vector from surface point to light
	float3 h = normalize(l + v);                        // Half vector between both l and v
	float3 reflection = -normalize(reflect(v, n));
	reflection.y *= -1.0f;

	float NdotL = clamp(dot(n, l), 0.001, 1.0);
	float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
	float NdotH = clamp(dot(n, h), 0.001, 1.0);
	float LdotH = clamp(dot(l, h), 0.001, 1.0);
	float VdotH = clamp(dot(v, h), 0.001, 1.0);

	// Calculate the shading terms for the microfacet specular shading model
	float3 F = specularReflection(reflectance, reflectance90, VdotH);
	float G = geometricOcclusion(NdotL, NdotV, perceptualRoughness);
	float D = microfacetDistribution(NdotH, perceptualRoughness);

	//const float3 u_LightColor = vec3(1.0);

	// Calculation of analytical lighting contribution
	float3 diffuseContrib = (1.0 - F) * (diffuseColor * M_PI);
	float3 specContrib = F * G * D / (4.0 * NdotL * NdotV);


	float3 color = NdotL * directionalLightBuffer.color * (diffuseContrib + specContrib);

	if (shadow == 1.0f)
	{
		//Truncate the output (shadow)
		color = color * 0.2f;
	}
	// Obtain final intensity as reflectance (BRDF) scaled by the energy of the light (cosine law)
	
	//Ambient occlusion
	//float ao = gEmissive.Sample(s1, input.texCoord).a;
	//if (ao > 0.0f)
	//{
	//	color = lerp(color, color * ao, 1.0f);
	//}

	//IBL ibl;
	//ibl.reflectivity = float3(0.5, 0.5, 0.5);
	//ibl = ImageBasedLighting(alphaRoughness, metallic, f0, baseColor.rgb, normal.xyz, v, cubeMap, brdfLUT, s1);
	//color += ibl.final_color * 0.2f;

	//emissive
	//float3 emissive = SRGBtoLINEAR(float4(gEmissive.Sample(s1, input.texCoord).xyz, 1.0)).xyz;
	//if ())
	//{
	//	color += emissive;
	//}


	//emissive

	if (debugValues.viewMode == 0)
	{
		return float4(color, 1.0);
	}
	else if (debugValues.viewMode == 1)
	{
		return baseColor; //Textures
	}
	else if (debugValues.viewMode == 2)
	{
		return float4(normal, 1.0); //Normals
	}
	else if (debugValues.viewMode == 3)
	{
		//return float4(1.0, 1.0, 1.0, 1.0);
		return float4(gEmissive.Sample(s1, input.texCoord));
	}
	else if (debugValues.viewMode == 4)
	{
		return float4(worldPos, 1.0);
	}

	return float4(color, 1.0);
}
