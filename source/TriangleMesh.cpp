#pragma once
#include "pch.h"
#include "TriangleMesh.h"
#include "Effect.h"
#include "Camera.h"
#include "Material.h"
#include "Texture.h"

TriangleMesh::TriangleMesh(unsigned int id, const std::vector<Vertex_Input>& vertices, const std::vector<uint32_t>& indices, EPrimitiveTopology top)
	: m_IsValid(true)
	, m_MaterialID(id)
	, m_PrimitiveTopology(top)
	, m_NeedsStateUpdate(false)
	, m_SampleState(ESamplerState::Point)
	, m_CullMode(ECullMode::BackCulling)
	, m_BlendState(EBlendState::BlendNone)
	, m_RotatedAngle(0.f)
	, m_RotateSpeed(1.f)
	, m_WorldMatrix
	(
		cos(m_RotatedAngle), 0.f, -sin(m_RotatedAngle), 0.f,
		0.f, 1.f, 0.f, 0.f,
		sin(m_RotatedAngle), 0.f, cos(m_RotatedAngle), 0.f,
		0.f, 0.f, 0.f, 1.f
	)
	, m_VertexBuffer(vertices)
	, m_Indices(indices)
	, m_pVertexBuffer(nullptr)
	, m_pIndexBuffer(nullptr)
	, m_AmountIndices((uint32_t)indices.size())
	, m_pVertexLayout(nullptr)
{
}

TriangleMesh::~TriangleMesh()
{
	m_VertexBuffer.clear();
	m_Indices.clear();
	m_pIndexBuffer->Release();
	m_pVertexLayout->Release();
	m_pVertexBuffer->Release();
}

void TriangleMesh::Update(float deltaT)
{
	m_RotatedAngle -= m_RotateSpeed * deltaT;
	m_WorldMatrix = FMatrix4
	(
		cos(m_RotatedAngle), 0.f, -sin(m_RotatedAngle), 0.f,
		0.f, 1.f, 0.f, 0.f,
		sin(m_RotatedAngle), 0.f, cos(m_RotatedAngle), 0.f,
		0.f, 0.f, 0.f, 1.f
	);
}

void TriangleMesh::Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, Camera* pCamera, Material* pMaterial, const std::vector<Elite::FMatrix4>& lightMatrices)
{
	//Set vertex buffer
	UINT stride = sizeof(Vertex_Input);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//Set index buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the input layout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	//Set primitive topology
	pDeviceContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)m_PrimitiveTopology);

	//Update certain effect variables at all times (since matrices are updated every frame)
	Effect* pEffect = pMaterial->GetEffect();
	pEffect->UpdateEffectVariables(m_WorldMatrix, pCamera->GetLookAtMatrix(), pCamera->GetProjMatrix(), pMaterial, lightMatrices);

	//If key input for swapping states is pressed, then update them and swap them
	if (m_NeedsStateUpdate)
	{
		pEffect->UpdateStateVariables(pDevice, pDeviceContext, m_SampleState, m_CullMode, m_BlendState);
		m_NeedsStateUpdate = false;
	}

	//Render triangle
	D3DX11_TECHNIQUE_DESC techDesc;
	pEffect->GetTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_AmountIndices, 0, 0);
	}
}

void TriangleMesh::SetSampleState(ESamplerState state)
{
	m_SampleState = state;
	m_NeedsStateUpdate = true;
}

void TriangleMesh::SetCullMode(ECullMode cullmode)
{
	m_CullMode = cullmode;
	m_NeedsStateUpdate = true;
}

void TriangleMesh::SetBlendState(EBlendState blendstate)
{
	m_BlendState = blendstate;
	m_NeedsStateUpdate = true;
}

void TriangleMesh::Initialize(ID3D11Device* pDevice, Effect* pEffect)
{
	//Create Vertex Layout
	HRESULT result = S_OK;
	D3D11_INPUT_ELEMENT_DESC vertexDesc[NUM_ELEMENTS]{};

	//Position (3 floats, 12 bytes => so start at 12 bytes for next element)
	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Color (3 floats, 12 bytes again => so start at 24 bytes for next element)
	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Color (2 floats, 8 bytes => so start at 32 bytes for next element)
	vertexDesc[2].SemanticName = "TEXCOORD";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 24;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Color (3 floats, 12 bytes => so start at 44 bytes for next element)
	vertexDesc[3].SemanticName = "NORMAL";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Color (3 floats, 12 bytes => so start at 56 bytes for next element)
	vertexDesc[4].SemanticName = "TANGENT";
	vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[4].AlignedByteOffset = 44;
	vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


	//Create vertex buffer
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex_Input) * (uint32_t)m_VertexBuffer.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData{ 0 };
	initData.pSysMem = m_VertexBuffer.data();

	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return;

	//Create the input layout
	D3DX11_PASS_DESC passDesc;
	pEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
	result = pDevice->CreateInputLayout(
		vertexDesc,
		NUM_ELEMENTS,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pVertexLayout);

	if (FAILED(result))
		return;

	//Create index buffer
	m_AmountIndices = (uint32_t)m_Indices.size();
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_AmountIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = m_Indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
		return;
}