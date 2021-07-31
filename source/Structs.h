#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include "Triangle.h"

using namespace Elite;

struct HitRecord
{
	unsigned int MatID = {};
	float InterpolatedZ = {};
	float InterpolatedW = {};
	RGBColor InterpolatedColor = {};
	FVector2 InterpolatedUV = {};
	FVector3 InterpolatedVertexNormal = {};
	FVector3 InterpolatedTangent = {};
	FVector3 ViewDirection = {};
};


//Number of attributes a vertex struct holds
static const uint32_t NUM_ELEMENTS = 5;

struct Vertex_Input
{
	Vertex_Input() {}
	Vertex_Input(FPoint3 pos, FVector2 uv, FVector3 normal, FVector3 tangent = {}, RGBColor col = {})
	{
		Position = pos;
		UV = uv;
		Color = col;
		VertexNormal = normal;
		Tangent = tangent;
	}

	inline bool operator==(const Vertex_Input& v) const
	{
		return (Position == v.Position && VertexNormal == v.VertexNormal && UV == v.UV);
	}

	FPoint3 Position = {};
	RGBColor Color = {};
	FVector2 UV = {};
	FVector3 VertexNormal = {};
	FVector3 Tangent = {};

};

struct Vertex_Output
{
	Vertex_Output() {}
	Vertex_Output(FPoint4 pos, FVector2 uv, FVector3 vertexNormal, FVector3 tangent, RGBColor col = RGBColor(1.f, 1.f, 1.f))
	{
		Position = pos; VertexNormal = vertexNormal; Color = col; UV = uv; Tangent = tangent;
	}

	Vertex_Output operator=(const Vertex_Output& v1)
	{
		this->Position = v1.Position;
		this->Color = v1.Color;
		this->WorldPosition = v1.WorldPosition;
		this->VertexNormal = v1.VertexNormal;
		this->UV = v1.UV;
		this->Tangent = v1.Tangent;
		return *this;
	}

	inline bool operator==(const Vertex_Output& v1) const
	{
		return (Position == v1.Position && VertexNormal == v1.VertexNormal && UV == v1.UV);
	}

	FPoint4 Position = {};
	FPoint4 WorldPosition = {};
	FVector3 VertexNormal = {};
	RGBColor Color = {};
	FVector2 UV = {};
	FVector3 Tangent = {};
};

struct BoundingBox
{
	BoundingBox(FPoint2 topLeft, FPoint2 bottomRight) { TopLeft = topLeft; BottomRight = bottomRight; }
	BoundingBox() {};

	FPoint2 TopLeft = { FLT_MAX, FLT_MAX };
	FPoint2 BottomRight = { 0.f, 0.f };
};

/* Don't forget to update the _NR_OF_OPTIONS when adding new options */
enum class ImageRenderInfo : unsigned int
{
	All = 0,
	OnlyIrradiance = 1,
	OnlyDiffuse = 2,
	_NR_OF_OPTIONS = 3
};

struct KeyBindInfo
{
	bool UseDepthBufferAsColor = false;
	bool UseMaterial = true;
	bool UseSimpleFrustumCulling = true;
	ImageRenderInfo ImageRenderInfo = ImageRenderInfo::All;
};