#pragma once
#include <d3d11.h>
#include "Structs.h"
#include <vector>

enum class ESamplerState : int;
enum class ECullMode : int;
enum class EBlendState : int;
class Effect;
class Camera;
class Texture;
class Material;

enum class EPrimitiveTopology
{
	TriangleList = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	TriangleStrip = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
};

class TriangleMesh final
{
public:
	TriangleMesh(unsigned int id, const std::vector<Vertex_Input>& vertices, const std::vector<uint32_t>& indices, EPrimitiveTopology top = EPrimitiveTopology::TriangleList);
	TriangleMesh(const TriangleMesh& t) = delete;
	TriangleMesh(TriangleMesh&& t) = delete;
	TriangleMesh& operator=(TriangleMesh&& t) = delete;
	TriangleMesh& operator=(const TriangleMesh& t) = delete;
	~TriangleMesh();
	
	/* Initializes the triangle mesh for DirectX -> sets vertex/index buffer and input-layout */
	void Initialize(ID3D11Device* pDevice, Effect* pEffect);

	/* Updates the triangle mesh */
	void Update(float deltaT);

	/* Renders the current triangle mesh through a given camera, shaded using the given lights and given the appropriate material linked to the mesh */
	void Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, Camera* pCamera, Material* pMaterial, const std::vector<Elite::FMatrix4>& lightMatrices);

	/* Returns material ID linked to this triangle mesh*/
	unsigned int GetMaterialID() const { return m_MaterialID; }

	/* Returns a const reference to the vector holding all the indices */
	const std::vector<uint32_t>& GetIndexBuffer() const { return m_Indices; }

	/* Returns a const reference to the vector holding all the input vertices */
	const std::vector<Vertex_Input>& GetVertexBuffer() const { return m_VertexBuffer; }

	/* Returns const reference to the primitive topology of this triangle mesh */
	const EPrimitiveTopology& GetPrimitiveTopology() const { return m_PrimitiveTopology; }

	/* Returns a const reference to the world matrix this triangle mesh is defined in */
	const FMatrix4& GetWorldMatrix() const { return m_WorldMatrix; }

	/* Returns const reference to the sample state of this triangle mesh */
	const ESamplerState& GetSampleState() const { return m_SampleState; }

	/* Returns const reference to the cullmode of this triangle mesh */
	const ECullMode& GetCullMode() const { return m_CullMode; }

	/* Returns const reference to the blend state of this triangle mesh */
	const EBlendState& GetBlendState() const { return m_BlendState; }

	/* Returns if mesh is valid to render */
	bool IsValid() const { return m_IsValid; }

	/* Sets sample state to different sample state */
	void SetSampleState(ESamplerState state);

	/* Sets cullmode setting for rasterizer state */
	void SetCullMode(ECullMode cullmode);

	/* Sets blend state (contains: blend or no blend + blend operator) */
	void SetBlendState(EBlendState blendstate);

	/* Sets mesh to (in)valid 
		Only renders meshes who are valid */
	void SetValid(bool v) { m_IsValid = v; }

private:
	/* Common Variables */
	bool m_IsValid;
	unsigned int m_MaterialID;
	EPrimitiveTopology m_PrimitiveTopology;

	bool m_NeedsStateUpdate;
	ESamplerState m_SampleState;
	ECullMode m_CullMode;
	EBlendState m_BlendState;

	float m_RotatedAngle;
	float m_RotateSpeed;
	FMatrix4 m_WorldMatrix;

	/* SRAS Variables */
	std::vector<Vertex_Input> m_VertexBuffer;
	std::vector<uint32_t> m_Indices;

	/* DirectX Variables */
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	uint32_t m_AmountIndices;
	ID3D11InputLayout* m_pVertexLayout;


};