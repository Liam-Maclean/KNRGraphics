
static float M_PI = 3.141592653589793;


//SRGB to Linear 
float4 SRGBtoLINEAR(float4 srgbIn)
{
	float3 linOut = pow(srgbIn.xyz, float3(2.2, 2.2, 2.2));
	return float4(linOut, srgbIn.w);
}

//Reinhard tonemapping
float3 Reinhard(float3 color)
{
	return color / (1.0 + color);
}

//Gamma correction
float3 GammaCorrection(float3 color)
{
	//Gamma = 2.2
	return pow(color, (float3(1.0, 1.0, 1.0) / 2.2));
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
float3 specularReflection(float3 reflectance0, float3 reflectance90, float VdotH)
{
	return reflectance0 + (reflectance90 - reflectance0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
float geometricOcclusion(float NdotL, float NdotV, float r)
{
	//sfloat NdotL = pbrInputs.NdotL;
	//sfloat NdotV = pbrInputs.NdotV;
	//sfloat r = pbrInputs.alphaRoughness;

	float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
	float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
	return attenuationL * attenuationV;
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	float smoothness = 1.0 - roughness;
	return F0 + (max(float3(smoothness, smoothness, smoothness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}


float3 GetSpecularDominantDir(float3 normal, float3 reflection, float roughness)
{
	const float smoothness = 1.0f - roughness;
	const float lerpFactor = smoothness * (sqrt(smoothness) + roughness);
	return lerp(normal, reflection, lerpFactor);
}

// From Panos Karabelas https://github.com/PanosK92/SpartanEngine/blob/master/Data/shaders/IBL.hlsl
float3 SampleEnvironment(TextureCube tex_environment, float3 dir, float mipLevel, SamplerState s1)
{
	float height;
	float width;

	tex_environment.GetDimensions(width, height);

	float2 texDimensions = float2(width, height);
	float2 resolution = float2(float(texDimensions.x), float(texDimensions.y));
	float2 texelSize = 1.0 / resolution;

	float dx = 1.0f * texelSize.x;
	float dy = 1.0f * texelSize.y;
	float dz = 0.5f * (texelSize.x + texelSize.y);

	float3 d0 = tex_environment.Sample(s1, dir + float3(0.0, 0.0, 0.0), mipLevel).rgb;
	float3 d1 = tex_environment.Sample(s1, dir + float3(-dx, -dy, -dz), mipLevel).rgb;
	float3 d2 = tex_environment.Sample(s1, dir + float3(-dx, -dy, +dz), mipLevel).rgb;
	float3 d3 = tex_environment.Sample(s1, dir + float3(-dx, +dy, +dz), mipLevel).rgb;
	float3 d4 = tex_environment.Sample(s1, dir + float3(+dx, -dy, -dz), mipLevel).rgb;
	float3 d5 = tex_environment.Sample(s1, dir + float3(+dx, -dy, +dz), mipLevel).rgb;
	float3 d6 = tex_environment.Sample(s1, dir + float3(+dx, +dy, -dz), mipLevel).rgb;
	float3 d7 = tex_environment.Sample(s1, dir + float3(+dx, +dy, +dz), mipLevel).rgb;

	return (d0 + d1 + d2 + d3 + d4 + d5 + d6 + d7) * 0.125f;
}


//IBL 
struct IBL { float3 final_color; float3 reflectivity; };
IBL ImageBasedLighting(float roughness, float metallic, float F0, float3 albedo, float3 normal, float3 camera_to_pixel, TextureCube tex_environment, Texture2D tex_lutIBL, SamplerState s1)
{
	IBL ibl;

	float3 reflection = reflect(camera_to_pixel, normal);
	// From Sebastien Lagarde Moving Frostbite to PBR page 69
	reflection = GetSpecularDominantDir(normal, reflection, roughness);

	float NdV = saturate(dot(-camera_to_pixel, normal));
	float3 F = FresnelSchlickRoughness(NdV, F0, roughness);

	float3 kS = F;// The energy of light that gets reflected
	float3 kD = 1.0f - kS;// Remaining energy, light that gets refracted
	kD *= 1.0f - metallic;

	// Diffuse
	float3 irradiance = SampleEnvironment(tex_environment, normal, 8, s1);
	float3 cDiffuse = irradiance * albedo;

	// Specular
	float mipLevel = max(0.001f, roughness * roughness) * 11.0f;// max lod 11.0f
	float3 prefilteredColor = SampleEnvironment(tex_environment, reflection, mipLevel, s1);
	float2 envBRDF = tex_lutIBL.Sample(s1, float2(NdV, roughness)).xy;
	ibl.reflectivity = F * envBRDF.x + envBRDF.y;
	float3 cSpecular = prefilteredColor * ibl.reflectivity;

	ibl.final_color = kD * cDiffuse + cSpecular;

	return ibl;
}

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
float microfacetDistribution(float NdotH, float r)
{
	float roughnessSq = r * r;
	float f = (NdotH * roughnessSq - NdotH) * NdotH + 1.0;
	return roughnessSq / (M_PI * f * f);
}

float3 getNormal(float3 inNormal, Texture2D txNormal, SamplerState s1, float3 worldPos, float2 inUV)
{
	float3 bumpedNormal = txNormal.Sample(s1, inUV).xyz * 2.0 - float3(1.0, 1.0, 1.0);
	float3 q1 = ddx(worldPos);
	float3 q2 = ddy(worldPos);
	float2 st1 = ddx(inUV);
	float2 st2 = ddy(inUV);

	float3 N = normalize(inNormal);
	float3 T = normalize(q1 * st2.y - q2 * st1.y);
	float3 B = normalize(cross(N, T));
	float3x3 TBN = float3x3(T, B, N);

	float3 output = mul(bumpedNormal, TBN);
	return output;
}

// ----------------------------------------------------------------------------
float DistributionGGX(float3 N, float3 H, float roughness)
{
	float PI = 3.141592653589793;
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}