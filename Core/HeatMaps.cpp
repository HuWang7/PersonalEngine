#include "Engine/Core/HeatMaps.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Rgba8.hpp"

TileHeatMap::TileHeatMap(IntVec2 const& dimensions)
	: m_dimensions(dimensions)
{
	m_values.resize(m_dimensions.x * m_dimensions.y);
	for (int i = 0; i < m_values.size(); ++i)
	{
		m_values[i] = 0.f;
	}
}

void TileHeatMap::SetAllValues(float newValue)
{
	for (int i = 0; i < m_values.size(); ++i)
	{
		m_values[i] = newValue;
	}
}

float TileHeatMap::GetValue(IntVec2 const& tileCoords) const
{
	int index = Clamp(GetIndexFromCoordinates(tileCoords), 0, m_dimensions.x * m_dimensions.y -1);
	return m_values[index];
}

void TileHeatMap::SetValue(IntVec2 const& tileCoords, float newValue)
{
	int index = Clamp(GetIndexFromCoordinates(tileCoords), 0, m_dimensions.x * m_dimensions.y - 1);
	m_values[index] = newValue;
}

void TileHeatMap::AddValue(IntVec2 const& tileCoords, float valueToAdd)
{
	int index = Clamp(GetIndexFromCoordinates(tileCoords), 0, m_dimensions.x * m_dimensions.y - 1);
	m_values[index] += valueToAdd;
}

void TileHeatMap::AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, AABB2 bounds, FloatRange valueRange, Rgba8 lowColor, Rgba8 highColor, float specialValue, Rgba8 specialColor)
{
	float tileWidth = (bounds.m_maxs.x - bounds.m_mins.x) / (float)m_dimensions.x;
	float tileHeight = (bounds.m_maxs.y - bounds.m_mins.y) / (float)m_dimensions.y;
	for (int y = 0; y < m_dimensions.y; ++y) {
		for (int x = 0; x < m_dimensions.x; ++x) 
		{
			Vec2 mins = Vec2(tileWidth*x, tileHeight*y) + bounds.m_mins;
			Vec2 maxs =  mins + Vec2(tileWidth, tileHeight);
			AABB2 tileAABB(mins, maxs);

			float tileValue = GetValue(IntVec2(x, y));
			Rgba8 tileColor;

			if (tileValue == specialValue) {
				tileColor = specialColor;
			}
			else {
				float normalizedValue = RangeMapClamped(tileValue, valueRange.m_min, valueRange.m_max, 0.0f, 1.0f);
				tileColor = tileColor.RgbaInterpolate(lowColor, highColor, normalizedValue);
			} 
			AddVertsForAABB2D(verts, tileAABB, tileColor);
		}
	}
}

IntVec2 TileHeatMap::GetDimensions()
{
	return m_dimensions;
}

int TileHeatMap::GetIndexFromCoordinates(IntVec2 const& tileCoords) const
{
	return tileCoords.x + m_dimensions.x * tileCoords.y;
}
