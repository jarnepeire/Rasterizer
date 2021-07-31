#pragma once
#include "pch.h"
#include "Texture.h"
#include "SDL_image.h"

Texture::Texture(const char* filepath, ID3D11Device* pDevice)
	: m_pSurface()
	, m_pTexture()
	, m_pTextureResourceView()
{
	Initialize(filepath, pDevice);
}

Texture::~Texture()
{
	m_pTextureResourceView->Release();
	m_pTexture->Release();
	SDL_FreeSurface(m_pSurface);
}

Elite::RGBColor Texture::Sample(const Elite::FVector2& uv) const
{
	Uint8 r;
	Uint8 g;
	Uint8 b;

	//Extra check if UV components are between [0,1]
	int x = int(RemapUVComponent(uv.x) * m_pSurface->w);
	int y = int(RemapUVComponent(uv.y) * m_pSurface->h);

	//Determine index -> then get RGB value of the pixel on that index
	int index = x + (y * m_pSurface->w);
	Uint32* pixels = (Uint32*)m_pSurface->pixels;
	SDL_GetRGB(pixels[index], m_pSurface->format, &r, &g, &b);
	return Elite::RGBColor(r / 255.f, g / 255.f, b / 255.f);
}

void Texture::Initialize(const char* filepath, ID3D11Device* pDevice)
{
	m_pSurface = IMG_Load(filepath);

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = m_pSurface->w;
	desc.Height = m_pSurface->h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = m_pSurface->pixels;
	initData.SysMemPitch = static_cast<UINT>(m_pSurface->pitch);
	initData.SysMemSlicePitch = static_cast<UINT>(m_pSurface->h * m_pSurface->pitch);

	HRESULT result = pDevice->CreateTexture2D(&desc, &initData, &m_pTexture);
	if (FAILED(result))
		return;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVdesc{};
	SRVdesc.Format = desc.Format;
	SRVdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVdesc.Texture2D.MipLevels = 1;

	result = pDevice->CreateShaderResourceView(m_pTexture, &SRVdesc, &m_pTextureResourceView);
	if (FAILED(result))
		return;
}

float Texture::RemapUVComponent(float component) const
{
	if (component < 0.f)
		return RemapUVComponent(component + 1);

	else if (component > 1.f)
		return RemapUVComponent(component - 1);

	return component;
}
