#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/OBB3.hpp"

constexpr float PI = 3.14159265358979323846f;

void TransfromVertexArrayXY3D(int numVerts, Vertex_PCU* verts, float scaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY)
{
	for (int vertIndex = 0; vertIndex < numVerts; ++vertIndex)
	{
		Vec3& pos = verts[vertIndex].m_position;
		TransformPositionXY3D(pos, scaleXY, rotationDegreesAboutZ, translationXY);
	}
}

void AddVertsForCapsule2D(std::vector<Vertex_PCU>& verts, Capsule2 const& capsule, Rgba8 const& color)
{
	AddVertsForCapsule2D(verts, capsule.m_start, capsule.m_end, capsule.m_radius, color);
}

void AddVertsForCapsule2D(std::vector<Vertex_PCU>& verts, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color)
{
	Vec2 displacement = boneEnd - boneStart;
	Vec2 direction = displacement.GetNormalized();
	Vec2 tangent = direction.GetRotated90Degrees();

	Vec2 offset = tangent * radius;
	Vertex_PCU p0 = Vertex_PCU(Vec3(boneStart.x + offset.x, boneStart.y + offset.y, 0.f), color, Vec2(0.f, 0.f));
	Vertex_PCU p1 = Vertex_PCU(Vec3(boneStart.x - offset.x, boneStart.y - offset.y, 0.f), color, Vec2(0.f, 0.f));
	Vertex_PCU p2 = Vertex_PCU(Vec3(boneEnd.x - offset.x, boneEnd.y - offset.y, 0.f), color, Vec2(0.f, 0.f));
	Vertex_PCU p3 = Vertex_PCU(Vec3(boneEnd.x + offset.x, boneEnd.y + offset.y, 0.f), color, Vec2(0.f, 0.f));

	verts.push_back(p0);
	verts.push_back(p1);
	verts.push_back(p2);

	verts.push_back(p0);
	verts.push_back(p2);
	verts.push_back(p3);

	const int numSlices = 20;

	for (int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
	{
		float fraction0 = (float)sliceIndex / numSlices;
		float fraction1 = (float)(sliceIndex + 1) / numSlices;

		Vec2 capStart0 = boneStart + (tangent * radius * cosf(fraction0 * 3.1415926f)) - (direction * radius * sinf(fraction0 * 3.1415926f));
		Vec2 capStart1 = boneStart + (tangent * radius * cosf(fraction1 * 3.1415926f)) - (direction * radius * sinf(fraction1 * 3.1415926f));
		verts.push_back(Vertex_PCU(Vec3(boneStart.x, boneStart.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(capStart0.x, capStart0.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(capStart1.x, capStart1.y, 0.f), color, Vec2(0.f, 0.f)));

		Vec2 capEnd0 = boneEnd + (tangent * radius * cosf(fraction0 * 3.1415926f)) + (direction * radius * sinf(fraction0 * 3.1415926f));
		Vec2 capEnd1 = boneEnd + (tangent * radius * cosf(fraction1 * 3.1415926f)) + (direction * radius * sinf(fraction1 * 3.1415926f));
		verts.push_back(Vertex_PCU(Vec3(boneEnd.x, boneEnd.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(capEnd0.x, capEnd0.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(capEnd1.x, capEnd1.y, 0.f), color, Vec2(0.f, 0.f)));
	}
}

void AddVertsForDisc2D(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, Rgba8 const& color)
{
	constexpr int numSides = 32;
	constexpr float degreesPerSide = 360.f / numSides;

	Vec2 prevPoint = Vec2(radius, 0.f) + center;
	Vec2 uvCenter = Vec2(0.5f, 0.5f);

	for (int sideIndex = 1; sideIndex <= numSides; ++sideIndex) {
		float angleDegrees = degreesPerSide * sideIndex;
		Vec2 point = Vec2::MakeFromPolarDegrees(angleDegrees) * radius + center;
		Vec2 uvPoint = Vec2::MakeFromPolarDegrees(angleDegrees) * 0.5f + uvCenter;

		verts.push_back(Vertex_PCU(Vec3(center.x, center.y, 0.f), color, uvCenter));
		verts.push_back(Vertex_PCU(Vec3(prevPoint.x, prevPoint.y, 0.f), color, uvPoint));
		verts.push_back(Vertex_PCU(Vec3(point.x, point.y, 0.f), color, uvPoint));

		prevPoint = point;
	}
}

void AddVertsForAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color)
{
	constexpr int NUM_TRIS = 2;
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);

	Vec3 BL = Vec3(bounds.m_mins.x, bounds.m_mins.y, 0.f);
	Vec3 BR = Vec3(bounds.m_maxs.x, bounds.m_mins.y, 0.f);
	Vec3 TR = Vec3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f);
	Vec3 TL = Vec3(bounds.m_mins.x, bounds.m_maxs.y, 0.f);

	verts.push_back(Vertex_PCU(BL, color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(BR, color, Vec2(1.f, 0.f)));
	verts.push_back(Vertex_PCU(TR, color, Vec2(1.f, 1.f)));

	verts.push_back(Vertex_PCU(BL, color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(TR, color, Vec2(1.f, 1.f)));
	verts.push_back(Vertex_PCU(TL, color, Vec2(0.f, 1.f)));
}

void AddVertsForAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, Vec2 uvAtMins, Vec2 uvAtMaxs)
{
	constexpr int NUM_TRIS = 2;
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);

	Vec3 BL = Vec3(bounds.m_mins.x, bounds.m_mins.y, 0.f); 
	Vec3 BR = Vec3(bounds.m_maxs.x, bounds.m_mins.y, 0.f); 
	Vec3 TR = Vec3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f);
	Vec3 TL = Vec3(bounds.m_mins.x, bounds.m_maxs.y, 0.f); 

	verts.push_back(Vertex_PCU(BL, color, Vec2(uvAtMins.x, uvAtMins.y)));
	verts.push_back(Vertex_PCU(BR, color, Vec2(uvAtMaxs.x, uvAtMins.y)));
	verts.push_back(Vertex_PCU(TR, color, Vec2(uvAtMaxs.x, uvAtMaxs.y)));

	verts.push_back(Vertex_PCU(BL, color, Vec2(uvAtMins.x, uvAtMins.y)));
	verts.push_back(Vertex_PCU(TR, color, Vec2(uvAtMaxs.x, uvAtMaxs.y)));
	verts.push_back(Vertex_PCU(TL, color, Vec2(uvAtMins.x, uvAtMaxs.y)));
}

void AddVertsForOBB2D(std::vector<Vertex_PCU>& verts, OBB2 const& box, Rgba8 const& color)
{
	Vec2 right = box.m_iBasisNormal * box.m_halfDimensions.x;
	Vec2 up = box.m_iBasisNormal.GetRotated90Degrees() * box.m_halfDimensions.y;

	Vec2 topRight = box.m_center + right + up;
	Vec2 topLeft = box.m_center - right + up;
	Vec2 bottomLeft = box.m_center - right - up;
	Vec2 bottomRight = box.m_center + right - up;

	verts.push_back(Vertex_PCU(Vec3(topRight.x, topRight.y, 0.f), color, Vec2(1.f, 1.f)));
	verts.push_back(Vertex_PCU(Vec3(topLeft.x, topLeft.y, 0.f), color, Vec2(0.f, 1.f)));
	verts.push_back(Vertex_PCU(Vec3(bottomLeft.x, bottomLeft.y, 0.f), color, Vec2(0.f, 0.f)));

	verts.push_back(Vertex_PCU(Vec3(bottomLeft.x, bottomLeft.y, 0.f), color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(Vec3(bottomRight.x, bottomRight.y, 0.f), color, Vec2(1.f, 0.f)));
	verts.push_back(Vertex_PCU(Vec3(topRight.x, topRight.y, 0.f), color, Vec2(1.f, 1.f)));
}

void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color)
{
	Vec2 forward = (end - start).GetNormalized();
	Vec2 left = Vec2(-forward.y, forward.x);

	Vec2 offset = left * (thickness / 2.0f);

	Vec2 p0 = start - offset;
	Vec2 p1 = start + offset;
	Vec2 p2 = end + offset;
	Vec2 p3 = end - offset;

	verts.push_back(Vertex_PCU(Vec3(p0.x, p0.y, 0.f), color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(Vec3(p1.x, p1.y, 0.f), color, Vec2(1.f, 0.f)));
	verts.push_back(Vertex_PCU(Vec3(p2.x, p2.y, 0.f), color, Vec2(1.f, 1.f)));

	verts.push_back(Vertex_PCU(Vec3(p0.x, p0.y, 0.f), color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(Vec3(p2.x, p2.y, 0.f), color, Vec2(1.f, 1.f)));
	verts.push_back(Vertex_PCU(Vec3(p3.x, p3.y, 0.f), color, Vec2(0.f, 1.f)));
}

void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, LineSegment2 const& lineSegment, float thickness, Rgba8 const& color)
{
	AddVertsForLineSegment2D(verts, lineSegment.m_start, lineSegment.m_end, thickness, color);
}

void AddVertsForArrow2D(std::vector<Vertex_PCU>& verts, Vec2 tailPos, Vec2 tipPos, float arrowSize, float lineThickness, Rgba8 color)
{
	Vec2 forward = tipPos - tailPos;
	forward.Normalize();

	Vec2 left = Vec2(-forward.y, forward.x);

	Vec2 lineStartLeft = tailPos + (left * (lineThickness * 0.5f));
	Vec2 lineStartRight = tailPos - (left * (lineThickness * 0.5f));
	Vec2 lineEndLeft = tipPos + (left * (lineThickness * 0.5f)) - (forward * arrowSize);
	Vec2 lineEndRight = tipPos - (left * (lineThickness * 0.5f)) - (forward * arrowSize);

	verts.push_back(Vertex_PCU(Vec3(lineStartLeft.x, lineStartLeft.y, 0.f), color, Vec2::ZERO));
	verts.push_back(Vertex_PCU(Vec3(lineStartRight.x, lineStartRight.y, 0.f), color, Vec2::ZERO));
	verts.push_back(Vertex_PCU(Vec3(lineEndRight.x, lineEndRight.y, 0.f), color, Vec2::ZERO));

	verts.push_back(Vertex_PCU(Vec3(lineStartLeft.x, lineStartLeft.y, 0.f), color, Vec2::ZERO));
	verts.push_back(Vertex_PCU(Vec3(lineEndRight.x, lineEndRight.y, 0.f), color, Vec2::ZERO));
	verts.push_back(Vertex_PCU(Vec3(lineEndLeft.x, lineEndLeft.y, 0.f), color, Vec2::ZERO));

	// Calculate the three vertices of the tip
	Vec2 tipLeft = tipPos - (forward * arrowSize) + (left * arrowSize);
	Vec2 tipRight = tipPos - (forward * arrowSize) - (left * arrowSize);

	// Add vertices for the tip (the head of the arrow)
	verts.push_back(Vertex_PCU(Vec3(tipPos.x, tipPos.y, 0.f), color, Vec2::ZERO));
	verts.push_back(Vertex_PCU(Vec3(tipLeft.x, tipLeft.y, 0.f), color, Vec2::ZERO));
	verts.push_back(Vertex_PCU(Vec3(tipRight.x, tipRight.y, 0.f), color, Vec2::ZERO));
}

void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft, const Rgba8& color, const AABB2& UVs)
{
		verts.push_back(Vertex_PCU(bottomLeft, color, Vec2(UVs.m_mins.x, UVs.m_mins.y))); // Bottom left
		verts.push_back(Vertex_PCU(bottomRight, color, Vec2(UVs.m_maxs.x, UVs.m_mins.y))); // Bottom right
		verts.push_back(Vertex_PCU(topRight, color, Vec2(UVs.m_maxs.x, UVs.m_maxs.y))); // Top right
	
		verts.push_back(Vertex_PCU(bottomLeft, color, Vec2(UVs.m_mins.x, UVs.m_mins.y))); // Bottom left
		verts.push_back(Vertex_PCU(topRight, color, Vec2(UVs.m_maxs.x, UVs.m_maxs.y))); // Top right
		verts.push_back(Vertex_PCU(topLeft, color, Vec2(UVs.m_mins.x, UVs.m_maxs.y))); // Top left
}

void AddVertsForQuad3DInverse(std::vector<Vertex_PCU>& verts, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft, const Rgba8& color, const AABB2& UVs)
{
	verts.push_back(Vertex_PCU(topRight, color, Vec2(UVs.m_maxs.x, UVs.m_maxs.y))); // Top right
	verts.push_back(Vertex_PCU(bottomRight, color, Vec2(UVs.m_maxs.x, UVs.m_mins.y))); // Bottom right
	verts.push_back(Vertex_PCU(bottomLeft, color, Vec2(UVs.m_mins.x, UVs.m_mins.y))); // Bottom left
	
	verts.push_back(Vertex_PCU(topLeft, color, Vec2(UVs.m_mins.x, UVs.m_maxs.y))); // Top left
	verts.push_back(Vertex_PCU(topRight, color, Vec2(UVs.m_maxs.x, UVs.m_maxs.y))); // Top right
	verts.push_back(Vertex_PCU(bottomLeft, color, Vec2(UVs.m_mins.x, UVs.m_mins.y))); // Bottom left
}

void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	Vec3 edge1 = bottomRight - bottomLeft;
	Vec3 edge2 = topRight - bottomRight;
	Vec3 edge3 = topLeft - topRight;
	Vec3 edge4 = bottomLeft - topLeft;
	Vec3 normal1 = CrossProduct3D(edge1, -1.f * edge4).GetNormalized();
	Vec3 normal2 = CrossProduct3D(edge2, -1.f * edge1).GetNormalized();
	Vec3 normal3 = CrossProduct3D(edge3, -1.f * edge2).GetNormalized();
	Vec3 normal4 = CrossProduct3D(edge4, -1.f * edge3).GetNormalized();

	unsigned int currentVertexCount = static_cast<unsigned int>(vertexes.size());

	vertexes.push_back(Vertex_PCUTBN(bottomLeft, color, UVs.m_mins, Vec3(0, 0, 0), Vec3(0, 0, 0), normal1));
	vertexes.push_back(Vertex_PCUTBN(bottomRight, color, Vec2(UVs.m_maxs.x, UVs.m_mins.y), Vec3(0, 0, 0), Vec3(0, 0, 0), normal2));
	vertexes.push_back(Vertex_PCUTBN(topRight, color, UVs.m_maxs, Vec3(0, 0, 0), Vec3(0, 0, 0), normal3));
	vertexes.push_back(Vertex_PCUTBN(topLeft, color, Vec2(UVs.m_mins.x, UVs.m_maxs.y), Vec3(0, 0, 0), Vec3(0, 0, 0), normal4));

	indexes.push_back(currentVertexCount + 0); // bottomLeft
	indexes.push_back(currentVertexCount + 1); // bottomRight
	indexes.push_back(currentVertexCount + 2); // topRight

	indexes.push_back(currentVertexCount + 0); // bottomLeft
	indexes.push_back(currentVertexCount + 2); // topRight
	indexes.push_back(currentVertexCount + 3); // topLeft
}

