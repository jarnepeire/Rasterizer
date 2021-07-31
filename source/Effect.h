#pragma once
#include "EMath.h"
#include <vector>

enum class ESamplerState : int
{
	Point = D3D11_FILTER_MIN_MAG_MIP_POINT,
	Linear = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
	Anisotropic = D3D11_FILTER_ANISOTROPIC,
};

enum class ECullMode : int
{
	NoCulling = D3D11_CULL_NONE,
	FrontCulling = D3D11_CULL_FRONT,
	BackCulling = D3D11_CULL_BACK
};

enum class EBlendState : int
{
	BlendNone = 0,
	BlendAdd = D3D11_BLEND_OP_ADD
};

class Material;
class Effect
{
public:
	Effect(const Effect& l) = delete;
	Effect(Effect&& l) = delete;
	Effect& operator=(const Effect& l) = delete;
	Effect& operator=(Effect&& l) = delete;
	virtual ~Effect();

	/* Updates the DX effect variables with the appropriate passed parameters */
	virtual void UpdateEffectVariables(const Elite::FMatrix4& world, const Elite::FMatrix4& onb, const Elite::FMatrix4& projMatrix, Material* pMaterial, const std::vector<Elite::FMatrix4>& lightMatrices);

	/* Updates the DX effect variables with the appropriate passed parameters */
	void UpdateStateVariables(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ESamplerState sampleState, ECullMode cullmode, EBlendState blendstate);

	/* Returns pointer to DX Effect*/
	ID3DX11Effect* GetDX11Effect() { return m_pEffect; }

	/* Returns pointer to DX technique, depending on paster sample state parameter */
	ID3DX11EffectTechnique* GetTechnique() { return m_pTechnique; }

protected:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	
	/* Core of the Effect */
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique = nullptr;

	/* Global Variables */
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable = nullptr;
	ID3DX11EffectMatrixVariable* m_pMatViewInverseVariable = nullptr;
	ID3DX11EffectMatrixVariable* m_pMatWorldVariable = nullptr;

	/* Different States*/
	ID3DX11EffectSamplerVariable* m_pSamplerVariable = nullptr;
	ID3DX11EffectRasterizerVariable* m_pRasterizerVariable = nullptr;
	ID3DX11EffectBlendVariable* m_pBlendVariable = nullptr;


private:
	/* Private Functions */
	ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	void UpdateSamplerState(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ESamplerState sampleState);
	void UpdateRasterizerState(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ECullMode cullmode);
	void UpdateBlendState(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, EBlendState blendstate);

};