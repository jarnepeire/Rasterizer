#pragma once
#include "EMath.h"
#include "ERGBColor.h"

struct HitRecord;
class Light
{
public:
	Light(const Light& l) = delete;
	Light(Light&& l) = delete;
	Light& operator=(const Light& l) = delete;
	Light& operator=(Light&& l) = delete;
	virtual ~Light() = default;

	/* Calculate and return the irradiance at a certain point (hitPos from HitRecord) */
	virtual Elite::RGBColor GetCalculatedIrradianceColor(const Elite::FVector3& normal, bool invertForRHS) const = 0;

	/* Return normalized direction of light (differs for type of light) */
	virtual Elite::FVector3 GetDirection(const HitRecord& hitRecord, bool invertForRHS) const = 0;

	/* Return light color */
	const Elite::RGBColor& GetColor() const { return m_Color; }

	/* Return light intensity */
	float GetIntensity() const { return m_LightIntensity; }

	/* Check if a light is active in the scene */
	bool GetIsActive() const { return m_IsActive; }

	/* Set a light active or inactive in the scene */
	void SetActive(bool isActive) { m_IsActive = isActive; }

	/* Returns reference to light matrix */
	const Elite::FMatrix4& GetLightMatrix() const { return m_LightMatrix; }

	/* Constructs light matrix based on 3 variables 
		@Column1: Light Color
		@Column2: Light Direction
		@Column3: Light Intensity (in all components)*/
	void ConstructLightMatrix(const Elite::RGBColor& lightColor, const Elite::FVector3& lightDir, float intensity);

protected:
	Light(const Elite::RGBColor& color, float intensity);

	Elite::FMatrix4 m_LightMatrix;
	Elite::RGBColor m_Color;
	float m_LightIntensity;
	bool m_IsActive;
};