#pragma once
#include <vector>
#include <string>

class Scene;
class SceneManager final
{
public:
	SceneManager();
	SceneManager(const SceneManager& s) = delete;
	SceneManager(SceneManager&& s) = delete;
	SceneManager& operator=(const SceneManager& s) = delete;
	SceneManager& operator=(SceneManager&& s) = delete;
	~SceneManager();

	/* Updates the currently active scene */
	void Update(float deltaT);

	/* Renders the currently active scene */
	void Render();

	/* Sets the scene with the given index to the current scene that will be updated and rendered
		Sets the active scene to the first one on invalid index */
	void SetActiveScene(const int index);

	/* Sets the scene with the given scene tag to the current scene that will be updated and rendered
		Sets the active scene to the first one on invalid scene tags */
	void SetActiveScene(const std::string& sceneTag);

	/* Adds a new scene to the manager and call its root- and initialize function */
	void AddScene(Scene* pScene);

	/* Returns pointer to the scene with the given index
		Returns nullptr on invalid index */
	Scene* GetScene(const int index) const;

	/* Returns pointer to the scene with the given scene tag
	Returns nullptr on invalid scene tag */
	Scene* GetScene(const std::string& sceneTag) const;

	/* Returns pointer to the scene that's currently active */
	Scene* GetCurrentScene() const;

	/* Returns the next free index value*/
	int GetNextFreeIndex() const { return int(m_pScenes.size()); }

private:
	int m_CurrentSceneIndex = 0;
	std::vector<Scene*> m_pScenes;
};