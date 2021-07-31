#pragma once
#include "pch.h"

//Project includes
#include "ERenderer.h"
#include "TriangleMesh.h"
#include "Material.h"
#include "Texture.h"
#include "Camera.h"
#include "TriangleMesh.h"
#include "Triangle.h"
#include "DirectionalLight.h"
#include "BRDF.h"

using Topology = EPrimitiveTopology;
using namespace Elite;
Elite::Renderer::Renderer(SDL_Window * pWindow)
	: m_pWindow(pWindow)
	, m_Width()
	, m_Height()
	, m_IsInitialized(false)
	, m_pFrontBuffer(nullptr)
	, m_pBackBuffer(nullptr)
	, m_pBackBufferPixels(nullptr)
	, m_DepthBuffer()
	, m_pDevice(nullptr)
	, m_pDeviceContext()
	, m_pDXGIFactory()
	, m_pSwapChain()
	, m_pRenderTargetBuffer()
	, m_pDepthStencilBuffer()
	, m_pRenderTargetView()
	, m_pDepthStencilView()
{
	//Initialization general variables
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);

	//Initialize SRAS variables
	m_pFrontBuffer = SDL_GetWindowSurface(m_pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;
	m_DepthBuffer = std::vector<float>(size_t(m_Width * m_Height), FLT_MAX);

	//Initialize DirectX pipeline
	if (SUCCEEDED(InitializeDirectX()))
	{
		m_IsInitialized = true;
		std::cout << "DirectX is ready\n";
	}
}

Elite::Renderer::~Renderer()
{
	m_pRenderTargetView->Release();
	m_pRenderTargetBuffer->Release();
	m_pDepthStencilView->Release();
	m_pDepthStencilBuffer->Release();
	m_pSwapChain->Release();
	m_pDXGIFactory->Release();
	if (m_pDeviceContext)
	{
		
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		m_pDeviceContext->Release();
	}
	m_pDevice->Release();
}

void Elite::Renderer::Render(const std::vector<TriangleMesh*>& pTriangleMeshes, const MaterialManager& materials, LightManager& lights, Camera* pCamera, const KeyBindInfo& keyBindInfo, ERendererType type)
{
	if (type == ERendererType::SRAS)
	{
		RenderSRAS(pTriangleMeshes, materials, lights, pCamera, keyBindInfo);
	}
	else if (type == ERendererType::DirectX)
	{
		RenderDX(pTriangleMeshes, materials, lights, pCamera);
	}
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

HRESULT Elite::Renderer::InitializeDirectX()
{
	//Use documentation!: https://docs.microsoft.com/en-us/windows/win32/direct3d11/atoc-dx-graphics-direct3d-11
	//Look up functions and variables on MSDN

	//Create Device and Device Context, using hardware acceleration
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	uint32_t createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pDeviceContext);
	if (FAILED(result))
		return result;

	//Create DXGI Factory to create SwapChain based on hardware
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory));
	if (FAILED(result))
		return result;

	//Create SwapChain Descriptor
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = m_Width;
	swapChainDesc.BufferDesc.Height = m_Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	//Get the handle HWND from the SDL Backbuffer
	SDL_SysWMinfo sysWMInfo{};
	SDL_VERSION(&sysWMInfo.version);
	SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
	swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

	//Create SwapChain and hook it into the handle of the SDL window
	result = m_pDXGIFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
	if (FAILED(result))
		return result;

	//Create Depth/Stencil buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width = m_Width;
	depthStencilDesc.Height = m_Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	//BindFlags = pieces of information that whenever u use it in the device context, 
	//it will be used for the driver to know how you use it (optimization and no race condition reasons)
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//Create Depth/Stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = depthStencilDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//Create the actual resource and the matching resource view
	result = m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer);
	if (FAILED(result))
		return result;

	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(result))
		return result;

	//Create the RenderTargetView
	result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
	if (FAILED(result))
		return result;

	result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, 0, &m_pRenderTargetView);
	if (FAILED(result))
		return result;

	//Bind views to the output merger stage
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	//Set up the viewport
	D3D11_VIEWPORT viewPort{};
	viewPort.Width = static_cast<float>(m_Width);
	viewPort.Height = static_cast<float>(m_Height);
	viewPort.TopLeftX = 0.f;
	viewPort.TopLeftY = 0.f;
	viewPort.MinDepth = 0.f;
	viewPort.MaxDepth = 1.f;
	m_pDeviceContext->RSSetViewports(1, &viewPort);

	return result;
}

