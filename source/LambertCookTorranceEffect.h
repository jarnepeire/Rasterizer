#pragma once
#include "Effect.h"

class LambertCookTorranceEffect final : public Effect
{
public:
	LambertCookTorranceEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	LambertCookTorranceEffect(const LambertCookTorranceEffect& l) = delete;
	LambertCookTorranceEffect(LambertCookTorranceEffect&& l) = delete;
	LambertCookTorranceEffect& operator=(const LambertCookTorranceEffect& l) = delete;
	LambertCookTorranceEffect& operator=(LambertCookTorranceEffect&& l) = delete;
	virtual ~LambertCookTorranceEffect();

	void UpdateEffectVariables(const Elite::FMatrix4& world, const Elite::FMatrix4& onb, const Elite::FMatrix4& projMatrix, Material* pMaterial, const std::vector<Elite::FMatrix4>& lightMatrices) override;

	/* Returns pointer to diffuse map dx variable */
	ID3DX11EffectShaderResourceVariable* GetDiffuseMapVariable() const { return m_pDiffuseMapVariable; }

	/* Returns pointer to normal map dx variable */
	ID3DX11EffectShaderResourceVariable* GetNormalMapVariable() const { return m_pNormalMapVariable; }

private:
	//Diffuse
	ID3DX11EffectScalarVariable* m_pUseDiffuseMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;

	//Normal
	ID3DX11EffectScalarVariable* m_pUseNormalMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;

	//Metalness
	ID3DX11EffectScalarVariable* m_pUseMetalnessMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pMetalnessMapVariable;

	//Roughness
	ID3DX11EffectScalarVariable* m_pUseRoughnessMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pRoughnessMapVariable;

	//Lights
	ID3DX11EffectMatrixVariable* m_pLights;
};