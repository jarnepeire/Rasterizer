#pragma once
#include <vector>

class Material;
class MaterialManager final
{
public:
	MaterialManager() = default;
	~MaterialManager();

	MaterialManager(const MaterialManager& m) = delete;
	MaterialManager(MaterialManager&& m) = delete;
	MaterialManager& operator=(const MaterialManager& m) = delete;
	MaterialManager& operator=(MaterialManager&& m) = delete;

	/* Adds material to the material manager class */
	void AddMaterial(Material* pMaterial);

	/* Returns const reference to the vector holding all the material pointers */
	const std::vector<Material*>& GetMaterials() const { return m_pMaterials; }

	/* Looks through all materials and returns the material with the similar ID to the passed parameter ID
		returns nullptr on invalid ID */
	Material* GetMaterialByID(unsigned int id) const;

private:
	std::vector<Material*> m_pMaterials;
};