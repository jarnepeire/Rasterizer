#pragma once
#include "InputManager.h"
#include "Scene.h"

class MainScene final : public Scene
{
public:

	MainScene(SDL_Window* pWindow, const std::string& sceneTag);
	virtual ~MainScene() = default;

	void Initialize() override;
	void Update(float deltaT) override;
	void Render() override;

	void DisplayKeyBindInfo() override;
private:
	size_t m_TriangleMeshIdx;
	size_t m_FireMeshIdx;

	/* Private functions */
	void UpdateKeyInputs();
	void InitializeMaterials();
	void InitializeTriangleMeshes();
	void InitializeLights();
};

