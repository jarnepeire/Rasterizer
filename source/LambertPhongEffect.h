#pragma once
#include "Effect.h"

class LambertPhongEffect final : public Effect
{
public:
	LambertPhongEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	LambertPhongEffect(const LambertPhongEffect& l) = delete;
	LambertPhongEffect(LambertPhongEffect&& l) = delete;
	LambertPhongEffect& operator=(const LambertPhongEffect& l) = delete;
	LambertPhongEffect& operator=(LambertPhongEffect&& l) = delete;
	virtual ~LambertPhongEffect();

	void UpdateEffectVariables(const Elite::FMatrix4& world, const Elite::FMatrix4& onb, const Elite::FMatrix4& projMatrix, Material* pMaterial, const std::vector<Elite::FMatrix4>& lightMatrices) override;

	/* Returns pointer to diffuse map dx variable */
	ID3DX11EffectShaderResourceVariable* GetDiffuseMapVariable() const { return m_pDiffuseMapVariable; }

	/* Returns pointer to normal map dx variable */
	ID3DX11EffectShaderResourceVariable* GetNormalMapVariable() const { return m_pNormalMapVariable; }

	/* Returns pointer to specular map dx variable */
	ID3DX11EffectShaderResourceVariable* GetSpecularMapVariable() const { return m_pSpecularMapVariable; }

	/* Returns pointer to glossiness map dx variable */
	ID3DX11EffectShaderResourceVariable* GetGlossinessMapVariable() const { return m_pGlossinessMapVariable; }

private:
	//Diffuse
	ID3DX11EffectScalarVariable* m_pUseDiffuseMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;

	//Normal
	ID3DX11EffectScalarVariable* m_pUseNormalMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;

	//Specular
	ID3DX11EffectScalarVariable* m_pShininessVariable;
	ID3DX11EffectScalarVariable* m_pUseSpecularMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;

	//Glossiness
	ID3DX11EffectScalarVariable* m_pUseGlossinessMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;

	//Lights
	ID3DX11EffectMatrixVariable* m_pLights;
};