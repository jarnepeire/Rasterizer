#pragma once
#include "pch.h"
#include "EMath.h"
#include "Triangle.h"
#include "Structs.h"
#include "Camera.h"
#include "Effect.h"
#include <array>

using namespace Elite;
Triangle::Triangle(Vertex_Input v0, Vertex_Input v1, Vertex_Input v2, unsigned int materialID)
    : m_MaterialID(materialID)
    , m_InputVertices(3)
    , m_TransformedVertices(3)
    , m_ViewDirection()
    , m_CullMode(ECullMode::BackCulling)
    , m_IsInsideFrustum(false)
    , m_IsTriangleClipped(false)
    , m_ClippedTriangles()
{
    m_InputVertices[0] = v0;
    m_InputVertices[1] = v1;
    m_InputVertices[2] = v2;
}

void Triangle::Update(float)
{
}

bool Triangle::Hit(const Elite::FPoint2& pixel, HitRecord& hitRecord) const
{
    //Before anything else, if we have a NoCulling setting we get visual artifacts
    //That's why we're going to determine what side we're looking at from the get-go, and simply cull away the opposite of that
    ECullMode cullmode = m_CullMode;
    if (cullmode == ECullMode::NoCulling)
    {
        //Calculate normal for this triangle (reference: raytracer normal pre-calculation)
        FVector3 v0_v1 = FVector3(m_InputVertices[1].Position - m_InputVertices[0].Position);
        FVector3 v0_v2 = FVector3(m_InputVertices[2].Position - m_InputVertices[0].Position);
        FVector3 normal = Elite::GetNormalized(Elite::Cross(v0_v1, v0_v2));
        float dot = Elite::Dot(normal, hitRecord.ViewDirection);
        if (dot > 0)
        {
            //Means we're looking in the same direction as the normal -> backface normal -> so cull front
            cullmode = ECullMode::FrontCulling;
        }
        else
        {
            //Means we're looking in the same direction as the normal -> frontface normal -> so cull back
            cullmode = ECullMode::BackCulling;
        }
    }

    //Total area needed to decide the weight of a vertex later
    FVector2 a{ m_TransformedVertices[1].Position - m_TransformedVertices[0].Position };
    FVector2 b{ m_TransformedVertices[2].Position - m_TransformedVertices[0].Position };
    float totalArea = Cross(b, a);
    std::array<float, 3> weights;

    //Check first edge
    FVector2 edgeA{ m_TransformedVertices[1].Position - m_TransformedVertices[0].Position };
    FVector2 toPixel{ pixel - FPoint2(m_TransformedVertices[0].Position) };

    //Inside-outside test
    //Cullmode integration (tried doing it the same way using dot products like in raytracer -> results in artifacts)
    //-> Figured it could have to do with the order of how vertices are defined (counterclockwise vs clockwise)
    //-> On discord people explained how in the inside outside test:
    //-> We can check the signs of the cross products along with the cullmode to determine if it should be visible or not
    float signedArea = Cross(toPixel, edgeA);

    //Cullmode check
    if (cullmode == ECullMode::BackCulling && (signedArea < 0)) return false;
    else if (cullmode == ECullMode::FrontCulling && (signedArea > 0)) return false;

    //Set weight of v2
    weights[2] = signedArea / totalArea;

    //Continue with next edge
    FVector2 edgeB{ m_TransformedVertices[2].Position - m_TransformedVertices[1].Position };
    toPixel = pixel - FPoint2(m_TransformedVertices[1].Position);

    //Cullmode check
    signedArea = Cross(toPixel, edgeB);
    if (cullmode == ECullMode::BackCulling && (signedArea < 0)) return false;
    else if (cullmode == ECullMode::FrontCulling && (signedArea > 0)) return false;

    //Set weight of v0
    weights[0] = signedArea / totalArea;

    //Continue with last edge
    FVector2 edgeC{ m_TransformedVertices[0].Position - m_TransformedVertices[2].Position };
    toPixel = pixel - FPoint2(m_TransformedVertices[2].Position);

    //Cullmode check
    signedArea = Cross(toPixel, edgeC);
    if (cullmode == ECullMode::BackCulling && (signedArea < 0)) return false;
    else if (cullmode == ECullMode::FrontCulling && (signedArea > 0)) return false;

    //Set weight of v1
    weights[1] = signedArea / totalArea;

    //Interpolated values
    hitRecord.InterpolatedZ = GetInterpolatedDepthInSS(weights);
    hitRecord.InterpolatedW = GetInterpolatedDepthInVS(weights);
    hitRecord.InterpolatedColor = GetInterpolatedColor(weights, hitRecord.InterpolatedW);
    hitRecord.InterpolatedUV = GetInterpolatedUV(weights, hitRecord.InterpolatedW);
    hitRecord.InterpolatedVertexNormal = GetInterpolatedVertexNormal(weights, hitRecord.InterpolatedW);
    hitRecord.InterpolatedTangent = GetInterpolatedTangent(weights, hitRecord.InterpolatedW);
    hitRecord.ViewDirection = GetInterpolatedViewDirection(weights, hitRecord.InterpolatedW);
    hitRecord.MatID = m_MaterialID;
    return true;
}

