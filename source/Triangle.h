#pragma once
#include "ERGBColor.h"
#include <vector>
#include <functional>

enum class ECullMode : int;
struct KeyBindInfo;
struct HitRecord;
struct BoundingBox;
struct Vertex_Input;
struct Vertex_Output;
class Camera;

class Triangle final
{
public:
	Triangle(Vertex_Input v0, Vertex_Input v1, Vertex_Input v2, unsigned int materialID = 0);
	Triangle(const Triangle& t) = default;
	Triangle(Triangle&& t) = default;
	Triangle& operator=(Triangle&& t) = default;
	Triangle& operator=(const Triangle& t) = default;
	~Triangle() = default;

	/* Updates the triangle */
	void Update(float deltaT);

	/* Determines if a pixel overlaps with the current triangle, stores hit information in the passed hit record */
	bool Hit(const Elite::FPoint2& pixel, HitRecord& hitRecord) const;

	/* Transforms the input vertices accordingly and stores them into the transformed vertices to be used in further calculations */
	void TransformVertices(float width, float height, const Elite::FMatrix4& worldMatrix, Camera* pCamera, const KeyBindInfo& keyBindInfo, bool invertToRHS);

	/* Adjusts the passed bounding box to fit neatly around this triangle */
	void AdjustBoundingBox(BoundingBox& boundingBox, float width, float height) const;

	/* Returns true if the triangle falls inside our view plane, useful test to know whether clipping should be applied or not */
	bool IsInsideFrustum() const { return m_IsInsideFrustum; }

	/* Returns const reference to the vector holding all input vertices */
	const std::vector<Vertex_Input>& GetInputVertices() const { return m_InputVertices; }

	/* Returns const reference to the vector holding all input vertices */
	const std::vector<Vertex_Output>& GetOutputVertices() const { return m_TransformedVertices; }

	/* Sets cullmode of triangle */
	void SetCullMode(ECullMode cullmode) { m_CullMode = cullmode; }

	//========PUBLIC CLIPPING FUNCTIONALITY========
	enum class VertexOrder
	{
		_012,
		_021,
		_102,
		_120,
		_201,
		_210
	};

	void SetTransformedVertices(const Vertex_Output& v0, const Vertex_Output& v1, const Vertex_Output& v2);

	bool IsTriangleClipped() const { return m_IsTriangleClipped; }
	const std::vector<Triangle>& GetClippedTriangles() const { return m_ClippedTriangles; }
	//=============================================

private:
	const unsigned int m_MaterialID;
	std::vector<Vertex_Input> m_InputVertices;
	std::vector<Vertex_Output> m_TransformedVertices;
	Elite::FVector4 m_ViewDirection[3];
	ECullMode m_CullMode;
	bool m_IsInsideFrustum;

	/* Private Functions */
	Elite::FVector3 GetInterpolatedTangent(const std::array<float, 3>& weights, float interpolatedDepthVS) const;
	Elite::FVector3 GetInterpolatedVertexNormal(const std::array<float, 3>& weights, float interpolatedDepthVS) const;
	Elite::FVector3 GetInterpolatedViewDirection(const std::array<float, 3>& weights, float interpolatedDepthVS) const;
	Elite::RGBColor GetInterpolatedColor(const std::array<float, 3>& weights, float interpolatedDepthVS) const;
	Elite::FVector2 GetInterpolatedUV(const std::array<float, 3>& weights, float interpolatedDepthVS) const;

	/* Interpolated z-component (= z-component in screen space) */
	float GetInterpolatedDepthInSS(const std::array<float, 3>& weights) const;

	/* Interpolated w-component (= z-component in view space) */
	float GetInterpolatedDepthInVS(const std::array<float, 3>& weights) const;

	/* If 1 vertex is out of bounds -> apply frustum culling for this triangle */
	void DoSimpleFrustumCulling();

	/* If vertices and attributes are defined in a lhs (for DirectX) -> invert Z-components to work for a rhs */
	void InvertAttributesToRHS(std::vector<Vertex_Input>& vertices);

	//========PRIVATE CLIPPING FUNCTIONALITY========
	bool m_IsTriangleClipped = false;
	std::vector<Triangle> m_ClippedTriangles;

	void DoClippingX();
	bool IsFullyOutsideFrustum() const;
	bool ClippingForComponent_NegativeRange(float c0, float c1, float c2,
		std::function<void(Vertex_Output&, Vertex_Output&, Vertex_Output&, VertexOrder)> clip1,
		std::function<void(Vertex_Output&, Vertex_Output&, Vertex_Output&, VertexOrder)> clip2);

	void SetVerticesOnOrder(Triangle& triangle, const Vertex_Output& v0, const Vertex_Output& v1, const Vertex_Output& v2, VertexOrder order);
	Vertex_Output InterpolatedVertex_Output(const Vertex_Output& v0, const Vertex_Output& v2, float alpha);
	Elite::FPoint4 LerpFPoint4(const Elite::FPoint4& v0, const Elite::FPoint4& v1, float alpha);
	//==============================================
};