void AddVertsForOBB3D(std::vector<Vertex_PCU>& verts, OBB3 obb, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	Vec3 iBasis = obb.iBasis.GetNormalized() * obb.halfDimensions.x;
	Vec3 jBasis = obb.jBasis.GetNormalized() * obb.halfDimensions.y;
	Vec3 kBasis = CrossProduct3D(obb.iBasis, obb.jBasis).GetNormalized() * obb.halfDimensions.z;

	Vec3 corners[8];

	// Calculate corners
	corners[0] = obb.center + iBasis + jBasis + kBasis; // topRightBack
	corners[1] = obb.center - iBasis + jBasis + kBasis; // topLeftBack
	corners[2] = obb.center - iBasis - jBasis + kBasis; // bottomLeftBack
	corners[3] = obb.center + iBasis - jBasis + kBasis; // bottomRightBack
	corners[4] = obb.center + iBasis + jBasis - kBasis; // topRightFront
	corners[5] = obb.center - iBasis + jBasis - kBasis; // topLeftFront
	corners[6] = obb.center - iBasis - jBasis - kBasis; // bottomLeftFront
	corners[7] = obb.center + iBasis - jBasis - kBasis; // bottomRightFront

	AddVertsForQuad3D(verts, corners[2], corners[3], corners[0], corners[1], color, UVs);
	AddVertsForQuad3D(verts, corners[5], corners[4], corners[7], corners[6], color, UVs);
	AddVertsForQuad3D(verts, corners[4], corners[5], corners[1], corners[0], color, UVs);
	AddVertsForQuad3D(verts, corners[6], corners[7], corners[3], corners[2], color, UVs);
	AddVertsForQuad3D(verts, corners[7], corners[4], corners[0], corners[3], color, UVs);
	AddVertsForQuad3D(verts, corners[5], corners[6], corners[2], corners[1], color, UVs);
}

