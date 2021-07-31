#pragma once
class Texture;
class Effect;
class Material final
{
public:

	enum class MaterialWorkflow
	{
		SpecGloss,
		MetalRough
	};

	Material(unsigned int materialID, MaterialWorkflow workflow, Effect* effect);
	~Material();

	Material(const Material& m) = delete;
	Material(Material&& m) = delete;
	Material& operator=(const Material& m) = delete;
	Material& operator=(Material&& m) = delete;

	/* Common */
	unsigned int GetMaterialID() const { return m_MaterialID; }
	Effect* GetEffect() const { return m_pEffect; }
	const MaterialWorkflow& GetMaterialWorkflow() const { return m_MatWorkflow; }

	/* Diffuse */
	bool UseDiffuseMap() const { return m_UseDiffuseMap; }
	float GetDiffuseReflectance() const { return m_DiffuseReflectance; }
	const Elite::RGBColor& GetDiffuseColor() const { return m_SpecularColor; }
	Texture* GetDiffuseTexture() const { return m_pDiffuseTexture; }
	void SetDiffuseTexture(const char* filepath, ID3D11Device* pDevice);
	void SetDiffuseReflectance(float reflectance) { m_DiffuseReflectance = reflectance; }
	void SetDiffuseColor(const Elite::RGBColor& color) { m_DiffuseColor = color; }

	/* Normal */
	bool UseNormalMap() const { return m_UseNormalMap; }
	Texture* GetNormalTexture() const { return m_pNormalTexture; }
	void SetNormalTexture(const char* filepath, ID3D11Device* pDevice);
	
	/* Specular */
	bool UseSpecularMap() const { return m_UseSpecularMap; }
	float GetShininess() const { return m_Shininess; }
	float GetSpecularReflectance() const { return m_SpecularReflectance; }
	const Elite::RGBColor& GetSpecularColor() const { return m_SpecularColor; }
	Texture* GetSpecularTexture() const { return m_pSpecularTexture; }
	void SetShininess(float shininess) { m_Shininess = shininess; }
	void SetSpecularTexture(const char* filepath, ID3D11Device* pDevice);
	void SetSpecularReflectance(float reflectance) { m_SpecularReflectance = reflectance; }
	void SetSpecularColor(const Elite::RGBColor& color) { m_SpecularColor = color; }

	/* Glossiness */
	bool UseGlossinessMap() const { return m_UseGlossinessMap; }
	Texture* GetGlossinessTexture() const { return m_pGlossinessTexture; }
	void SetGlossinessTexture(const char* filepath, ID3D11Device* pDevice);

	/* Metallic */
	bool UseMetalnessMap() const { return m_UseMetalnessMap; }
	Texture* GetMetalnessTexture() const { return m_pMetalnessTexture; }
	void SetMetalnessTexture(const char* filepath, ID3D11Device* pDevice);

	/* Roughness */
	bool UseRoughnessMap() const { return m_UseRoughnessMap; }
	Texture* GetRoughnessTexture() const { return m_pRoughnessTexture; }
	void SetRoughnessTexture(const char* filepath, ID3D11Device* pDevice);

private:
	unsigned int m_MaterialID;
	MaterialWorkflow m_MatWorkflow;
	Effect* m_pEffect;

	bool m_UseDiffuseMap;
	float m_DiffuseReflectance;
	Texture* m_pDiffuseTexture;
	Elite::RGBColor m_DiffuseColor;

	bool m_UseNormalMap;
	Texture* m_pNormalTexture;

	bool m_UseSpecularMap;
	float m_Shininess;
	float m_SpecularReflectance;
	Texture* m_pSpecularTexture;
	Elite::RGBColor m_SpecularColor;

	bool m_UseGlossinessMap;
	Texture* m_pGlossinessTexture;

	bool m_UseMetalnessMap;
	Texture* m_pMetalnessTexture;

	bool m_UseRoughnessMap;
	Texture* m_pRoughnessTexture;
};