void Triangle::TransformVertices(float width, float height, const Elite::FMatrix4& worldMatrix, Camera* pCamera, const KeyBindInfo& keyBindInfo, bool invertToRHS)
{
    //Since the vertices are parsed for DirectX (LHS) we have to revert it to work for our SRAS in RHS
    if (invertToRHS)
        InvertAttributesToRHS(m_InputVertices);

    //Gather camera variables
    const auto& cameraToWorld = pCamera->GetLookAtMatrix();
    const auto& projMatrix = pCamera->GetProjMatrix();

    //Model to WorldSpace -> to ViewSpace -> to Projection Space
    Elite::FMatrix4 worldViewProjMatrix = projMatrix * Elite::Inverse(cameraToWorld) * ((invertToRHS) ? Elite::Inverse(worldMatrix) : worldMatrix);
    for (int i = 0; i < 3; ++i)
    {
        //Copy over attributes
        m_TransformedVertices[i].Color = m_InputVertices[i].Color;
        m_TransformedVertices[i].UV = m_InputVertices[i].UV;

        //Transform normals and tangents in ONLY WORLD SPACE
        m_TransformedVertices[i].VertexNormal = FMatrix3(worldMatrix) * GetNormalized(m_InputVertices[i].VertexNormal);
        m_TransformedVertices[i].Tangent = FMatrix3(worldMatrix) * GetNormalized(m_InputVertices[i].Tangent);

        //Make world position and store view directions
        m_TransformedVertices[i].WorldPosition = worldMatrix * FPoint4(m_InputVertices[i].Position, 1.f);
        m_ViewDirection[i] = GetNormalized(m_TransformedVertices[i].WorldPosition - FPoint4(pCamera->GetPosition(), 1.f));

        //Model in homogeneous Space -> Clipping Space
        m_TransformedVertices[i].Position = worldViewProjMatrix * FPoint4(m_InputVertices[i].Position, 1.f);
    }

    //Simple frustum culling that culls away the triangle as soon as 1 vertex is out of the view plane
    if (keyBindInfo.UseSimpleFrustumCulling)
    {
        DoSimpleFrustumCulling();
        if (!m_IsInsideFrustum)
            return;
    }
    //3D Clipping applied on the triangle (only culls when all vertices are out of view plane)
    else
    {
        //Clear clipped triangles
        m_ClippedTriangles.clear();
        m_IsTriangleClipped = false;

        //For now I'm only testing clipping to the left side of the screen
        DoClippingX();
        if (!m_IsInsideFrustum)
            return;
    }

    //Only applies in case 3D clipping is applied -> will further transform all the newly made triangles
    if (m_IsTriangleClipped)
    {
        for (Triangle& t : m_ClippedTriangles)
        {
            //Further transformation of spaces
            for (int i = 0; i < 3; ++i)
            {
                //Clipping space to NDC space
                //Perspective divide! -> ViewSpace z value is now stored in the w-component
                float viewSpaceZ = t.m_TransformedVertices[i].Position.w;
                t.m_TransformedVertices[i].Position.x /= viewSpaceZ;
                t.m_TransformedVertices[i].Position.y /= viewSpaceZ;
                t.m_TransformedVertices[i].Position.z /= viewSpaceZ;

                //Screen Space
                t.m_TransformedVertices[i].Position.x = (t.m_TransformedVertices[i].Position.x + 1) / 2.f * width;
                t.m_TransformedVertices[i].Position.y = (1 - t.m_TransformedVertices[i].Position.y) / 2.f * height;
            }
        }
    }
    //Transform original triangle vertices in case no clipping was necessary
    else
    {
        for (int i = 0; i < 3; ++i)
        {
            //Clipping space to NDC space
            //Perspective divide! -> ViewSpace z value is now stored in the w-component
            float viewSpaceZ = m_TransformedVertices[i].Position.w;
            m_TransformedVertices[i].Position.x /= viewSpaceZ;
            m_TransformedVertices[i].Position.y /= viewSpaceZ;
            m_TransformedVertices[i].Position.z /= viewSpaceZ;

            //Screen Space
            m_TransformedVertices[i].Position.x = (m_TransformedVertices[i].Position.x + 1) / 2.f * width;
            m_TransformedVertices[i].Position.y = (1 - m_TransformedVertices[i].Position.y) / 2.f * height;
        }
    }
}

