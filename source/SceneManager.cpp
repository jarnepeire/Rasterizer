#pragma once
#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include <algorithm>

SceneManager::SceneManager()
	: m_CurrentSceneIndex(0)
	, m_pScenes()
{
}

SceneManager::~SceneManager()
{
	for (size_t i{}; i < m_pScenes.size(); ++i)
	{
		delete m_pScenes[i];
		m_pScenes[i] = nullptr;
	}
	m_pScenes.clear();
}

void SceneManager::Update(float deltaT)
{
	//Input check
	auto& input = m_pScenes[m_CurrentSceneIndex]->GetInput();

	if (input.IsPressed(EKeyboardInput::PreviousScene))
		SetActiveScene(--m_CurrentSceneIndex);
	else if (input.IsPressed(EKeyboardInput::NextScene))
		SetActiveScene(++m_CurrentSceneIndex);

	//Further update
	m_pScenes[m_CurrentSceneIndex]->RootUpdate(deltaT);
	m_pScenes[m_CurrentSceneIndex]->Update(deltaT);
}

void SceneManager::Render()
{
	m_pScenes[m_CurrentSceneIndex]->RootRender();
	m_pScenes[m_CurrentSceneIndex]->Render();
}

void SceneManager::SetActiveScene(const int index)
{
	if (size_t(index) < m_pScenes.size() && m_pScenes[index])
		m_CurrentSceneIndex = index;
	else if (index == -1)
		m_CurrentSceneIndex = (int)m_pScenes.size() - 1;
	else
		m_CurrentSceneIndex = 0;

	//Display to text what scene is being used now with its according inputs
	m_pScenes[m_CurrentSceneIndex]->DisplayKeyBindInfo();
}

void SceneManager::SetActiveScene(const std::string& sceneTag)
{
	std::vector<Scene*>::iterator it = std::find_if(m_pScenes.begin(), m_pScenes.end(), [&sceneTag](Scene* pScene) { return (pScene->GetSceneTag() == sceneTag); });
	if (it == m_pScenes.end())
		return;

	Scene* pScene = *it;
	m_CurrentSceneIndex = pScene->GetSceneIndex();

	//Display to text what scene is being used now with its according inputs
	pScene->DisplayKeyBindInfo();
}

void SceneManager::AddScene(Scene* pScene)
{
	if (pScene)
	{
		pScene->SetSceneIndex(GetNextFreeIndex());
		m_pScenes.push_back(pScene);

		pScene->RootInitialize();
		pScene->Initialize();
		pScene->PostInitialize();
	}
}

Scene* SceneManager::GetScene(const int index) const
{
	if (size_t(index) < m_pScenes.size() && m_pScenes[index])
		return m_pScenes[index];
	else
		return nullptr;
}

Scene* SceneManager::GetScene(const std::string& sceneTag) const
{
	std::vector<Scene*>::const_iterator it = std::find_if(m_pScenes.begin(), m_pScenes.end(), [&sceneTag](Scene* pScene) { return (pScene->GetSceneTag() == sceneTag); });
	if (std::find_if(m_pScenes.begin(), m_pScenes.end(), [&sceneTag](Scene* pScene) { return (pScene->GetSceneTag() == sceneTag); }) == m_pScenes.end())
		return nullptr;
	else
		return *it;
}

Scene* SceneManager::GetCurrentScene() const
{
	return m_pScenes[m_CurrentSceneIndex];
}