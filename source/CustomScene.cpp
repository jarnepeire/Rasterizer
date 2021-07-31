#pragma once
//Main include
#include "pch.h"
#include "CustomScene.h"

//Other includes
#include "Camera.h"
#include "ObjParser.h"
#include "TriangleMesh.h"
#include "ERenderer.h"
#include "Material.h"
#include "LambertCookTorranceEffect.h"
#include "DirectionalLight.h"

CustomScene::CustomScene(SDL_Window* pWindow, const std::string& sceneTag)
	: Scene(pWindow, sceneTag)
	, m_TriangleMeshIdx()
{
}

void CustomScene::Initialize()
{
	//Materials
	InitializeMaterials();

	//Meshes
	InitializeTriangleMeshes();

	//Lights
	InitializeLights();

	//Camera
	Camera* pCamera = GetCamera();
	pCamera->Initialize(FPoint3(0, 15, 55), float(m_Width), float(m_Height), 45.f, false);
	pCamera->SetRotationSpeed(.3f);
	pCamera->SetTranslationSpeed(5.f);
}
void CustomScene::Update(float deltaT)
{
	UNREFERENCED_PARAMETER(deltaT);
	UpdateKeyInputs();
}

void CustomScene::Render()
{
}

void CustomScene::UpdateKeyInputs()
{
	const auto& input = GetInput();

	//------------------- SAMPLE STATES -------------------
	if (input.IsPressed(EKeyboardInput::ToggleSamplerState))
	{
		if (IsValidTriangleMesh(m_TriangleMeshIdx))
		{
			//Get one sample state (they're the same for both meshes)
			TriangleMesh* pTriangleMesh = GetTriangleMeshOnIndex(m_TriangleMeshIdx);
			auto samplerState = pTriangleMesh->GetSampleState();

			//Switch states
			if (samplerState == ESamplerState::Point)
			{
				GetTriangleMeshOnIndex(m_TriangleMeshIdx)->SetSampleState(ESamplerState::Linear);
				std::cout << "Sampler State - LINEAR\n";
			}
			else if (samplerState == ESamplerState::Linear)
			{
				GetTriangleMeshOnIndex(m_TriangleMeshIdx)->SetSampleState(ESamplerState::Anisotropic);
				std::cout << "Sampler State - ANISOTROPIC\n";
			}
			else if (samplerState == ESamplerState::Anisotropic)
			{
				GetTriangleMeshOnIndex(m_TriangleMeshIdx)->SetSampleState(ESamplerState::Point);
				std::cout << "Sampler State - POINT\n";
			}
		}
	}
	//------------------- RASTERIZER TYPE -------------------
	else if (input.IsPressed(EKeyboardInput::ToggleRasterizer))
	{
		//Get next render type in list
		ERendererType renderType = static_cast<ERendererType>(((int)GetRendererType() + 1) % (int)ERendererType::NUM_OF_OPTIONS);

		//Swap sras -> dx
		SetRendererType(renderType);

		//Swap lhs -> rhs for camera
		Camera* pCam = GetCamera();
		pCam->SetLeftHanded(!pCam->IsLH());
	}
	//------------------- CULLMODES -------------------
	else if (input.IsPressed(EKeyboardInput::ToggleCullMode))
	{
		if (IsValidTriangleMesh(m_TriangleMeshIdx))
		{
			//Our fire mesh needs to be render without culling, so we only change our vehicle
			TriangleMesh* pTriangleMesh = GetTriangleMeshOnIndex(m_TriangleMeshIdx);
			auto cullmode = pTriangleMesh->GetCullMode();

			//Switch cullmodes
			if (cullmode == ECullMode::NoCulling)
			{
				pTriangleMesh->SetCullMode(ECullMode::FrontCulling);
				std::cout << "Rastizer State - FRONT CULLING\n";
			}
			else if (cullmode == ECullMode::FrontCulling)
			{
				pTriangleMesh->SetCullMode(ECullMode::BackCulling);
				std::cout << "Rastizer State - BACK CULLING\n";
			}
			else if (cullmode == ECullMode::BackCulling)
			{
				pTriangleMesh->SetCullMode(ECullMode::NoCulling);
				std::cout << "Rastizer State - NO CULLING\n";
			}
		}
	}
	//------------------- DISPLAY INFO -------------------
	else if (input.IsPressed(EKeyboardInput::DisplayKeyBindInfo))
	{
		DisplayKeyBindInfo();
	}
	//------------------- MESH UPDATE -------------------
	else if (input.IsPressed(EKeyboardInput::StopRotating))
	{
		EnableTriangleUpdate(!IsUpdatingTriangles());
	}
}

