#pragma once
#include "EMath.h"
#include "ERGBColor.h"

struct SDL_Surface;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

class Texture final
{
public:
	Texture(const char* filepath, ID3D11Device* pDevice);
	Texture(const Texture& l) = delete;
	Texture(Texture&& l) = delete;
	Texture& operator=(const Texture& l) = delete;
	Texture& operator=(Texture&& l) = delete;
	~Texture();

	/* Returns pointer to texture resource view */
	ID3D11ShaderResourceView* GetTextureResourceView() const { return m_pTextureResourceView; }

	/* Samples and returns a color [0,1] from the stored texture at the given UV-coordinate */
	Elite::RGBColor Sample(const Elite::FVector2& uv) const;

private:
	SDL_Surface* m_pSurface;
	ID3D11Texture2D* m_pTexture;
	ID3D11ShaderResourceView* m_pTextureResourceView;

	/* Initializes the DX texture and shader resource view, given a filepath to a textre */
	void Initialize(const char* filepath, ID3D11Device* pDevice);

	/* Recursive function that remaps the UV-coordinates between [0, 1] in case they become out of range
	-> Uses wrap addressing mode to achieve this */
	float RemapUVComponent(float component) const;
};

