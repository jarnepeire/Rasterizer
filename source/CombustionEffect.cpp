#pragma once
#include "pch.h"
#include "CombustionEffect.h"
#include "Material.h"
#include "Texture.h"

using namespace Elite;
CombustionEffect::CombustionEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
	: Effect(pDevice, assetFile)
	, m_pDiffuseMapVariable(nullptr)
{
	//Diffuse Map
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
		std::cout << "m_pDiffuseMapVariable not valid\n";
}

CombustionEffect::~CombustionEffect()
{
	m_pDiffuseMapVariable->Release();
}

void CombustionEffect::UpdateEffectVariables(const FMatrix4& world, const FMatrix4& onb, const FMatrix4& projMatrix, Material* pMaterial, const std::vector<FMatrix4>& lightMatrices)
{
	Effect::UpdateEffectVariables(world, onb, projMatrix, pMaterial, lightMatrices);

	m_pDiffuseMapVariable->SetResource(pMaterial->GetDiffuseTexture()->GetTextureResourceView());
}