void Triangle::AdjustBoundingBox(BoundingBox& boundingBox, float width, float height) const
{
    //Determining topLeft and bottomRight point for our bounding box depending on where the triangle is
    for (size_t i = 0; i < 3; ++i)
    {
        boundingBox.TopLeft.x = std::min(boundingBox.TopLeft.x, m_TransformedVertices[i].Position.x);
        boundingBox.TopLeft.y = std::min(boundingBox.TopLeft.y, m_TransformedVertices[i].Position.y);

        boundingBox.BottomRight.x = std::max(boundingBox.BottomRight.x, m_TransformedVertices[i].Position.x);
        boundingBox.BottomRight.y = std::max(boundingBox.BottomRight.y, m_TransformedVertices[i].Position.y);
    }

    //In case of bounding box going out of screen dimensions, we have to clamp it
    if (boundingBox.TopLeft.x < 0)
        boundingBox.TopLeft.x = 0;

    if (boundingBox.TopLeft.y < 0)
        boundingBox.TopLeft.y = 0;

    if (boundingBox.BottomRight.x > width)
        boundingBox.BottomRight.x = width;

    if (boundingBox.BottomRight.y > height)
        boundingBox.BottomRight.y = height;

    //Flooring/ceiling since we'll be comparing to integer values that represent the pixel later
    boundingBox.TopLeft.x = std::floorf(boundingBox.TopLeft.x);
    boundingBox.TopLeft.y = std::floorf(boundingBox.TopLeft.y);
    boundingBox.BottomRight.x = std::ceilf(boundingBox.BottomRight.x);
    boundingBox.BottomRight.y = std::ceilf(boundingBox.BottomRight.y);
}

Elite::FVector3 Triangle::GetInterpolatedTangent(const std::array<float, 3>& weights, float interpolatedDepthVS) const
{
    Elite::FVector3 interpolatedTangent
    {
        ((m_TransformedVertices[0].Tangent / m_TransformedVertices[0].Position.w) * weights[0] +
         (m_TransformedVertices[1].Tangent / m_TransformedVertices[1].Position.w) * weights[1] +
         (m_TransformedVertices[2].Tangent / m_TransformedVertices[2].Position.w) * weights[2]) * interpolatedDepthVS
    };
    return Elite::GetNormalized(interpolatedTangent);
}

Elite::FVector3 Triangle::GetInterpolatedVertexNormal(const std::array<float, 3>& weights, float interpolatedDepthVS) const
{
    Elite::FVector3 interpolatedVertexNormal
    {
        ((m_TransformedVertices[0].VertexNormal / m_TransformedVertices[0].Position.w) * weights[0] +
         (m_TransformedVertices[1].VertexNormal / m_TransformedVertices[1].Position.w) * weights[1] +
         (m_TransformedVertices[2].VertexNormal / m_TransformedVertices[2].Position.w) * weights[2]) * interpolatedDepthVS
    };
    return Elite::GetNormalized(interpolatedVertexNormal);
}

Elite::FVector3 Triangle::GetInterpolatedViewDirection(const std::array<float, 3>& weights, float interpolatedDepthVS) const
{

    Elite::FVector3 interpolatedCameraToWorldPos
    {
        ((m_ViewDirection[0] / m_TransformedVertices[0].Position.w) * weights[0] +
         (m_ViewDirection[1] / m_TransformedVertices[1].Position.w) * weights[1] +
         (m_ViewDirection[2] / m_TransformedVertices[2].Position.w) * weights[2]) * interpolatedDepthVS
    };

    return GetNormalized(interpolatedCameraToWorldPos);
}
 
