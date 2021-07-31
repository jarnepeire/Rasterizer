/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render too + DirectX initialization.
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include <vector>
#include "MaterialManager.h"
#include "LightManager.h"

struct HitRecord;
struct SDL_Window;
struct SDL_Surface;
struct KeyBindInfo;

class TriangleMesh;
class Triangle;
class Camera;
class Material;

//Render type
enum class ERendererType : unsigned int
{
	SRAS = 0,
	DirectX = 1,

	//Change value on adding more types
	NUM_OF_OPTIONS = 2
};

namespace Elite
{
	class Renderer final
	{
	public:

		/* Rule of 5 */
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		/* Renders the given triangle meshes
			- linked to its correct material in the material manager
			- using the given light manager's lights for shading
			- in the view of the given camera */
		void Render(const std::vector<TriangleMesh*>& pTriangleMeshes, const MaterialManager& materials, LightManager& lights, Camera* pCamera, const KeyBindInfo& keyBindInfo, ERendererType type);

		/* Save back buffer pixels to image */
		bool SaveBackbufferToImage() const;

		/* Returns pointers to the d3d11 device */
		ID3D11Device* GetDevice() const { return m_pDevice; }

		/* Returns pointers to the d3d11 device context */
		ID3D11DeviceContext* GetDeviceContext() const { return m_pDeviceContext; }

	private:
		SDL_Window* m_pWindow;
		uint32_t m_Width;
		uint32_t m_Height;
		bool m_IsInitialized;

		/* SRAS Variables */
		SDL_Surface* m_pFrontBuffer;
		SDL_Surface* m_pBackBuffer;
		uint32_t* m_pBackBufferPixels;
		std::vector<float> m_DepthBuffer;

		/* DirectX Variables */
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGIFactory* m_pDXGIFactory;
		IDXGISwapChain* m_pSwapChain;

		//Resources are actual data on the GPU
		ID3D11Resource* m_pRenderTargetBuffer;
		ID3D11Texture2D* m_pDepthStencilBuffer;

		//Views are used with your DeviceContext to bind them to the pipeline at certain points (how you want to use resources)
		ID3D11RenderTargetView* m_pRenderTargetView;
		ID3D11DepthStencilView* m_pDepthStencilView;

		/* Private functions */
		HRESULT InitializeDirectX();
		void RenderSRAS(const std::vector<TriangleMesh*>& pTriangleMeshes, const MaterialManager& materials, const LightManager& lights, Camera* pCamera, const KeyBindInfo& keyBindInfo);
		void RenderDX(const std::vector<TriangleMesh*>& pTriangleMeshes, const MaterialManager& materials, const LightManager& lights, Camera* pCamera);

		void PixelLoop(const Triangle& triangle, const MaterialManager& materialManager, const std::vector<Light*>& pLights, const KeyBindInfo& keyBindInfo);
		Elite::RGBColor PixelShading(const HitRecord& hitRecord, const MaterialManager& materialManager, const std::vector<Light*>& pLights, const KeyBindInfo& keyBindInfo);
	};
}

#endif