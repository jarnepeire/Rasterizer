#pragma once
#include "InputManager.h"
#include "Scene.h"

class CustomScene final : public Scene
{
public:
	CustomScene(SDL_Window* pWindow, const std::string& sceneTag);
	virtual ~CustomScene() = default;

	void Initialize() override;
	void Update(float deltaT) override;
	void Render() override;

	void DisplayKeyBindInfo() override;
private:
	size_t m_TriangleMeshIdx;

	/* Private functions */
	void UpdateKeyInputs();
	void InitializeMaterials();
	void InitializeTriangleMeshes();
	void InitializeLights();
};

