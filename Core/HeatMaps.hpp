#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include <vector>

class TileHeatMap {
public:
	TileHeatMap(IntVec2 const& dimensions);

	void	SetAllValues(float newValue);
	float	GetValue(IntVec2 const& tileCoords) const;
	void	SetValue(IntVec2 const& tileCoords, float newValue);
	void	AddValue(IntVec2 const& tileCoords, float valueToAdd);
	void	AddVertsForDebugDraw( std::vector<Vertex_PCU>& verts, AABB2 bounds, FloatRange valueRange, Rgba8 lowColor, Rgba8 highColor, float specialValue, Rgba8 specialColor );
	IntVec2 GetDimensions();

private:
	IntVec2 m_dimensions;
	std::vector<float> m_values;
	int GetIndexFromCoordinates(IntVec2 const& tileCoords) const;
};