Elite::RGBColor Triangle::GetInterpolatedColor(const std::array<float, 3>& weights, float interpolatedDepthVS) const
{
    return RGBColor
    {
        ((m_TransformedVertices[0].Color / m_TransformedVertices[0].Position.w) * weights[0] +
         (m_TransformedVertices[1].Color / m_TransformedVertices[1].Position.w) * weights[1] +
         (m_TransformedVertices[2].Color / m_TransformedVertices[2].Position.w) * weights[2]) * interpolatedDepthVS
    };
}

Elite::FVector2 Triangle::GetInterpolatedUV(const std::array<float, 3>& weights, float interpolatedDepthVS) const
{
    return Elite::FVector2
    {
        ((m_TransformedVertices[0].UV / m_TransformedVertices[0].Position.w) * weights[0] +
         (m_TransformedVertices[1].UV / m_TransformedVertices[1].Position.w) * weights[1] +
         (m_TransformedVertices[2].UV / m_TransformedVertices[2].Position.w) * weights[2]) * interpolatedDepthVS
    };
}
float Triangle::GetInterpolatedDepthInSS(const std::array<float, 3>& weights) const
{
    return float
    {
                                        1.f
                                         /
        ((1.f / m_TransformedVertices[0].Position.z) * weights[0] +
          (1.f / m_TransformedVertices[1].Position.z) * weights[1] +
          (1.f / m_TransformedVertices[2].Position.z) * weights[2])
    };
}
float Triangle::GetInterpolatedDepthInVS(const std::array<float, 3>& weights) const
{
    return float
    {
                                        1.f
                                         /
        ((1.f / m_TransformedVertices[0].Position.w) * weights[0] +
          (1.f / m_TransformedVertices[1].Position.w) * weights[1] +
          (1.f / m_TransformedVertices[2].Position.w) * weights[2])
    };
}

void Triangle::DoSimpleFrustumCulling()
{
    //Vertices here are defined in clipping space (1 space before perspective divide to NDC)
    //If 1 vertex is out of the view plane -> cull 
    for (int i = 0; i < 3; ++i)
    {
        float range = m_TransformedVertices[i].Position.w;
        if (m_TransformedVertices[i].Position.x < -range || m_TransformedVertices[i].Position.x > range
            || m_TransformedVertices[i].Position.y < -range || m_TransformedVertices[i].Position.y > range)
        {
            m_IsInsideFrustum = false;
            return;
        }
    }
    m_IsInsideFrustum = true;
}

void Triangle::InvertAttributesToRHS(std::vector<Vertex_Input>& vertices)
{
    for (int i = 0; i < vertices.size(); ++i)
    {
        vertices[i].Position.z = -vertices[i].Position.z;
        vertices[i].VertexNormal.z = -vertices[i].VertexNormal.z;
        vertices[i].Tangent.z = -vertices[i].Tangent.z;
    }
}



//==============================================================================================
//============================= CLIPPING FUNCTIONALITY STARTS HERE =============================
//==============================================================================================

bool Triangle::IsFullyOutsideFrustum() const
{
    //As of right now, the coordinates are in clipping space [-w, w]
    //Check if all vertices are outsome frustum to conclude we need to fully cull away this triangle

    //X values are over the right of our viewplane check:
    if (m_TransformedVertices[0].Position.x > m_TransformedVertices[0].Position.w &&
        m_TransformedVertices[1].Position.x > m_TransformedVertices[1].Position.w &&
        m_TransformedVertices[2].Position.x > m_TransformedVertices[2].Position.w)
    {
        return true;
    }

    //X values are over the left of our viewplane check:
    if (m_TransformedVertices[0].Position.x < -m_TransformedVertices[0].Position.w &&
        m_TransformedVertices[1].Position.x < -m_TransformedVertices[1].Position.w &&
        m_TransformedVertices[2].Position.x < -m_TransformedVertices[2].Position.w)
    {
        return true;
    }

    //Y values are over the top of our viewplane check:
    if (m_TransformedVertices[0].Position.y > m_TransformedVertices[0].Position.w &&
        m_TransformedVertices[1].Position.y > m_TransformedVertices[1].Position.w &&
        m_TransformedVertices[2].Position.y > m_TransformedVertices[2].Position.w)
    {
        return true;
    }

    //Y values are over the bottom of our viewplane check:
    if (m_TransformedVertices[0].Position.y < -m_TransformedVertices[0].Position.w &&
        m_TransformedVertices[1].Position.y < -m_TransformedVertices[1].Position.w &&
        m_TransformedVertices[2].Position.y < -m_TransformedVertices[2].Position.w)
    {
        return true;
    }

    //Z values are over the top of our viewplane check:
    if (m_TransformedVertices[0].Position.z > m_TransformedVertices[0].Position.w &&
        m_TransformedVertices[1].Position.z > m_TransformedVertices[1].Position.w &&
        m_TransformedVertices[2].Position.z > m_TransformedVertices[2].Position.w)
    {
        return true;
    }

    //Z values are over the bottom of our viewplane check:
    if (m_TransformedVertices[0].Position.z < 0.f &&
        m_TransformedVertices[1].Position.z < 0.f &&
        m_TransformedVertices[2].Position.z < 0.f)
    {
        return true;
    }

    //If all tests passed, our triangle is fully inside frustum 
    return false;
}