void Elite::Renderer::RenderSRAS(const std::vector<TriangleMesh*>& pTriangleMeshes, const MaterialManager& materials, const LightManager& lights, Camera* pCamera, const KeyBindInfo& keyBindInfo)
{
	SDL_LockSurface(m_pBackBuffer);

	//Clear depth and color buffer
	for (uint32_t r = 0; r < m_Height; ++r)
	{
		for (uint32_t c = 0; c < m_Width; ++c)
		{
			m_DepthBuffer[c + (r * m_Width)] = FLT_MAX;
			m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(50),
				static_cast<uint8_t>(50),
				static_cast<uint8_t>(50));
		}
	}

	//For every triangle mesh
	const auto& pLights = lights.GetLights();
	for (TriangleMesh* pTriangleMesh : pTriangleMeshes)
	{
		//Check if valid
		if (!pTriangleMesh->IsValid())
			continue;

		//Gather data from triangle mesh
		const auto& indexBuffer = pTriangleMesh->GetIndexBuffer();
		const auto& vertices = pTriangleMesh->GetVertexBuffer();
		Topology topology = pTriangleMesh->GetPrimitiveTopology();
		size_t incrementValue = (topology == Topology::TriangleList) ? 3 : 1;
		bool swapOnOdd = (topology == Topology::TriangleList) ? false : true;

		//Start looping over all indices 
		for (size_t i = 0; i < indexBuffer.size() - 2; i += incrementValue)
		{
			//Create triangle
			Triangle t = (swapOnOdd && i & 1)
				? Triangle //Swap last 2 indices on odd triangle in strip
				(
					Vertex_Input{ vertices[indexBuffer[i]] },
					Vertex_Input{ vertices[indexBuffer[i + 2]] },
					Vertex_Input{ vertices[indexBuffer[i + 1]] }
				)
				: Triangle //Else continue making triangles from a list or even triangle in strip
				(
					Vertex_Input{ vertices[indexBuffer[i]] },
					Vertex_Input{ vertices[indexBuffer[i + 1]] },
					Vertex_Input{ vertices[indexBuffer[i + 2]] }
			);

			//Transform triangle
			t.TransformVertices((float)m_Width, (float)m_Height, pTriangleMesh->GetWorldMatrix(), pCamera, keyBindInfo, true);

			//If triangle already isn't valid, continue
			if (!t.IsInsideFrustum())
				continue;

			//Set cullmode for upcoming hit check
			t.SetCullMode(pTriangleMesh->GetCullMode());

			//If triangle is clipped, loop over the pixels surrounding that triangle and render
			if (t.IsTriangleClipped())
			{
				auto& clippedTriangles = t.GetClippedTriangles();
				for (const Triangle& clippedTriangle : clippedTriangles)
				{
					PixelLoop(clippedTriangle, materials, pLights, keyBindInfo);
				}
			}
			//Else loop over the pixels surrounding the current triangle and render
			else
			{
				PixelLoop(t, materials, pLights, keyBindInfo);
			}
		}
	}

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Elite::Renderer::RenderDX(const std::vector<TriangleMesh*>& pTriangleMeshes, const MaterialManager& materials, const LightManager& lights, Camera* pCamera)
{
	if (!m_IsInitialized)
		return;

	//Clear buffers
	RGBColor clearColor = RGBColor(0.f, 0.f, 0.3f);
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	//Render
	for (TriangleMesh* pTriangleMesh : pTriangleMeshes)
	{
		Material* pMat = materials.GetMaterialByID(pTriangleMesh->GetMaterialID());
		if (!pMat)
		{
			std::cout << "Couldn't match Triangle Mesh with ID: \"" << pTriangleMesh->GetMaterialID() << "\" to material\n";
			continue;
		}
		pTriangleMesh->Render(m_pDevice, m_pDeviceContext, pCamera, pMat, lights.GetLightMatrices());
	}

	//Present
	m_pSwapChain->Present(0, 0);
}

