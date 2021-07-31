#pragma once
#include "pch.h"
#include "LightManager.h"
#include "Light.h"

LightManager::LightManager()
	: m_pLights()
	, m_LightMatrices()
{
}

LightManager::~LightManager()
{
	for (size_t t = 0; t < m_pLights.size(); t++)
	{
		delete m_pLights[t];
		m_pLights[t] = nullptr;
	}

	m_pLights.clear();
	m_LightMatrices.clear();
}

size_t LightManager::AddLight(Light* pLight)
{
	if (pLight)
	{
		size_t index = m_pLights.size();
		if (index >= MAX_LIGHTS)
			return size_t(-1);

		m_pLights.push_back(pLight);
		m_LightMatrices.push_back(pLight->GetLightMatrix());
		return index;
	}
	return size_t(-1);
}