void AddVertsForRoundedQuad3D(std::vector<Vertex_PCUTBN>& vertexes, const Vec3& topLeft, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	Vec3 midPointOfTop = (topLeft + topRight) * 0.5f;
	Vec3 midPointOfBott = (bottomLeft + bottomRight) * 0.5f;
	Vec2 UVTop = Vec2((UVs.m_maxs.x - UVs.m_mins.x) * 0.5f + UVs.m_mins.x, UVs.m_maxs.y);
	Vec2 UVBott = Vec2((UVs.m_maxs.x - UVs.m_mins.x) * 0.5f + UVs.m_mins.x, UVs.m_mins.y);
	Vec3 uDirection = (bottomRight - bottomLeft).GetNormalized();
	Vec3 vDirection = (topLeft - bottomLeft).GetNormalized();
	Vec3 normal = CrossProduct3D(uDirection, vDirection).GetNormalized();

	vertexes.push_back(Vertex_PCUTBN(bottomLeft, color, Vec2(UVs.m_mins.x, UVs.m_mins.y), Vec3(0, 0, 0), Vec3(0, 0, 0), -1.f * uDirection));
	vertexes.push_back(Vertex_PCUTBN(midPointOfBott, color, UVBott, Vec3(0, 0, 0), Vec3(0, 0, 0), normal));
	vertexes.push_back(Vertex_PCUTBN(midPointOfTop, color, UVTop, Vec3(0, 0, 0), Vec3(0, 0, 0), normal));

	vertexes.push_back(Vertex_PCUTBN(midPointOfTop, color, UVTop, Vec3(0, 0, 0), Vec3(0, 0, 0), normal));
	vertexes.push_back(Vertex_PCUTBN(topLeft, color, Vec2(UVs.m_mins.x, UVs.m_maxs.y), Vec3(0, 0, 0), Vec3(0, 0, 0), -1.f * uDirection));
	vertexes.push_back(Vertex_PCUTBN(bottomLeft, color, Vec2(UVs.m_mins.x, UVs.m_mins.y), Vec3(0, 0, 0), Vec3(0, 0, 0), -1.f * uDirection));

	vertexes.push_back(Vertex_PCUTBN(midPointOfBott, color, UVBott, Vec3(0, 0, 0), Vec3(0, 0, 0), normal));
	vertexes.push_back(Vertex_PCUTBN(bottomRight, color, Vec2(UVs.m_maxs.x, UVs.m_mins.y), Vec3(0, 0, 0), Vec3(0, 0, 0), uDirection));
	vertexes.push_back(Vertex_PCUTBN(topRight, color, Vec2(UVs.m_maxs.x, UVs.m_maxs.y), Vec3(0, 0, 0), Vec3(0, 0, 0), uDirection));


	vertexes.push_back(Vertex_PCUTBN(topRight, color, Vec2(UVs.m_maxs.x, UVs.m_maxs.y), Vec3(0, 0, 0), Vec3(0, 0, 0), uDirection));
	vertexes.push_back(Vertex_PCUTBN(midPointOfTop, color, UVTop, Vec3(0, 0, 0), Vec3(0, 0, 0), normal));
	vertexes.push_back(Vertex_PCUTBN(midPointOfBott, color, UVBott, Vec3(0, 0, 0), Vec3(0, 0, 0), normal));
}

