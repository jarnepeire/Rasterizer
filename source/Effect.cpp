#pragma once
#include "pch.h"
#include "Effect.h"
#include <sstream>

using namespace Elite;
Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
    : m_pEffect(nullptr)
    , m_pTechnique(nullptr)
    , m_pMatWorldViewProjVariable(nullptr)
    , m_pMatViewInverseVariable(nullptr)
    , m_pMatWorldVariable(nullptr)
    , m_pSamplerVariable(nullptr)
    , m_pRasterizerVariable(nullptr)
    , m_pBlendVariable(nullptr)
{
    m_pEffect = LoadEffect(pDevice, assetFile);

    m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
    if (!m_pTechnique->IsValid())
        std::cout << "m_pTechnique not valid \n";

    //Transfer WVP to GPU
    m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
    if (!m_pMatWorldViewProjVariable->IsValid())
        std::cout << "m_pMatWorldViewProjVariable not valid\n";

    //Transfer VI to GPU
    m_pMatViewInverseVariable = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();
    if (!m_pMatViewInverseVariable->IsValid())
        std::cout << "m_pMatViewInverseVariable not valid\n";

    //Transfer WORLD to GPU
    m_pMatWorldVariable = m_pEffect->GetVariableByName("gWorld")->AsMatrix();
    if (!m_pMatWorldVariable->IsValid())
        std::cout << "m_pMatWorldVariable not valid\n";

    //Transfer Sampler State
    m_pSamplerVariable = m_pEffect->GetVariableByName("gSamplerState")->AsSampler();
    if (!m_pSamplerVariable->IsValid())
        std::cout << "m_pSamplerVariable not valid\n";

    //Transfer Rasterizer State
    m_pRasterizerVariable = m_pEffect->GetVariableByName("gRasterizerState")->AsRasterizer();
    if (!m_pRasterizerVariable->IsValid())
        std::cout << "m_pRasterizerVariable not valid\n";

    //Transfer Blend State
    m_pBlendVariable = m_pEffect->GetVariableByName("gBlendState")->AsBlend();
    if (!m_pBlendVariable->IsValid())
        std::cout << "m_pBlendVariable not valid\n";
}

Effect::~Effect()
{
    m_pBlendVariable->Release();
    m_pRasterizerVariable->Release();
    m_pSamplerVariable->Release();
    m_pMatWorldVariable->Release();
    m_pMatViewInverseVariable->Release();
    m_pMatWorldViewProjVariable->Release();
    m_pTechnique->Release();
    m_pEffect->Release();
}

void Effect::UpdateEffectVariables(const FMatrix4& world, const FMatrix4& onb, const FMatrix4& projMatrix, Material* pMaterial, const std::vector<FMatrix4>& lightMatrices)
{
    UNREFERENCED_PARAMETER(lightMatrices);
    UNREFERENCED_PARAMETER(pMaterial);

    FMatrix4 invViewMatrix = Elite::Inverse(onb);
    FMatrix4 wvp = projMatrix * invViewMatrix * world;

    m_pMatWorldViewProjVariable->SetMatrix((float*)&wvp);
    m_pMatViewInverseVariable->SetMatrix((float*)&onb);
    m_pMatWorldVariable->SetMatrix((float*)&world);

}

void Effect::UpdateStateVariables(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ESamplerState sampleState, ECullMode cullmode, EBlendState blendstate)
{
    UpdateSamplerState(pDevice, pDeviceContext, sampleState);
    UpdateRasterizerState(pDevice, pDeviceContext, cullmode);
    UpdateBlendState(pDevice, pDeviceContext, blendstate);
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
    HRESULT result = S_OK;
    ID3D10Blob* pErrorBlob = nullptr;
    ID3DX11Effect* pEffect;

    DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3DCOMPILE_DEBUG;
    shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    result = D3DX11CompileEffectFromFile(assetFile.c_str(),
        nullptr,
        nullptr,
        shaderFlags,
        0,
        pDevice,
        &pEffect,
        &pErrorBlob);

    if (FAILED(result))
    {
        if (pErrorBlob != nullptr)
        {
            char* pErrors = (char*)pErrorBlob->GetBufferPointer();

            std::stringstream ss;
            for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
                ss << pErrors[i];

            OutputDebugString(ss.str().c_str());
            pErrorBlob->Release();
            pErrorBlob = nullptr;

            std::cout << ss.str() << std::endl;
        }
        else
        {
            std::wstringstream ss;
            ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
            std::wcout << ss.str() << std::endl;
            return nullptr;
        }
    }

    return pEffect;
}

