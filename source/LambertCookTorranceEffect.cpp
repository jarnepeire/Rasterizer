#pragma once
#include "pch.h"
#include "LambertCookTorranceEffect.h"
#include "Material.h"
#include "LightManager.h"
#include "Texture.h"

using namespace Elite;
LambertCookTorranceEffect::LambertCookTorranceEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
    : Effect(pDevice, assetFile)
    , m_pUseDiffuseMapVariable(nullptr)
    , m_pDiffuseMapVariable(nullptr)
    , m_pUseNormalMapVariable(nullptr)
    , m_pNormalMapVariable(nullptr)
    , m_pUseMetalnessMapVariable(nullptr)
    , m_pMetalnessMapVariable(nullptr)
    , m_pUseRoughnessMapVariable(nullptr)
    , m_pRoughnessMapVariable(nullptr)
    , m_pLights(nullptr)
{
    //Diffuse Map
    m_pUseDiffuseMapVariable = m_pEffect->GetVariableByName("gUseDiffuseMap")->AsScalar();
    if (!m_pUseDiffuseMapVariable->IsValid())
        std::cout << "m_pUseDiffuseMapVariable not valid\n";

    m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
    if (!m_pDiffuseMapVariable->IsValid())
        std::cout << "m_pDiffuseMapVariable not valid\n";

    //Normal Map
    m_pUseNormalMapVariable = m_pEffect->GetVariableByName("gUseNormalMap")->AsScalar();
    if (!m_pUseNormalMapVariable->IsValid())
        std::cout << "m_pUseNormalMapVariable not valid\n";

    m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
    if (!m_pNormalMapVariable->IsValid())
        std::cout << "m_pNormalMapVariable not valid\n";

    //Metalness
    m_pUseMetalnessMapVariable = m_pEffect->GetVariableByName("gUseMetalMap")->AsScalar();
    if (!m_pUseMetalnessMapVariable->IsValid())
        std::cout << "m_pUseMetalnessMapVariable not valid\n";

    m_pMetalnessMapVariable = m_pEffect->GetVariableByName("gMetalnessMap")->AsShaderResource();
    if (!m_pMetalnessMapVariable->IsValid())
        std::cout << "m_pMetalnessMapVariable not valid\n";

    //Roughness
    m_pUseRoughnessMapVariable = m_pEffect->GetVariableByName("gUseRoughnessMap")->AsScalar();
    if (!m_pUseRoughnessMapVariable->IsValid())
        std::cout << "m_pUseRoughnessMapVariable not valid\n";

    m_pRoughnessMapVariable = m_pEffect->GetVariableByName("gRoughnessMap")->AsShaderResource();
    if (!m_pRoughnessMapVariable->IsValid())
        std::cout << "m_pRoughnessMapVariable not valid\n";

    //Lights
    m_pLights = m_pEffect->GetVariableByName("gLights")->AsMatrix();
    if (!m_pLights->IsValid())
        std::cout << "m_pLights not valid\n";
}

LambertCookTorranceEffect::~LambertCookTorranceEffect()
{
    m_pLights->Release();
    m_pRoughnessMapVariable->Release();
    m_pUseRoughnessMapVariable->Release();
    m_pMetalnessMapVariable->Release();
    m_pUseMetalnessMapVariable->Release();
    m_pNormalMapVariable->Release();
    m_pUseNormalMapVariable->Release();
    m_pDiffuseMapVariable->Release();
    m_pUseDiffuseMapVariable->Release();
}

void LambertCookTorranceEffect::UpdateEffectVariables(const FMatrix4& world, const FMatrix4& onb, const FMatrix4& projMatrix, Material* pMaterial, const std::vector<FMatrix4>& lightMatrices)
{
    Effect::UpdateEffectVariables(world, onb, projMatrix, pMaterial, lightMatrices);

    //Diffuse
    bool useDiffuse = pMaterial->UseDiffuseMap();
    m_pUseDiffuseMapVariable->SetBool(useDiffuse);
    if (useDiffuse) m_pDiffuseMapVariable->SetResource(pMaterial->GetDiffuseTexture()->GetTextureResourceView());

    //Normal
    bool useNormal = pMaterial->UseNormalMap();
    m_pUseNormalMapVariable->SetBool(useNormal);
    if (useNormal) m_pNormalMapVariable->SetResource(pMaterial->GetNormalTexture()->GetTextureResourceView());

    //Metallic
    bool useMetal = pMaterial->UseMetalnessMap();
    m_pUseMetalnessMapVariable->SetBool(useMetal);
    if (useMetal) m_pMetalnessMapVariable->SetResource(pMaterial->GetMetalnessTexture()->GetTextureResourceView());

    //Roughness
    bool useRoughness = pMaterial->UseRoughnessMap();
    m_pUseRoughnessMapVariable->SetBool(useRoughness);
    if (useRoughness) m_pRoughnessMapVariable->SetResource(pMaterial->GetRoughnessTexture()->GetTextureResourceView());

    //Lights
    //To fix issue of light matrices not being aligned in memory
    const int maxLights = LightManager::MAX_LIGHTS;
    Elite::FMatrix4 matrices[maxLights]{};
    for (int i = 0; i < maxLights; ++i)
    {
        if (i < lightMatrices.size())
            matrices[i] = lightMatrices[i];
    }
    m_pLights->SetMatrixArray((float*)&matrices, 0, maxLights);
}