void AddVertsForLineSegment3D(std::vector<Vertex_PCU>& mainVertexList, Vec3 const& start, Vec3 const& end, float thickness, Rgba8 const& color)
{
	float halfThickness = 0.5f * thickness;
	Vec3 idirection = (end - start).GetNormalized();
	Vec3 jdirection = CrossProduct3D(idirection, Vec3(0.0f, 0.0f, 1.0f));
	if (jdirection.GetLengthSquared() < 0.01f)
	{
		jdirection = CrossProduct3D(idirection, Vec3(0.0f, 1.0f, 0.0f));
	}
	jdirection = jdirection.GetNormalized();
	Vec3 kdirection = CrossProduct3D(idirection, jdirection);
	kdirection = kdirection.GetNormalized();

	Vec3 frontTopLeft = start - halfThickness * idirection - halfThickness * jdirection + halfThickness * kdirection;
	Vec3 frontTopRight = start - halfThickness * idirection + halfThickness * jdirection + halfThickness * kdirection;
	Vec3 frontBottomLeft = end + halfThickness * idirection - halfThickness * jdirection + halfThickness * kdirection;
	Vec3 frontBottomRight = end + halfThickness * idirection + halfThickness * jdirection + halfThickness * kdirection;
	Vec3 BackTopLeft = start - halfThickness * idirection - halfThickness * jdirection - halfThickness * kdirection;
	Vec3 BackTopRight = start - halfThickness * idirection + halfThickness * jdirection - halfThickness * kdirection;
	Vec3 BackBottomLeft = end + halfThickness * idirection - halfThickness * jdirection - halfThickness * kdirection;
	Vec3 BackBottomRight = end + halfThickness * idirection + halfThickness * jdirection - halfThickness * kdirection;

	AddVertsForQuad3D(mainVertexList, BackBottomRight, BackBottomLeft, BackTopLeft, BackTopRight, color, AABB2::ZERO_TO_ONE);
	AddVertsForQuad3D(mainVertexList, frontBottomLeft, frontBottomRight, frontTopRight, frontTopLeft, color, AABB2::ZERO_TO_ONE);
	AddVertsForQuad3D(mainVertexList, BackBottomLeft, frontBottomLeft, frontTopLeft, BackTopLeft, color, AABB2::ZERO_TO_ONE);

	AddVertsForQuad3D(mainVertexList, frontBottomRight, BackBottomRight, BackTopRight, frontTopRight, color, AABB2::ZERO_TO_ONE);
	AddVertsForQuad3D(mainVertexList, BackTopRight, BackTopLeft, frontTopLeft, frontTopRight, color, AABB2::ZERO_TO_ONE);
	AddVertsForQuad3D(mainVertexList, frontBottomRight, frontBottomLeft, BackBottomLeft, BackBottomRight, color, AABB2::ZERO_TO_ONE);
}

