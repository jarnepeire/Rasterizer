#pragma once
#include "Effect.h"

class CombustionEffect final : public Effect
{
public:
	CombustionEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	CombustionEffect(const CombustionEffect& l) = delete;
	CombustionEffect(CombustionEffect&& l) = delete;
	CombustionEffect& operator=(const CombustionEffect& l) = delete;
	CombustionEffect& operator=(CombustionEffect&& l) = delete;
	virtual ~CombustionEffect();

	void UpdateEffectVariables(const Elite::FMatrix4& world, const Elite::FMatrix4& onb, const Elite::FMatrix4& projMatrix, Material* pMaterial, const std::vector<Elite::FMatrix4>& lightMatrices) override;

	/* Returns pointer to diffuse map dx variable */
	ID3DX11EffectShaderResourceVariable* GetDiffuseMapVariable() const { return m_pDiffuseMapVariable; }

private:
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
};