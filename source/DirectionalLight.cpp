#pragma once
#include "pch.h"
#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const Elite::FVector3& direction, const Elite::RGBColor& color, float intensity)
	: Light(color, intensity)
	, m_Direction(direction)
{
	ConstructLightMatrix(color, direction, intensity);
}

Elite::RGBColor DirectionalLight::GetCalculatedIrradianceColor(const Elite::FVector3& normal, bool invertForRHS) const
{
	Elite::FVector3 dir{ m_Direction };
	if (invertForRHS)
		dir = Elite::FVector3(-m_Direction.x, m_Direction.y, -m_Direction.z);
	
	float observedArea = Elite::Clamp(Dot(-normal, dir), 0.f, 1.f);
	if (observedArea < 0)
		return Elite::RGBColor(0.f, 0.f, 0.f);

	return m_Color * m_LightIntensity * observedArea;
}

Elite::FVector3 DirectionalLight::GetDirection(const HitRecord& hitRecord, bool invertForRHS) const
{
	UNREFERENCED_PARAMETER(hitRecord);
	return (invertForRHS) ? Elite::FVector3(-m_Direction.x, m_Direction.y, -m_Direction.z) : m_Direction;
}
