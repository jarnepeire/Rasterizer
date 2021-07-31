#pragma once
#include "Structs.h"
#include <string>

#include "InputManager.h"
#include "MaterialManager.h"
#include "LightManager.h"
#include "TriangleMesh.h"

namespace Elite
{
	class Renderer;
}

enum class ERendererType : unsigned int;
struct KeyBindInfo;
struct SDL_Window;
class SceneManager;
class Camera;
class Texture;
class Material;
class Light;

class Scene
{
public:
	Scene(SDL_Window* pWindow, const std::string& sceneTag);
	Scene(const Scene& s) = delete;
	Scene(Scene&& s) = delete;
	Scene& operator=(const Scene& s) = delete;
	Scene& operator=(Scene&& s) = delete;
	virtual ~Scene();

	/* Initialize the scene and its objects */
	virtual void Initialize() = 0;

	/* Update the scene (track input, updative primites..) */
	virtual void Update(float deltaT) = 0;

	/* Render the scene and its objects */
	virtual void Render() = 0;

	/* Enables/Disables the updating of the triangle (rotation) */
	void EnableTriangleUpdate(bool update) { m_UpdateTriangles = update; }
	
	/* Returns whether triangles are being updated */
	bool IsUpdatingTriangles() const { return m_UpdateTriangles; }

	/* Returns pointer to the renderer of the scene */
	Elite::Renderer* GetRenderer() const { return m_pRenderer; }

	/* Returns true if a triangle mesh exists on the given index */
	bool IsValidTriangleMesh(size_t idx) const { return (idx < m_pTriangleMeshes.size()); }

	/* Returns pointer to triangle mesh on given index
		Check if valid triangle mesh with index first! */
	TriangleMesh* GetTriangleMeshOnIndex(size_t idx) const { return m_pTriangleMeshes[idx]; }

	/* Return pointer to the camera of this scene */
	Camera* GetCamera() const { return m_pCamera; }

	/* Returns reference to input manager that tracks mouse- and keyboard input */
	const InputManager& GetInput() const { return m_Input; }

	/* Return reference to the scene's tag */
	const std::string& GetSceneTag() const { return m_SceneTag; }

	/* Return the scene's index */
	int GetSceneIndex() const { return m_SceneIndex; }

	/* Returns reference to the enum describing the rendering type */
	const ERendererType& GetRendererType() const { return m_RendererType; }

	/* Sets renderer to different renderer type 
		Options being SRAS or DX */
	void SetRendererType(ERendererType type);

protected:
	/* Unique scene information */
	int m_SceneIndex;
	const std::string m_SceneTag;

	/* Window info */
	int m_Height;
	int m_Width;

	KeyBindInfo m_KeyBindInfo;
	std::string m_FirstSpaces;
	std::string m_LastSpaces;
	virtual void DisplayKeyBindInfo() = 0;

	/* Adds a new triangle mesh to the current scene */
	size_t AddTriangleMesh(unsigned int id, std::vector<Vertex_Input>& vertices, std::vector<uint32_t>& indices, const EPrimitiveTopology& top);

	/* Adds a new material to the current scene */
	void AddMaterial(Material* pMaterial);

	/* Adds a new light to the current scene
		Keep in mind, there's only a maximum number of lights allowed due to limitations of hlsl shader*/
	void AddLight(Light* pLight);

private:
	/* For access to root functionality (RootInitialize(), RootUpdate()..) */
	friend class SceneManager;

	ERendererType m_RendererType;
	Elite::Renderer* m_pRenderer;
	Camera* m_pCamera;
	std::vector<TriangleMesh*> m_pTriangleMeshes;
	bool m_UpdateTriangles;
	InputManager m_Input;
	MaterialManager m_MaterialManager;
	LightManager m_LightManager;

	unsigned int m_TotalWhiteSpaces;

	/* Private functions */
	void RootInitialize();
	void PostInitialize();
	void RootUpdate(float deltaT);
	void RootRender();
	void InitializeSpacingForKeybindInfo();
	void SetSceneIndex(int sceneIdx);
};