void AddVertsForLineSegment3D(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes, Vec3 const& start, Vec3 const& end, float thickness, Rgba8 const& color)
{
	float halfThickness = 0.5f * thickness;
	Vec3 idirection = (end - start).GetNormalized();
	Vec3 jdirection = CrossProduct3D(idirection, Vec3(0.0f, 0.0f, 1.0f));
	if (jdirection.GetLengthSquared() < 0.01f)
	{
		jdirection = CrossProduct3D(idirection, Vec3(0.0f, 1.0f, 0.0f));
	}
	jdirection = jdirection.GetNormalized();
	Vec3 kdirection = CrossProduct3D(idirection, jdirection);
	kdirection = kdirection.GetNormalized();

	Vec3 frontTopLeft = start - halfThickness * idirection - halfThickness * jdirection + halfThickness * kdirection;
	Vec3 frontTopRight = start - halfThickness * idirection + halfThickness * jdirection + halfThickness * kdirection;
	Vec3 frontBottomLeft = end + halfThickness * idirection - halfThickness * jdirection + halfThickness * kdirection;
	Vec3 frontBottomRight = end + halfThickness * idirection + halfThickness * jdirection + halfThickness * kdirection;
	Vec3 BackTopLeft = start - halfThickness * idirection - halfThickness * jdirection - halfThickness * kdirection;
	Vec3 BackTopRight = start - halfThickness * idirection + halfThickness * jdirection - halfThickness * kdirection;
	Vec3 BackBottomLeft = end + halfThickness * idirection - halfThickness * jdirection - halfThickness * kdirection;
	Vec3 BackBottomRight = end + halfThickness * idirection + halfThickness * jdirection - halfThickness * kdirection;

	AddVertsForQuad3D(vertexes, indexes, BackBottomRight, BackBottomLeft, BackTopLeft, BackTopRight, color, AABB2::ZERO_TO_ONE);
	AddVertsForQuad3D(vertexes, indexes, frontBottomLeft, frontBottomRight, frontTopRight, frontTopLeft, color, AABB2::ZERO_TO_ONE);
	AddVertsForQuad3D(vertexes, indexes, BackBottomLeft, frontBottomLeft, frontTopLeft, BackTopLeft, color, AABB2::ZERO_TO_ONE);

	AddVertsForQuad3D(vertexes, indexes, frontBottomRight, BackBottomRight, BackTopRight, frontTopRight, color, AABB2::ZERO_TO_ONE);
	AddVertsForQuad3D(vertexes, indexes, BackTopRight, BackTopLeft, frontTopLeft, frontTopRight, color, AABB2::ZERO_TO_ONE);
	AddVertsForQuad3D(vertexes, indexes, frontBottomRight, frontBottomLeft, BackBottomLeft, BackBottomRight, color, AABB2::ZERO_TO_ONE);
}

