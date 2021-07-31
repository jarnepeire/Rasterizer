#pragma once
//Main include
#include "pch.h"
#include "MainScene.h"

//Other includes
#include "Camera.h"
#include "ObjParser.h"
#include "TriangleMesh.h"
#include "ERenderer.h"
#include "Material.h"
#include "LambertPhongEffect.h"
#include "CombustionEffect.h"
#include "DirectionalLight.h"

MainScene::MainScene(SDL_Window* pWindow, const std::string& sceneTag)
	: Scene(pWindow, sceneTag)
	, m_TriangleMeshIdx()
	, m_FireMeshIdx()
{
}

void MainScene::Initialize()
{
	//Materials
	InitializeMaterials();

	//Meshes
	InitializeTriangleMeshes();

	//Lights
	InitializeLights();

	//Camera
	Camera* pCamera = GetCamera();
	pCamera->Initialize(FPoint3(0, 0, 65), float(m_Width), float(m_Height), 45.f, false);
	pCamera->SetRotationSpeed(.3f);
	pCamera->SetTranslationSpeed(5.f);
}
void MainScene::Update(float deltaT)
{
	UNREFERENCED_PARAMETER(deltaT);
	UpdateKeyInputs();
}

void MainScene::Render()
{
}

void MainScene::UpdateKeyInputs()
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
				GetTriangleMeshOnIndex(m_FireMeshIdx)->SetSampleState(ESamplerState::Linear);
				std::cout << "Sampler State - LINEAR\n";
			}
			else if (samplerState == ESamplerState::Linear)
			{
				GetTriangleMeshOnIndex(m_TriangleMeshIdx)->SetSampleState(ESamplerState::Anisotropic);
				GetTriangleMeshOnIndex(m_FireMeshIdx)->SetSampleState(ESamplerState::Anisotropic);
				std::cout << "Sampler State - ANISOTROPIC\n";
			}
			else if (samplerState == ESamplerState::Anisotropic)
			{
				GetTriangleMeshOnIndex(m_TriangleMeshIdx)->SetSampleState(ESamplerState::Point);
				GetTriangleMeshOnIndex(m_FireMeshIdx)->SetSampleState(ESamplerState::Point);
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

		//Turn off fire mesh rendering for software rasterizer
		if (IsValidTriangleMesh(m_FireMeshIdx))
		{
			auto pTriangleMesh = GetTriangleMeshOnIndex(m_FireMeshIdx);
			pTriangleMesh->SetValid((renderType == ERendererType::DirectX));
		}
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
	//------------------- BLEND STATES -------------------
	else if (input.IsPressed(EKeyboardInput::ToggleTransparency))
	{
		//Our fire mesh is the only thing that has transparency -> so we'll only switch that
		TriangleMesh* pFireMesh = GetTriangleMeshOnIndex(m_FireMeshIdx);
		auto blendstate = pFireMesh->GetBlendState();
		if (blendstate == EBlendState::BlendNone)
		{
			pFireMesh->SetBlendState(EBlendState::BlendAdd);
			std::cout << "Blend State - BLEND ADD\n";
		}
		else if (blendstate == EBlendState::BlendAdd)
		{
			pFireMesh->SetBlendState(EBlendState::BlendNone);
			std::cout << "Blend State - BLEND NONE\n";
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

void MainScene::InitializeMaterials()
{
	//Get device
	ID3D11Device* pDevice = GetRenderer()->GetDevice();

	//Vehicle Material + Effect
	LambertPhongEffect* pVehicleEffect = new LambertPhongEffect(pDevice, L"./Resources/effects/LambertPhong.fx");
	Material* pMatVehicle = new Material(0, Material::MaterialWorkflow::SpecGloss, pVehicleEffect);
	pMatVehicle->SetDiffuseTexture("./Resources/vehicle/vehicle_diffuse.png", pDevice);
	pMatVehicle->SetNormalTexture("./Resources/vehicle/vehicle_normal.png", pDevice);
	pMatVehicle->SetShininess(25.f);
	pMatVehicle->SetSpecularTexture("./Resources/vehicle/vehicle_specular.png", pDevice);
	pMatVehicle->SetGlossinessTexture("./Resources/vehicle/vehicle_gloss.png", pDevice);
	AddMaterial(pMatVehicle);

	//Combustion Fire Material + Effect
	CombustionEffect* pCombustionEffect = new CombustionEffect(pDevice, L"./Resources/effects/Combustion.fx");
	Material* pMatFire = new Material(1, Material::MaterialWorkflow::SpecGloss, pCombustionEffect);
	pMatFire->SetDiffuseTexture("./Resources/combustion/fireFX_diffuse.png", pDevice);
	AddMaterial(pMatFire);
}

void MainScene::InitializeTriangleMeshes()
{
	//Vehicle Mesh Parsing
	std::vector<Vertex_Input> vertices{};
	std::vector<uint32_t> indices{};
	
	ObjParser parser{ "./Resources/vehicle/vehicle.obj", true };
	parser.LoadIndexBuffer(indices);
	parser.LoadVertexBuffer(vertices);

	//Adding new triangle mesh with parsed information and material ID
	m_TriangleMeshIdx = AddTriangleMesh(0, vertices, indices, EPrimitiveTopology::TriangleList);

	
	//Combustion mesh parsing
	std::vector<Vertex_Input> verticesFire{};
	std::vector<uint32_t> indicesFire{};
	ObjParser fire{ "./Resources/combustion/fireFX.obj" , true };
	fire.LoadVertexBuffer(verticesFire);
	fire.LoadIndexBuffer(indicesFire);

	//Adding new triangle mesh with parsed information and material ID
	m_FireMeshIdx = AddTriangleMesh(1, verticesFire, indicesFire, EPrimitiveTopology::TriangleList);

	//Changing some start settings for our fire mesh
	auto pFireMesh = GetTriangleMeshOnIndex(m_FireMeshIdx);
	pFireMesh->SetValid(false);
	pFireMesh->SetCullMode(ECullMode::NoCulling);
	pFireMesh->SetBlendState(EBlendState::BlendAdd);
	
}

void MainScene::InitializeLights()
{
	//General directional light
	AddLight(new DirectionalLight(FVector3(0.577f, -0.577f, 0.577f), Elite::RGBColor(1.f, 1.f, 1.f), 7.0f));
}

void MainScene::DisplayKeyBindInfo()
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