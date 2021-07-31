//SDL + Scene
#pragma once
#include "pch.h"
#include "SDL.h"
#include "Scene.h"

//Other
#include "ERenderer.h"
#include "Material.h"
#include "Camera.h"
#include <iostream>

Scene::Scene(SDL_Window* pWindow, const std::string& sceneTag)
	: m_SceneIndex(-1)
	, m_SceneTag(sceneTag)
	, m_Height()
	, m_Width()
	, m_KeyBindInfo()
	, m_FirstSpaces()
	, m_LastSpaces()
	, m_RendererType(ERendererType::SRAS)
	, m_pRenderer(new Elite::Renderer(pWindow))
	, m_pCamera(new Camera())
	, m_pTriangleMeshes()
	, m_UpdateTriangles(true)
	, m_Input()
	, m_MaterialManager()
	, m_LightManager()
	, m_TotalWhiteSpaces(44)
{
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
}

Scene::~Scene()
{
	delete m_pCamera;
	for (size_t i = 0; i < m_pTriangleMeshes.size(); ++i)
	{
		delete m_pTriangleMeshes[i];
		m_pTriangleMeshes[i] = nullptr;
	}
	delete m_pRenderer;
}

size_t Scene::AddTriangleMesh(unsigned int id, std::vector<Vertex_Input>& vertices, std::vector<uint32_t>& indices, const EPrimitiveTopology& top)
{
	size_t idx = m_pTriangleMeshes.size();
	m_pTriangleMeshes.push_back(new TriangleMesh(id, vertices, indices, top));
	return idx;
}

void Scene::AddMaterial(Material* pMaterial)
{
	m_MaterialManager.AddMaterial(pMaterial);
}

void Scene::AddLight(Light* pLight)
{
	m_LightManager.AddLight(pLight);
}

void Scene::SetRendererType(ERendererType type)
{
	m_RendererType = type;
}

void Scene::RootInitialize()
{
	//Init variables by default
	m_pCamera->Initialize(Elite::FPoint3(0.f, 5.f, 15.f), float(m_Width), float(m_Height), 45.f, false);
	InitializeSpacingForKeybindInfo();
}

void Scene::PostInitialize()
{
	//Post initialize needed to certify a valid render device and valid initialized material effectst to set the triangle meshes
	for (TriangleMesh* pTriangleMesh : m_pTriangleMeshes)
	{
		Material* pMat = m_MaterialManager.GetMaterialByID(pTriangleMesh->GetMaterialID());
		if (!pMat)
		{
			std::cout << "Couldn't match Triangle Mesh with ID: \"" << pTriangleMesh->GetMaterialID() << "\" to material\n";
			continue;
		}
		pTriangleMesh->Initialize(m_pRenderer->GetDevice(), pMat->GetEffect());
	}
}

void Scene::RootUpdate(float deltaT)
{
	m_Input.ProcessInput();
	m_pCamera->Update(m_Input, deltaT);
	if (!m_UpdateTriangles)
		return;

	//Update all triangle meshes
	for (TriangleMesh* pTriangleMesh : m_pTriangleMeshes)
	{
		pTriangleMesh->Update(deltaT);
	}
}

void Scene::SetSceneIndex(int sceneIdx)
{
	m_SceneIndex = sceneIdx;
}

void Scene::RootRender()
{
	//Render all triangle meshes
	m_pRenderer->Render(m_pTriangleMeshes, m_MaterialManager, m_LightManager, m_pCamera, m_KeyBindInfo, m_RendererType);
}

void Scene::InitializeSpacingForKeybindInfo()
{
	unsigned int nameSize = (unsigned int)m_SceneTag.size();
	unsigned int leftOverSpaceSize = m_TotalWhiteSpaces - nameSize;
	bool isOddLeftOverSpace = (leftOverSpaceSize % 2 != 0);
	leftOverSpaceSize /= 2;

	m_FirstSpaces.insert(0, (leftOverSpaceSize + static_cast<unsigned int>(isOddLeftOverSpace)), ' ');
	m_FirstSpaces.push_back('|');
	std::reverse(m_FirstSpaces.begin(), m_FirstSpaces.end());

	m_LastSpaces.insert(0, leftOverSpaceSize, ' ');
	m_LastSpaces.push_back('|');
}