void AddVertsForAABB3D(std::vector<Vertex_PCU>& verts, const AABB3& bounds, const Rgba8& color /*= Rgba::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	Vec3 topRightBack(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z);
	Vec3 topLeftBack(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);
	Vec3 bottomLeftBack(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z);
	Vec3 bottomRightBack(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z);
	Vec3 topRightFront(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z);
	Vec3 topLeftFront(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);
	Vec3 bottomLeftFront(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z);
	Vec3 bottomRightFront(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);

	AddVertsForQuad3D(verts, bottomLeftBack, bottomRightBack, topRightBack, topLeftBack, color, UVs);//z
	AddVertsForQuad3D(verts, topLeftFront, topRightFront, bottomRightFront, bottomLeftFront,color, UVs);//-z
	AddVertsForQuad3D(verts, topRightFront, topLeftFront, topLeftBack, topRightBack, color, UVs);//y
	AddVertsForQuad3D(verts, bottomLeftFront, bottomRightFront, bottomRightBack, bottomLeftBack, color, UVs);//-y
	AddVertsForQuad3D(verts, bottomRightFront, topRightFront, topRightBack, bottomRightBack, color, UVs);//x
	AddVertsForQuad3D(verts, topLeftFront, bottomLeftFront, bottomLeftBack, topLeftBack, color, UVs);//-x
}

void AddVertsForSphere3D(std::vector<Vertex_PCU>& verts, const Vec3& center, float radius, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/, int numLatitudesSlices /*= 8*/)
{
	UNUSED(UVs);
	for (int latIndex = 0; latIndex < numLatitudesSlices; ++latIndex) {
		for (int longIndex = 0; longIndex < numLatitudesSlices; ++longIndex) {
			float latStart = PI * latIndex / numLatitudesSlices - PI / 2;
			float latEnd = PI * (latIndex + 1) / numLatitudesSlices - PI / 2;
			float longStart = 2 * PI * longIndex / numLatitudesSlices;
			float longEnd = 2 * PI * (longIndex + 1) / numLatitudesSlices;

			Vec3 v1 = Vec3::MakeFromPolarRadians(latStart, longStart, radius) + center;
			Vec3 v2 = Vec3::MakeFromPolarRadians(latStart, longEnd, radius) + center;
			Vec3 v3 = Vec3::MakeFromPolarRadians(latEnd, longStart, radius) + center;
			Vec3 v4 = Vec3::MakeFromPolarRadians(latEnd, longEnd, radius) + center;

			Vec2 uv1 = Vec2(static_cast<float>(longIndex) / numLatitudesSlices, static_cast<float>(latIndex) / numLatitudesSlices);
			Vec2 uv2 = Vec2(static_cast<float>(longIndex + 1) / numLatitudesSlices, static_cast<float>(latIndex) / numLatitudesSlices);
			Vec2 uv3 = Vec2(static_cast<float>(longIndex) / numLatitudesSlices, static_cast<float>(latIndex + 1) / numLatitudesSlices);
			Vec2 uv4 = Vec2(static_cast<float>(longIndex + 1) / numLatitudesSlices, static_cast<float>(latIndex + 1) / numLatitudesSlices);

			verts.push_back(Vertex_PCU(v2, color, uv2));
			verts.push_back(Vertex_PCU(v4, color, uv4));
			verts.push_back(Vertex_PCU(v3, color, uv3));

			verts.push_back(Vertex_PCU(v2, color, uv2));
			verts.push_back(Vertex_PCU(v3, color, uv3));
			verts.push_back(Vertex_PCU(v1, color, uv1));
		}
	}
}