void Effect::UpdateSamplerState(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ESamplerState sampleState)
{
    //Reference to why release twice: https://gamedev.stackexchange.com/questions/49614/how-to-debug-direct3d-resource-leak
    //This function only gets called on key input for swap! Not creating/releasing every frame! 
    //Get current sampler state
    ID3D11SamplerState* pCurrSamplerState = nullptr;
    m_pSamplerVariable->GetSampler(0, &pCurrSamplerState);

    //Get current description
    D3D11_SAMPLER_DESC samplerDesc{};
    pCurrSamplerState->GetDesc(&samplerDesc);
    
    //Release after Get sampler state call, we simply wanted the descriptor! 
    pCurrSamplerState->Release();

    ////Edit descriptor to fit needs
    samplerDesc.Filter = (D3D11_FILTER)sampleState;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    
    //Re-create the sampler state, to then update it back to the shader
    pDevice->CreateSamplerState(&samplerDesc, &pCurrSamplerState);
    
    //Set context to appropriate sampler state in pipeline
    pDeviceContext->PSSetSamplers(0, 1, &pCurrSamplerState);
    
    //Update sampler variable
    m_pSamplerVariable->SetSampler(0, pCurrSamplerState);
    
    //Release after create sampler state call! 
    pCurrSamplerState->Release(); 
}

void Effect::UpdateRasterizerState(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ECullMode cullmode)
{
    //Reference to why release twice: https://gamedev.stackexchange.com/questions/49614/how-to-debug-direct3d-resource-leak
    //This function only gets called on key input for swap! Not creating/releasing every frame! 
    //Get current rasterizer state
    ID3D11RasterizerState* pRasterizerSate{};
    m_pRasterizerVariable->GetRasterizerState(0, &pRasterizerSate);

    //Get current descriptor
    D3D11_RASTERIZER_DESC rasterizerDesc{};
    pRasterizerSate->GetDesc(&rasterizerDesc);

    //Release after Get rasterizer state call, we simply wanted the descriptor! 
    pRasterizerSate->Release();

    //Edit descriptor
    rasterizerDesc.CullMode = (D3D11_CULL_MODE)cullmode;
    //rasterizerDesc.FrontCounterClockwise = (cullmode == ECullMode::FrontCulling) ? false : true; 

    //Re-create rasterizer state
    pDevice->CreateRasterizerState(&rasterizerDesc, &pRasterizerSate);

    //Set context to appriopriate rasterizer state in pipeline
    pDeviceContext->RSSetState(pRasterizerSate);

    //Update variable
    m_pRasterizerVariable->SetRasterizerState(0, pRasterizerSate);

    //Release after create rasterizer state call! 
    pRasterizerSate->Release();
}

void Effect::UpdateBlendState(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, EBlendState blendstate)
{
    //Reference to why release twice: https://gamedev.stackexchange.com/questions/49614/how-to-debug-direct3d-resource-leak
    //This function only gets called on key input for swap! Not creating/releasing every frame! 
    //Get current rasterizer state
    ID3D11BlendState* pBlendState{};
    m_pBlendVariable->GetBlendState(0, &pBlendState);

    //Get current descriptor
    D3D11_BLEND_DESC blendDesc{};
    pBlendState->GetDesc(&blendDesc);

    //Release after Get blend state call, we simply wanted the descriptor! 
    pBlendState->Release();

    //Edit descriptor (we only have 1 render target here)
    if (blendstate == EBlendState::BlendAdd)
    {
        blendDesc.RenderTarget[0].BlendEnable = true;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)blendstate;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
    }
    else //No blend
    {
        blendDesc.RenderTarget[0].BlendEnable = false;
    }

    //Re-create blend state
    pDevice->CreateBlendState(&blendDesc, &pBlendState);

    //Set context to appriopriate blend state in pipeline (ref: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-omsetblendstate)
    //BlendFactor = array of blend factors for each rgba component (passing null equals in {1,1,1,1}) 
    //SampleMask, 32-bit sample coverage, default value is 0xffffffff
    pDeviceContext->OMSetBlendState(pBlendState, NULL, 0xffffffff);

    //Update variable
    m_pBlendVariable->SetBlendState(0, pBlendState);

    //Release after blend rasterizer state call!
    pBlendState->Release();
}