void Triangle::DoClippingX()
{
    //Do full culling check
    m_IsInsideFrustum = !IsFullyOutsideFrustum();
    if (!m_IsInsideFrustum) return;

    //If only 1 vertex is on the wrong side (assuming v0 is on the wrong side)
    const auto Clip1_X_Left = [this](Vertex_Output& v0, Vertex_Output& v1, Vertex_Output& v2, VertexOrder order)
    {
        //calculate alphas to get intersection point
       // const float alphaA = ( abs(-v0.Position.w - v0.Position.x) ) / (v1.Position.x - v0.Position.x);
       // const float alphaB = ( abs(-v0.Position.w - v0.Position.x) ) / (v2.Position.x - v0.Position.x);

        const float alphaA = (-v0.Position.w - v0.Position.x) / (v1.Position.x - v0.Position.x);
        const float alphaB = (-v0.Position.w - v0.Position.x) / (v2.Position.x - v0.Position.x);

        //interpolate to get interpolated intersection point
        auto v0a = InterpolatedVertex_Output(v0, v1, alphaA);
        auto v0b = InterpolatedVertex_Output(v0, v2, alphaB);

        //Create 2 new triangle and push to the vector
        Triangle t1{ {}, {}, {} };

        //determine original order
        SetVerticesOnOrder(t1, v0a, v1, v2, order);

        //Now we need to see if that triangle needed clipping
        t1.DoClippingX();
        if (t1.IsTriangleClipped())
        {
            for (const Triangle& clippedTriangle : t1.GetClippedTriangles())
            {
                m_ClippedTriangles.push_back(clippedTriangle);
            }
        }
        else
        {
            m_ClippedTriangles.push_back(t1);
        }

        //Create triangle
        Triangle t2{ {}, {}, {} };

        //determine original order
        SetVerticesOnOrder(t2, v0b, v0a, v2, order);

        //Now we need to see if that triangle needed clipping
        t2.DoClippingX();
        if (t2.IsTriangleClipped())
        {
            for (const Triangle& clippedTriangle : t2.GetClippedTriangles())
            {
                m_ClippedTriangles.push_back(clippedTriangle);
            }
        }
        else
        {
            m_ClippedTriangles.push_back(t2);
        }
    };

    //If two vertices are on the left side (assuming v0 and v1)
    const auto Clip2_X_Left = [this](Vertex_Output& v0, Vertex_Output& v1, Vertex_Output& v2, VertexOrder order)
    {
        //calculate alphas to get intersection point
        const float alphaA = (abs(-v0.Position.w - v0.Position.x)) / (v2.Position.x - v0.Position.x);
        const float alphaB = (abs(-v1.Position.w - v1.Position.x)) / (v2.Position.x - v1.Position.x);

        //interpolate to get interpolated intersection point
        v0 = InterpolatedVertex_Output(v0, v2, alphaA);
        v1 = InterpolatedVertex_Output(v1, v2, alphaB);

        //Create new triangle
        Triangle t{ {}, {}, {} };

        //determine original order
        SetVerticesOnOrder(t, v0, v1, v2, order);

        //Now we need to see if that triangle needed clipping
        t.DoClippingX();
        if (t.IsTriangleClipped())
        {
            //If so return its clipped triangles to add to this main clipped triangle vector
            for (const Triangle& clippedTriangle : t.GetClippedTriangles())
            {
                m_ClippedTriangles.push_back(clippedTriangle);
            }
        }
        else
        {
            m_ClippedTriangles.push_back(t);
        }

    };

    //Clip for X out of bounds to the left
    m_IsTriangleClipped = ClippingForComponent_NegativeRange(m_TransformedVertices[0].Position.x, m_TransformedVertices[1].Position.x, m_TransformedVertices[2].Position.x, Clip1_X_Left, Clip2_X_Left);

}