void TransformVertexArray3D(std::vector<Vertex_PCU>& verts, const Mat44& transform)
{
	for (Vertex_PCU& vert : verts) {
		vert.m_position = transform.TransformPosition3D(vert.m_position);
	}
}

AABB2 GetVertexBounds2D(const std::vector<Vertex_PCU>& verts) {
	if (verts.empty()) return AABB2(); 

	float minX = verts[0].m_position.x;
	float maxX = minX;
	float minY = verts[0].m_position.y;
	float maxY = minY;

	for (const Vertex_PCU& vert : verts) {
		if (vert.m_position.x < minX) minX = vert.m_position.x;
		if (vert.m_position.x > maxX) maxX = vert.m_position.x;
		if (vert.m_position.y < minY) minY = vert.m_position.y;
		if (vert.m_position.y > maxY) maxY = vert.m_position.y;
	}

	return AABB2(Vec2(minX, minY), Vec2(maxX, maxY));
}


void AddVertsForCylinder3D(std::vector<Vertex_PCU>& verts, const Vec3& start, const Vec3& end, float radius, const Rgba8& color, const AABB2& UVs, int numSlices) {

	Vec3 cylinderAxis = end - start;
	Vec3 upVector = cylinderAxis.GetNormalized();

	Vec3 arbitraryVector = (fabsf(upVector.x) < 0.99f) ? Vec3(1, 0, 0) : Vec3(0, 1, 0);
	Vec3 rightVector = CrossProduct3D(arbitraryVector, upVector).GetNormalized();
	Vec3 forwardVector = CrossProduct3D(rightVector, upVector).GetNormalized();

	// Generate sides
	for (int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex) {
		float thetaStart = (static_cast<float>(sliceIndex) / numSlices) * 2.0f * PI;
		float thetaEnd = (static_cast<float>(sliceIndex + 1) / numSlices) * 2.0f * PI;

		Vec3 quadStartLower = start + (cosf(thetaStart) * rightVector + sinf(thetaStart) * forwardVector) * radius;
		Vec3 quadEndLower = start + (cosf(thetaEnd) * rightVector + sinf(thetaEnd) * forwardVector) * radius;
		Vec3 quadStartUpper = end + (cosf(thetaStart) * rightVector + sinf(thetaStart) * forwardVector) * radius;
		Vec3 quadEndUpper = end + (cosf(thetaEnd) * rightVector + sinf(thetaEnd) * forwardVector) * radius;

		float uStart = UVs.m_maxs.x - (thetaStart / (2.0f * PI)) * (UVs.m_maxs.x - UVs.m_mins.x);
		float uEnd = UVs.m_maxs.x - (thetaEnd / (2.0f * PI)) * (UVs.m_maxs.x - UVs.m_mins.x);

		Vec2 uvStartLower = Vec2(uStart, UVs.m_mins.y);
		Vec2 uvEndLower = Vec2(uEnd, UVs.m_mins.y);
		Vec2 uvStartUpper = Vec2(uStart, UVs.m_maxs.y);
		Vec2 uvEndUpper = Vec2(uEnd, UVs.m_maxs.y);

		verts.push_back(Vertex_PCU(quadEndUpper, color, uvEndUpper));
		verts.push_back(Vertex_PCU(quadEndLower, color, uvEndLower));
		verts.push_back(Vertex_PCU(quadStartUpper, color, uvStartUpper));

		verts.push_back(Vertex_PCU(quadStartUpper, color, uvStartUpper));
		verts.push_back(Vertex_PCU(quadEndLower, color, uvEndLower));
		verts.push_back(Vertex_PCU(quadStartLower, color, uvStartLower));
	}
	Vec2 capCenterUV = 0.5f * (UVs.m_mins + UVs.m_maxs); // Assuming the cap texture is centralized
	float capUVRadius = 0.5f * (UVs.m_maxs.x - UVs.m_mins.x);

	// Generate caps
	for (int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex) {
		float thetaStart = (static_cast<float>(sliceIndex) / numSlices) * 2.0f * PI;
		float thetaEnd = (static_cast<float>(sliceIndex + 1) / numSlices) * 2.0f * PI;

		Vec3 pointStart = (cosf(thetaStart) * rightVector + sinf(thetaStart) * forwardVector) * radius;
		Vec3 pointEnd = (cosf(thetaEnd) * rightVector + sinf(thetaEnd) * forwardVector) * radius;

		Vec2 uvStart = Vec2(cosf(thetaStart), sinf(thetaStart)) * capUVRadius + capCenterUV;
		Vec2 uvEnd = Vec2(cosf(thetaEnd), sinf(thetaEnd)) * capUVRadius + capCenterUV;

		// Bottom cap with adjusted UVs
		verts.push_back(Vertex_PCU(start, color, capCenterUV));
		verts.push_back(Vertex_PCU(start + pointStart, color, uvStart));
		verts.push_back(Vertex_PCU(start + pointEnd, color, uvEnd));

		// Top cap with adjusted UVs
		verts.push_back(Vertex_PCU(end + pointEnd, color, Vec2(uvEnd.x, 1.0f - uvEnd.y)));
		verts.push_back(Vertex_PCU(end + pointStart, color, Vec2(uvStart.x, 1.0f - uvStart.y)));
		verts.push_back(Vertex_PCU(end, color, Vec2(0.5f, 0.5f)));
	}
}

