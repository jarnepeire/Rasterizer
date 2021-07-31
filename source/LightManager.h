#pragma once
#include <vector>

class Light;
class LightManager final
{
public:
	LightManager();
	LightManager(const LightManager& lm) = delete;
	LightManager(LightManager&& lm) = delete;
	LightManager& operator=(const LightManager& lm) = delete;
	LightManager& operator=(LightManager&& lm) = delete;
	~LightManager();

	/* Adds a light to the manager and returns the current index this light is held in,
		In case of invalid light or nullptr -> returns -1 */
	size_t AddLight(Light* pLight);

	/* Return reference to vector of all lights this manager holds */
	const std::vector<Light*>& GetLights() const { return m_pLights; }

	/* Return reference to vector of all light matrices this manager holds */
	const std::vector<Elite::FMatrix4>& GetLightMatrices() const { return m_LightMatrices; }

	/* Amount of maximum lights, we need to specify this amount due to limitation of the hlsl shader 
		If you happen to change this value, make sure you also change it in the shaders */
	static const int MAX_LIGHTS = 5;
private:
	std::vector<Light*> m_pLights;
	std::vector<Elite::FMatrix4> m_LightMatrices;
};