void Elite::Renderer::PixelLoop(const Triangle& triangle, const MaterialManager& materialManager, const std::vector<Light*>& pLights, const KeyBindInfo& keyBindInfo)
{
	//Adjust bounding box
	BoundingBox boundingBox{};
	triangle.AdjustBoundingBox(boundingBox, (float)m_Width, (float)m_Height);

	//Loop over all pixels
	for (uint32_t r = uint32_t(boundingBox.TopLeft.y); r < uint32_t(boundingBox.BottomRight.y); ++r)
	{
		for (uint32_t c = uint32_t(boundingBox.TopLeft.x); c < uint32_t(boundingBox.BottomRight.x); ++c)
		{
			//Create pixel point and empty hitrecord to store the information of the hit
			FPoint2 pixel{ float(c), float(r) };
			HitRecord hitRecord{};

			if (triangle.Hit(pixel, hitRecord))
			{
				if (hitRecord.InterpolatedZ > 0.f && hitRecord.InterpolatedZ < 1.f && hitRecord.InterpolatedZ <= m_DepthBuffer[c + (r * m_Width)])
				{
					//Store closer depth value
					m_DepthBuffer[c + (r * m_Width)] = hitRecord.InterpolatedZ;

					//You can start shading this pixel now 
					RGBColor finalColor = PixelShading(hitRecord, materialManager, pLights, keyBindInfo);

					//Fill the pixels
					m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
						static_cast<uint8_t>(finalColor.r * 255.f),
						static_cast<uint8_t>(finalColor.g * 255.f),
						static_cast<uint8_t>(finalColor.b * 255.f));
				}
			}
		}
	}
}

