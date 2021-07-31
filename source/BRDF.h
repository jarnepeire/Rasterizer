#pragma once
#include "EMath.h"
#include "ERGBColor.h"

class BRDF final
{
public:

	/* Return calculated pixel color using the lambert diffuse brdf */
	static Elite::RGBColor Lambert(const Elite::RGBColor& diffColor, float diffReflectance);

	/* Return calculated pixel color using the lambert diffuse brdf */
	static Elite::RGBColor Lambert(const Elite::RGBColor& albedoColor, const Elite::RGBColor& kd);

	/* Return calculated pixel color using the specular phong brdf */
	static Elite::RGBColor Phong(float specReflectance, float phongExp, const Elite::FVector3& lightDir, const Elite::FVector3& invViewDir, const Elite::FVector3& hitNormal);

	/* Return calculated pixel color using the lambert cook torrance brdf */
	static Elite::RGBColor LambertCookTorrance(const Elite::RGBColor& albedoColor, int metalness, float roughness, const Elite::FVector3& lightDir, const Elite::FVector3& invViewDir, const Elite::FVector3& hitNormal);

private:
	static Elite::RGBColor Fresnel(float dotNH, const Elite::RGBColor& f0);
	static float NormalDistribution(float dotNH, float roughnessSquared);
	static float GeometryMasking(float dotNV, float k);
	static float GeometryShadowing(float dotNL, float k);
	static Elite::RGBColor CalculateSpecular(float D, const Elite::RGBColor& F, float G, float dotNL, float dotNV);
};