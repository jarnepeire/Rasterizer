#pragma once
#include "pch.h"
#include "LambertPhongEffect.h"
#include "Material.h"
#include "Texture.h"
#include "LightManager.h"

using namespace Elite;
LambertPhongEffect::LambertPhongEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
    : Effect(pDevice, assetFile)
    , m_pUseDiffuseMapVariable(nullptr)
    , m_pDiffuseMapVariable(nullptr)
    , m_pUseNormalMapVariable(nullptr)
    , m_pNormalMapVariable(nullptr)
    , m_pShininessVariable(nullptr)
    , m_pUseSpecularMapVariable(nullptr)
    , m_pSpecularMapVariable(nullptr)
    , m_pUseGlossinessMapVariable(nullptr)
    , m_pGlossinessMapVariable(nullptr)
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

    //Specular Map
    m_pUseSpecularMapVariable = m_pEffect->GetVariableByName("gUseSpecularMap")->AsScalar();
    if (!m_pUseSpecularMapVariable->IsValid())
        std::cout << "m_pUseSpecularMapVariable not valid\n";

    m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
    if (!m_pSpecularMapVariable->IsValid())
        std::cout << "m_pSpecularMapVariable not valid\n";

    m_pShininessVariable = m_pEffect->GetVariableByName("gShininess")->AsScalar();
    if (!m_pShininessVariable->IsValid())
        std::cout << "m_pShininessVariable not valid\n";

    //Glossiness Map
    m_pUseGlossinessMapVariable = m_pEffect->GetVariableByName("gUseGlossinessMap")->AsScalar();
    if (!m_pUseGlossinessMapVariable->IsValid())
        std::cout << "m_pUseGlossinessMapVariable not valid\n";

    m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
    if (!m_pGlossinessMapVariable->IsValid())
        std::cout << "m_pGlossinessMapVariable not valid\n";

    //Lights
    m_pLights = m_pEffect->GetVariableByName("gLights")->AsMatrix(); //
    if (!m_pLights->IsValid())
        std::cout << "m_pLights not valid\n";
}

LambertPhongEffect::~LambertPhongEffect()
{
    m_pLights->Release();
    m_pGlossinessMapVariable->Release();
    m_pUseGlossinessMapVariable->Release();
    m_pShininessVariable->Release();
    m_pSpecularMapVariable->Release();
    m_pUseSpecularMapVariable->Release();
    m_pNormalMapVariable->Release();
    m_pUseNormalMapVariable->Release();
    m_pDiffuseMapVariable->Release();
    m_pUseDiffuseMapVariable->Release();
}

void LambertPhongEffect::UpdateEffectVariables(const FMatrix4& world, const FMatrix4& onb, const FMatrix4& projMatrix, Material* pMaterial, const std::vector<FMatrix4>& lightMatrices)
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

    //Specular
    bool useSpecular = pMaterial->UseSpecularMap();
    m_pUseSpecularMapVariable->SetBool(useSpecular);
    if (useSpecular) m_pSpecularMapVariable->SetResource(pMaterial->GetSpecularTexture()->GetTextureResourceView());
    m_pShininessVariable->SetFloat(pMaterial->GetShininess());

    //Glossiness
    bool useGloss = pMaterial->UseGlossinessMap();
    m_pUseGlossinessMapVariable->SetBool(useGloss);
    if (useGloss) m_pGlossinessMapVariable->SetResource(pMaterial->GetGlossinessTexture()->GetTextureResourceView());

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