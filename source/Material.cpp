#pragma once
#include "pch.h"
#include "Texture.h"
#include "Material.h"
#include "Effect.h"

Material::Material(unsigned int materialID, MaterialWorkflow workflow, Effect* effect)
	: m_MaterialID(materialID)
	, m_MatWorkflow(workflow)
	, m_pEffect(effect)
	, m_UseDiffuseMap(false)
	, m_DiffuseReflectance(1.f)
	, m_pDiffuseTexture(nullptr)
	, m_DiffuseColor(Elite::RGBColor(1, 1, 1))
	, m_UseNormalMap(false)
	, m_pNormalTexture(nullptr)
	, m_UseSpecularMap(false)
	, m_Shininess(15.f)
	, m_SpecularReflectance(1.f)
	, m_pSpecularTexture(nullptr)
	, m_SpecularColor(Elite::RGBColor(1, 1, 1))
	, m_UseGlossinessMap(false)
	, m_pGlossinessTexture(nullptr)
	, m_UseMetalnessMap(false)
	, m_pMetalnessTexture(nullptr)
	, m_UseRoughnessMap(false)
	, m_pRoughnessTexture(nullptr)
{
}

Material::~Material()
{
	delete m_pDiffuseTexture;
	delete m_pNormalTexture;
	delete m_pSpecularTexture;
	delete m_pGlossinessTexture;
	delete m_pMetalnessTexture;
	delete m_pRoughnessTexture;
	delete m_pEffect;
}

void Material::SetDiffuseTexture(const char* filepath, ID3D11Device* pDevice)
{
	m_pDiffuseTexture = new Texture(filepath, pDevice); 
	m_UseDiffuseMap = true; 
}

void Material::SetNormalTexture(const char* filepath, ID3D11Device* pDevice)
{
	m_pNormalTexture = new Texture(filepath, pDevice); 
	m_UseNormalMap = true;
}

void Material::SetSpecularTexture(const char* filepath, ID3D11Device* pDevice)
{
	m_pSpecularTexture = new Texture(filepath, pDevice); 
	m_UseSpecularMap = true;
}

void Material::SetGlossinessTexture(const char* filepath, ID3D11Device* pDevice)
{
	m_pGlossinessTexture = new Texture(filepath, pDevice); 
	m_UseGlossinessMap = true;
}

void Material::SetMetalnessTexture(const char* filepath, ID3D11Device* pDevice)
{
	m_pMetalnessTexture = new Texture(filepath, pDevice); 
	m_UseMetalnessMap = true;
}

void Material::SetRoughnessTexture(const char* filepath, ID3D11Device* pDevice)
{
	m_pRoughnessTexture = new Texture(filepath, pDevice); 
	m_UseRoughnessMap = true;
}
