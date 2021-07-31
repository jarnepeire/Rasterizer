#pragma once
#include "pch.h"
#include "BRDF.h"
using namespace Elite;

Elite::RGBColor BRDF::Lambert(const Elite::RGBColor& diffColor, float diffReflectance)
{
	return diffColor * diffReflectance / ((float)E_PI / 2.f);
}

Elite::RGBColor BRDF::Lambert(const Elite::RGBColor& albedoColor, const Elite::RGBColor& kd)
{
	return albedoColor * kd / (float)E_PI;
}

Elite::RGBColor BRDF::Phong(float specReflectance, float phongExp, const Elite::FVector3& lightDir, const Elite::FVector3& invViewDir, const Elite::FVector3& hitNormal)
{
	Elite::FVector3 reflect = lightDir - 2 * Elite::Dot(hitNormal, lightDir) * hitNormal;
	float specStrength = Elite::Clamp(Elite::Dot(reflect, invViewDir), 0.f, 1.f);

	float phongSpecReflect = specReflectance * (powf(specStrength, phongExp));
	Elite::RGBColor phongSpecReflect_color{ phongSpecReflect, phongSpecReflect, phongSpecReflect };

	return phongSpecReflect_color;
}

Elite::RGBColor BRDF::LambertCookTorrance(const Elite::RGBColor& albedoColor, int metalness, float roughness, const Elite::FVector3& lightDir, const Elite::FVector3& invViewDir, const Elite::FVector3& hitNormal)
{
	//Roughness Squared
	float roughnessSquared = (roughness * roughness);

	//Determine F0 value (0.04, 0.04, 0.04) or Albedo based on Metalness
	Elite::RGBColor F0 = (metalness == 0) ? Elite::RGBColor(0.04f, 0.04f, 0.04f) : albedoColor;

	//Calculate halfVector between view and light
	FVector3 halfVector = GetNormalized(invViewDir + lightDir);

	//Re-usable dot products for functions (clamp needed to prevent overflow of color issue!)
	float dotNL = Clamp(Elite::Dot(hitNormal, lightDir), 0.f, 1.f);
	float dotNV = Clamp(Elite::Dot(hitNormal, invViewDir), 0.f, 1.f);
	float dotNH = Clamp(Elite::Dot(hitNormal, halfVector), 0.f, 1.f);
	float dotVH = Clamp(Elite::Dot(invViewDir, halfVector), 0.f, 1.f);

	//Calculate Fresnel (F)
	Elite::RGBColor fresnel = BRDF::Fresnel(dotVH, F0);

	//Calculate Normal Distribution
	float normalDistribution = BRDF::NormalDistribution(dotNH, roughnessSquared);

	//Calculate geometry (Smith's function)
	float kDirect = powf(roughnessSquared + 1, 2) / 8.f;
	float geometry = BRDF::GeometryMasking(dotNV, kDirect) * BRDF::GeometryShadowing(dotNL, kDirect);

	//Calculate specular
	RGBColor specular = CalculateSpecular(normalDistribution, fresnel, geometry, dotNL, dotNV);

	//Determine KD
	Elite::RGBColor kd = (metalness == 0) ? (Elite::RGBColor(1, 1, 1) - fresnel) : Elite::RGBColor(0, 0, 0);

	//Calculate diffuse
	Elite::RGBColor diffuse = BRDF::Lambert(albedoColor, kd);

	//Return final color (keep in mind the kd is implemented in the diffuse and the ks was implemented into the fresnel)
	return (diffuse + specular);
}

Elite::RGBColor BRDF::Fresnel(float dotVH, const Elite::RGBColor& f0)
{
	Elite::RGBColor fresnel = f0 + (Elite::RGBColor(1, 1, 1) - f0) * powf(1 - dotVH, 5);
	return fresnel;
}

float BRDF::NormalDistribution(float dotNH, float roughnessSquared)
{
	float a = roughnessSquared * roughnessSquared;
	float denominator = powf((powf(dotNH, 2)) * (a - 1) + 1, 2);

	float normalDistribution = a / ((float)E_PI * denominator);
	return normalDistribution;
}

float BRDF::GeometryMasking(float dotNV, float k)
{
	float geometryMasking = dotNV / (dotNV * (1.f - k) + k);
	return geometryMasking;
}

float BRDF::GeometryShadowing(float dotNL, float k)
{
	float geometryShadowing = dotNL / (dotNL * (1.f - k) + k);
	return geometryShadowing;
}

Elite::RGBColor BRDF::CalculateSpecular(float D, const Elite::RGBColor& F, float G, float dotNL, float dotNV)
{
	Elite::RGBColor dfg{ F * D * G };
	return Elite::RGBColor{ dfg / 4.f * dotNV * dotNL };
}