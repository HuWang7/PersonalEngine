#include "Engine/Core/Vertex_PCU.hpp"

Vertex_PCU::Vertex_PCU(const Vec3& position, const Rgba8& color, const Vec2& uvTexCoords)
	: m_position(position)
	, m_color(color)
	, m_uvTexCoords(uvTexCoords) 
{
}