#pragma once
#include "pch.h"
#include "Light.h"

Light::Light(const Elite::RGBColor& color, float intensity)
	: m_LightMatrix(Elite::FMatrix3::Identity())
	, m_Color(color)
	, m_LightIntensity(intensity)
	, m_IsActive(true)
{
}

void Light::ConstructLightMatrix(const Elite::RGBColor& lightColor, const Elite::FVector3& lightDir, float intensity)
{
	Elite::FMatrix3 lightMatrix3x3;
	lightMatrix3x3[0][0] = lightColor.r;
	lightMatrix3x3[1][0] = lightColor.g;
	lightMatrix3x3[2][0] = lightColor.b;

	lightMatrix3x3[0][1] = lightDir.x;
	lightMatrix3x3[1][1] = lightDir.y;
	lightMatrix3x3[2][1] = lightDir.z;

	lightMatrix3x3[0][2] = intensity;
	lightMatrix3x3[1][2] = intensity;
	lightMatrix3x3[2][2] = intensity;

	lightMatrix3x3 = Elite::Transpose(lightMatrix3x3);
	m_LightMatrix = Elite::FMatrix4(lightMatrix3x3);
}