Elite::RGBColor Elite::Renderer::PixelShading(const HitRecord& hitRecord, const MaterialManager& materialManager, const std::vector<Light*>& pLights, const KeyBindInfo& keyBindInfo)
{
	//Coloring in the returned result
	RGBColor finalColor{};
	if (keyBindInfo.UseDepthBufferAsColor)
	{
		//Use depth as gradient for color
		float depthColor = Elite::Remap(hitRecord.InterpolatedZ, 0.985f, 1.f);
		finalColor = { depthColor, depthColor, depthColor };
	}
	else
	{
		//If not using material, then use the color defined in the triangle
		if (keyBindInfo.UseMaterial)
		{
			//Get sampled texture color from texture matching the triangle
			Material* pMat = materialManager.GetMaterialByID(hitRecord.MatID);
			if (!pMat)
				return RGBColor(0, 0, 0);

			//------ Diffuse ------
			Elite::RGBColor diffuse{};
			float diffuseReflectance = pMat->GetDiffuseReflectance();
			if (pMat->UseDiffuseMap())
			{
				diffuse = pMat->GetDiffuseTexture()->Sample(hitRecord.InterpolatedUV) * diffuseReflectance;
			}
			else
			{
				diffuse = pMat->GetDiffuseColor() * diffuseReflectance;
			}

			//------ Normal ------
			FVector3 newNormal{};
			if (pMat->UseNormalMap())
			{
				//We define our tangent space
				FVector3 binormal = Cross(hitRecord.InterpolatedTangent, hitRecord.InterpolatedVertexNormal);
				FMatrix3 localTangentSpace = FMatrix3(hitRecord.InterpolatedTangent, binormal, hitRecord.InterpolatedVertexNormal);

				//Sample normal map
				Elite::RGBColor normalSample = pMat->GetNormalTexture()->Sample(hitRecord.InterpolatedUV);

				//An RBG Color goes from [0, 255] range, while we will need this in [-1, 1]
				//Sampled value is already returned in range [0, 1]
				normalSample = (normalSample * 2.f) - RGBColor(1.f, 1.f, 1.f);

				//Transform to tangent space
				newNormal = GetNormalized(localTangentSpace * FVector3(normalSample.r, normalSample.g, normalSample.b));
			}
			else
			{
				//This value is already normalized
				newNormal = hitRecord.InterpolatedVertexNormal;
			}

			//Data to store information while contributing every light
			for (Light* pLight : pLights)
			{
				//------ Irradiance ------
				Elite::RGBColor irradiance = pLight->GetCalculatedIrradianceColor(newNormal, true);

				//Decide what BRDF to use:
				Material::MaterialWorkflow workflow{ pMat->GetMaterialWorkflow() };
				if (workflow == Material::MaterialWorkflow::SpecGloss)
				{
					//------ Phong BRDF ------
					//------ Specular ------
					//Color
					RGBColor specColor{};
					if (pMat->UseSpecularMap())
					{
						specColor = pMat->GetSpecularTexture()->Sample(hitRecord.InterpolatedUV);
					}
					else
					{
						specColor = pMat->GetSpecularColor();
					}

					//Shininess
					float shininess = pMat->GetShininess();
					if (pMat->UseGlossinessMap())
					{
						shininess *= pMat->GetGlossinessTexture()->Sample(hitRecord.InterpolatedUV).r;
					}

					//Reflectance
					float specReflectance = pMat->GetSpecularReflectance();

					//Because of lightDir being initialized for a LHS in DX, we have to invert Z to get same effect
					FVector3 lightDir = pLight->GetDirection(hitRecord, true);
					RGBColor specularReflect = BRDF::Phong(specReflectance, shininess, -lightDir, hitRecord.ViewDirection, newNormal);
					Elite::RGBColor phongSpecular = specColor * specularReflect;

					//Adding up contribution
					finalColor += irradiance * (diffuse / float(E_PI)) + phongSpecular;
					
				}
				else if (workflow == Material::MaterialWorkflow::MetalRough)
				{
					//------ Lambert Cook-Torrance BRDF ------
					//Because of lightDir being initialized for a LHS in DX, we have to invert Z to get same effec
					FVector3 lightDir = pLight->GetDirection(hitRecord, true);

					//------ Roughness ------
					float roughness = 0.6f;
					if (pMat->UseRoughnessMap())
					{
						roughness = pMat->GetRoughnessTexture()->Sample(hitRecord.InterpolatedUV).r;
					}

					//------ Metallic ------
					int metallic = 0;
					if (pMat->UseMetalnessMap())
					{
						float metalSample = pMat->GetMetalnessTexture()->Sample(hitRecord.InterpolatedUV).r;
						if (metalSample > 0.5f)
							metallic = 1;
						else if (metalSample < 0.5f)
							metallic = 0;
					}

					//LambertCookTorrance BRDF
					RGBColor lamertCookTorranceBRDF = BRDF::LambertCookTorrance(diffuse, metallic, roughness, -lightDir, hitRecord.ViewDirection, newNormal);

					//Adding up contribution
					finalColor += irradiance * lamertCookTorranceBRDF;
				}
			}
		}
		else
		{
			finalColor = hitRecord.InterpolatedColor;
		}
	}

	//Call max to one on color overflow
	if (finalColor.r > 1.f || finalColor.g > 1.f || finalColor.b > 1.f)
		finalColor.MaxToOne();

	return finalColor;
}