void AddVertsForCone3D(std::vector<Vertex_PCU>& verts, const Vec3& start, const Vec3& end, float radius, const Rgba8& color, const AABB2& UVs, int numSlices) {
	UNUSED(UVs);
	Vec3 coneAxis = end - start;
	coneAxis.GetNormalized();

	Vec3 upVector(0, 1, 0);
	if (fabsf(DotProduct3D(upVector, coneAxis)) > 0.999f) {
		upVector = Vec3(0, 0, 1);
	}
	Vec3 rightVector = CrossProduct3D(upVector, coneAxis).GetNormalized();
	Vec3 forwardVector = CrossProduct3D(coneAxis, rightVector).GetNormalized();

	Vec2 tipUV = Vec2(0.5f, 1.0f);
	Vec2 baseCenterUV = Vec2(0.5f, 0.5f);

	for (int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex) {
		float startAngle = (static_cast<float>(sliceIndex) / numSlices) * 2.0f * PI;
		float endAngle = (static_cast<float>(sliceIndex + 1) / numSlices) * 2.0f * PI;

		Vec3 startVertex = start + (cosf(startAngle) * rightVector + sinf(startAngle) * forwardVector) * radius;
		Vec3 endVertex = start + (cosf(endAngle) * rightVector + sinf(endAngle) * forwardVector) * radius;

		// Base triangle
		verts.push_back(Vertex_PCU(startVertex, color, baseCenterUV));
		verts.push_back(Vertex_PCU(start, color, baseCenterUV));
		verts.push_back(Vertex_PCU(endVertex, color, baseCenterUV));

		// Side triangle
		verts.push_back(Vertex_PCU(endVertex, color, Vec2(endAngle / (2.0f * PI), 0.0f)));
		verts.push_back(Vertex_PCU(end, color, tipUV));
		verts.push_back(Vertex_PCU(startVertex, color, Vec2(startAngle / (2.0f * PI), 0.0f)));
	}
}


