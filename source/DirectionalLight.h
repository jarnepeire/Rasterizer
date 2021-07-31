#pragma once
#include "Light.h"

class DirectionalLight final : public Light
{
public:
	DirectionalLight(const Elite::FVector3& direction, const Elite::RGBColor& color, float intensity);
	DirectionalLight(const DirectionalLight& l) = delete;
	DirectionalLight(DirectionalLight&& l) = delete;
	DirectionalLight& operator=(const DirectionalLight& l) = delete;
	DirectionalLight& operator=(DirectionalLight&& l) = delete;
	~DirectionalLight() = default;

	Elite::RGBColor GetCalculatedIrradianceColor(const Elite::FVector3& normal, bool invertForRHS) const override;
	Elite::FVector3 GetDirection(const HitRecord& hitRecord, bool invertForRHS) const override;
	Elite::FPoint3 GetPosition() { return Elite::FPoint3(m_Direction); }

private:
	Elite::FVector3 m_Direction;
};

