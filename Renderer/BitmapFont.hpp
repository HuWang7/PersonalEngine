#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include <vector>

enum TextBoxMode
{
	SHRINK,
	OVERRUN
};

class BitmapFont
{
	friend class Renderer; // Only the Renderer can create new BitmapFont objects!

private:
	BitmapFont(char const* fontFilePathNameWithNoExtension, Texture& fontTexture);

public:
	Texture& GetTexture();

	void AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f);
	float GetTextWidth(float cellHeight, std::string const& text, float cellAspect = 1.f);
	void AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, AABB2 const& box, float cellHeight,
		std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f,
		Vec2 const& alignment = Vec2(.5f, .5f), TextBoxMode mode = TextBoxMode::SHRINK, int maxGlyphsToDraw = 99999999);
	float GetMin(float a, float b);
	void AddvertsForText3DAtOriginXForward(std::vector<Vertex_PCU>& verts, float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect= 1.f, Vec2 const& alignment = Vec2(0.5f, 0.5f), int maxGlyphsToDraw = 999999999);


protected:
	float GetGlyphAspect(int glyphUnicode) const; // For now this will always return 1.0f!!!

protected:
	std::string	m_fontFilePathNameWithNoExtension;
	SpriteSheet	m_fontGlyphsSpriteSheet;
};
