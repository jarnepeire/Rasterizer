#pragma once
#include "pch.h"
#include "MaterialManager.h"
#include "Material.h"
#include <algorithm>

MaterialManager::~MaterialManager()
{
    for (size_t i = 0; i < m_pMaterials.size(); ++i)
    {
        delete m_pMaterials[i];
        m_pMaterials[i] = nullptr;
    }
    m_pMaterials.clear();
}

void MaterialManager::AddMaterial(Material* pMaterial)
{
    if (pMaterial) m_pMaterials.push_back(pMaterial);
}

Material* MaterialManager::GetMaterialByID(unsigned int id) const
{
    auto it = std::find_if(m_pMaterials.begin(), m_pMaterials.end(), [id](Material* pMaterial)
        {
            return (pMaterial->GetMaterialID() == id);
        });

    if (it == m_pMaterials.end())
        return nullptr;
    else
        return (*it);
}