void CustomScene::InitializeMaterials()
{
	//Get device
	ID3D11Device* pDevice = GetRenderer()->GetDevice();

	//Robot Material + Effect
	LambertCookTorranceEffect* pBotEffect = new LambertCookTorranceEffect(pDevice, L"./Resources/effects/LambertCookTorrance.fx");
	Material* pMat = new Material(0, Material::MaterialWorkflow::MetalRough, pBotEffect);
	pMat->SetDiffuseTexture("./Resources/daebot/diffuse.png", pDevice);
	pMat->SetNormalTexture("./Resources/daebot/normal.png", pDevice);
	pMat->SetRoughnessTexture("./Resources/daebot/roughness.png", pDevice);
	pMat->SetMetalnessTexture("./Resources/daebot/metallic.png", pDevice);
	AddMaterial(pMat);
}

void CustomScene::InitializeTriangleMeshes()
{
	//Robot mesh
	std::vector<Vertex_Input> vertices{};
	std::vector<uint32_t> indices{};
	ObjParser parser{ "./Resources/daebot/daebot.obj", true };
	parser.LoadIndexBuffer(indices);
	parser.LoadVertexBuffer(vertices);

	//Adding new triangle mesh with parsed information and material ID
	m_TriangleMeshIdx = AddTriangleMesh(0, vertices, indices, EPrimitiveTopology::TriangleList);
	auto pTriangleMesh = GetTriangleMeshOnIndex(m_TriangleMeshIdx);
	pTriangleMesh->SetBlendState(EBlendState::BlendNone);
	pTriangleMesh->SetCullMode(ECullMode::BackCulling);
	pTriangleMesh->SetSampleState(ESamplerState::Point);
}

void CustomScene::InitializeLights()
{
	//General directional light
	AddLight(new DirectionalLight(FVector3(0.577f, -0.577f, 0.577f), Elite::RGBColor(1.f, 1.f, 1.f), 4.0f));
	AddLight(new DirectionalLight(FVector3(-0.577f, -0.577f, -0.577f), Elite::RGBColor(1.f, 1.f, 1.f), 4.0f));
}

void CustomScene::DisplayKeyBindInfo()
{
	std::cout <<
		"+--------------------------------------------+\n" <<
		"|     Exam Project - Jarne Peire(2DAE02)     |\n" <<
		"+--------------------------------------------+\n" <<
		m_FirstSpaces << m_SceneTag << m_LastSpaces << "\n" <<
		"|                 Key Binds :                |\n" <<
		"+--------------------------------------+-----+\n" <<
		"| Display KeyBind Information          |  I  |\n" <<
		"+--------------------------------------+-----+\n" <<
		"| Toggle Rasterizers (SRAS-DX)         |  R  |\n" <<
		"+--------------------------------------+-----+\n" <<
		"| Toggle Texture Sampling States       |  F  |\n" <<
		"+--------------------------------------+-----+\n" <<
		"| Toggle Transparancy (On/Off)         |  T  |\n" <<
		"+--------------------------------------+-----+\n" <<
		"| Toggle CullModes (SRAS and DX)       |  C  |\n" <<
		"+--------------------------------------+-----+\n" <<
		"| Toggle Mesh Rotation                 |SPACE|\n" <<
		"+--------------------------------------+-----+\n" <<
		"| Previous Scene                       |  F1 |\n" <<
		"+--------------------------------------+-----+\n" <<
		"| Next Scene                           |  F2 |\n" <<
		"+--------------------------------------+-----+\n";
}