bool Triangle::ClippingForComponent_NegativeRange(float c0, float c1, float c2, std::function<void(Vertex_Output&, Vertex_Output&, Vertex_Output&, VertexOrder)> clip1, std::function<void(Vertex_Output&, Vertex_Output&, Vertex_Output&, VertexOrder)> clip2)
{
    m_IsInsideFrustum = !IsFullyOutsideFrustum();
    if (!m_IsInsideFrustum) return false;

    //Set to true on start
    bool isClipped = true;

    //plane clipping tests in clipping space (range will always be [-pos.w, pos.w] 
    if (c0 < -m_TransformedVertices[0].Position.w)
    {
        if (c1 < -m_TransformedVertices[1].Position.w)
        {
            clip2(m_TransformedVertices[0], m_TransformedVertices[1], m_TransformedVertices[2], VertexOrder::_012);
        }
        else if (c2 < -m_TransformedVertices[2].Position.w)
        {
            clip2(m_TransformedVertices[0], m_TransformedVertices[2], m_TransformedVertices[1], VertexOrder::_021);
        }
        else
        {
            clip1(m_TransformedVertices[0], m_TransformedVertices[1], m_TransformedVertices[2], VertexOrder::_012);
        }
    }
    else if (c1 < -m_TransformedVertices[1].Position.w)
    {
        if (c2 < -m_TransformedVertices[2].Position.w)
        {
            clip2(m_TransformedVertices[1], m_TransformedVertices[2], m_TransformedVertices[0], VertexOrder::_120);
        }
        else
        {
            clip1(m_TransformedVertices[1], m_TransformedVertices[0], m_TransformedVertices[2], VertexOrder::_102);
        }
    }
    else if (c2 < -m_TransformedVertices[2].Position.w)
    {
        clip1(m_TransformedVertices[2], m_TransformedVertices[0], m_TransformedVertices[1], VertexOrder::_201);
    }
    else
    {
        //No clipping needed
        isClipped = false;
    }

    return isClipped;
}

void Triangle::SetVerticesOnOrder(Triangle& triangle, const Vertex_Output& v0, const Vertex_Output& v1, const Vertex_Output& v2, VertexOrder order)
{
    if (order == VertexOrder::_012)
    {
        triangle.SetTransformedVertices(v0, v1, v2);
    }
    else if (order == VertexOrder::_021)
    {
        triangle.SetTransformedVertices(v0, v2, v1);
    }
    else if (order == VertexOrder::_102)
    {
        triangle.SetTransformedVertices(v1, v0, v2);
    }
    else if (order == VertexOrder::_120)
    {
        triangle.SetTransformedVertices(v2, v0, v1);
    }
    else if (order == VertexOrder::_201)
    {
        triangle.SetTransformedVertices(v1, v2, v0);
    }
    else if (order == VertexOrder::_210)
    {
        triangle.SetTransformedVertices(v2, v1, v0);
    }
}

void Triangle::SetTransformedVertices(const Vertex_Output& v0, const Vertex_Output& v1, const Vertex_Output& v2)
{
    m_TransformedVertices[0] = v0;
    m_TransformedVertices[1] = v1;
    m_TransformedVertices[2] = v2;
}

Vertex_Output Triangle::InterpolatedVertex_Output(const Vertex_Output& v0, const Vertex_Output& v2, float alpha)
{
    Vertex_Output interpolated{};
    interpolated.Position = LerpFPoint4(v0.Position, v2.Position, alpha);
    //interpolated.Color = LerpRGBColor(v0.Color, v2.Color, alpha);
    interpolated.UV = Lerp(v0.UV, v2.UV, alpha);
    interpolated.WorldPosition = LerpFPoint4(v0.WorldPosition, v2.WorldPosition, alpha);
    interpolated.VertexNormal = Lerp(v0.VertexNormal, v2.VertexNormal, alpha);
    interpolated.Tangent = Lerp(v0.Tangent, v2.Tangent, alpha);

    return interpolated;
}

Elite::FPoint4 Triangle::LerpFPoint4(const FPoint4& v0, const FPoint4& v1, float alpha)
{
    Elite::FPoint4 lerpedPoint{};
    lerpedPoint.x = Lerp(v0.x, v1.x, alpha);
    lerpedPoint.y = Lerp(v0.y, v1.y, alpha);
    lerpedPoint.z = Lerp(v0.z, v1.z, alpha);
    lerpedPoint.w = Lerp(v0.w, v1.w, alpha);
    return lerpedPoint;
}