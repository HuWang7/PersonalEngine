#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"

BitmapFont::BitmapFont(char const* fontFilePathNameWithNoExtension, Texture& fontTexture)
	: m_fontFilePathNameWithNoExtension(fontFilePathNameWithNoExtension)
	, m_fontGlyphsSpriteSheet(fontTexture, IntVec2(16, 16)) 
{

}

Texture& BitmapFont::GetTexture()
{
	return m_fontGlyphsSpriteSheet.GetTexture(); 
}

void BitmapFont::AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint /*= Rgba8::WHITE*/, float cellAspect /*= 1.f*/)
{
	Vec2 cursor = textMins;

	for (char c : text) {
		const SpriteDefinition& SpriteDefKKK = m_fontGlyphsSpriteSheet.GetSpriteDef(c);
		Vec2 uvMins, uvMaxs;
		SpriteDefKKK.GetUVs(uvMins, uvMaxs);
		float glyphWidth = cellHeight * GetGlyphAspect(c) * cellAspect;
		AABB2 bounds(Vec2(cursor.x, cursor.y), Vec2(cursor.x + glyphWidth, cursor.y + cellHeight));
		AddVertsForAABB2D(vertexArray, bounds, tint, uvMins, uvMaxs);
		cursor.x += glyphWidth;
	}
}

float BitmapFont::GetTextWidth(float cellHeight, std::string const& text, float cellAspect /*= 1.f*/)
{
	float totalWidth = 0.0f;

	for (char c : text) {
		totalWidth += cellHeight * GetGlyphAspect(c) * cellAspect;
	}

	return totalWidth;
}

void BitmapFont::AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, AABB2 const& box, float cellHeight, std::string const& text, Rgba8 const& tint /*= Rgba8::WHITE*/, float cellAspect /*= 1.f*/, Vec2 const& alignment /*= Vec2(.5f, .5f)*/, TextBoxMode mode /*= TextBoxMode::SHRINK*/, int maxGlyphsToDraw /*= 99999999*/)
{
	// Split into 1+ line, on newline (ascii 10) characters
	Strings textLines = SplitStringOnDelimiter(text, '\n');
	int numLines = (int) textLines.size();
	std::vector<float> linePureWidths(numLines);

	//Compute the width of each line, and the overall size of the text to be
	float textBlockWidthPure = 0.f;
	for (int i = 0; i < numLines; ++i)
	{
		float lineWidthPure = GetTextWidth(1.f, textLines[i]);
		linePureWidths[i] = lineWidthPure;
		if (lineWidthPure > textBlockWidthPure)
		{
			textBlockWidthPure = lineWidthPure;
		}
	}

	Vec2 textActualSize = Vec2(cellAspect * cellHeight * textBlockWidthPure, cellHeight * numLines);
	Vec2 boxActualSize = box.GetDimensions();

	// Compute scale required to fit in box (1.0 if it already fits)
	if (mode == TextBoxMode::SHRINK)
	{
		float horizScale = textActualSize.x > boxActualSize.x ? boxActualSize.x / textActualSize.x : 1.f;
		float vertScale = textActualSize.y > boxActualSize.y ? boxActualSize.y / textActualSize.y : 1.f;
		float fitScale = GetMin(horizScale, vertScale);
		textActualSize *= fitScale;
		cellHeight *= fitScale;
	}

	

	// Compute aligned text position
	Vec2 extraSpace = boxActualSize - textActualSize;
	Vec2 textOffsetFromMins = extraSpace * alignment;
	Vec2 textMins = box.m_mins + textOffsetFromMins;

	// Print each line
	int totalGlyphsDrawn = 0;
	for (int lineIndex = 0; lineIndex < numLines && totalGlyphsDrawn < maxGlyphsToDraw; ++lineIndex) {
		std::string lineToDraw = textLines[lineIndex];
		int glyphsInLine = std::min(static_cast<int>(lineToDraw.length()), maxGlyphsToDraw - totalGlyphsDrawn);

		// Extract substring based on the number of glyphs to draw
		lineToDraw = lineToDraw.substr(0, glyphsInLine);
		totalGlyphsDrawn += glyphsInLine;

		float clampedAlignmentX = ClampZeroToOne(alignment.x);
		float lineActualWidth = linePureWidths[lineIndex] * cellHeight * cellAspect;
		float lineExtraSpace = textActualSize.x - lineActualWidth;
		float textLineMinY = textMins.y + (cellHeight * float(numLines - 1 - lineIndex));
		float textLineMinX = textMins.x + (lineExtraSpace * clampedAlignmentX);

		AddVertsForText2D(vertexArray, Vec2(textLineMinX, textLineMinY), cellHeight, lineToDraw, tint, cellAspect);

		if (totalGlyphsDrawn >= maxGlyphsToDraw) {
			break; // Stop drawing if the max glyph limit is reached
		}
	}
}

float BitmapFont::GetMin(float a, float b)
{
	return std::min(a, b);
}

void BitmapFont::AddvertsForText3DAtOriginXForward(std::vector<Vertex_PCU>& verts, float cellHeight, std::string const& text, Rgba8 const& tint /*= Rgba8::WHITE*/, float cellAspect/*= 1.f*/, Vec2 const& alignment /*= Vec2(0.5f, 0.5f)*/, int maxGlyphsToDraw /*= 999999999*/)
{
	float totalTextWidth = GetTextWidth(cellHeight, text, cellAspect);

	Vec2 adjustedStartPos = Vec2(0.0f, 0.0f);
	adjustedStartPos.x -= totalTextWidth * alignment.x;

	std::vector<Vertex_PCU> textVerts;
	int glyphsDrawn = 0;
	for (char c : text) {
		if (glyphsDrawn >= maxGlyphsToDraw) break; 

		AddVertsForText2D(textVerts, adjustedStartPos, cellHeight, std::string(1, c), tint, cellAspect);
		float glyphWidth = cellHeight * GetGlyphAspect(c) * cellAspect;
		adjustedStartPos.x += glyphWidth;
		glyphsDrawn++;
	}

	Mat44 transform = Mat44::CreateTranslation3D(Vec3(0.0f, 0.0f, 0.0f));
	TransformVertexArray3D(textVerts, transform);
	verts.insert(verts.end(), textVerts.begin(), textVerts.end());
}

float BitmapFont::GetGlyphAspect(int glyphUnicode) const
{
	UNUSED(glyphUnicode);
	return 1.0f;
}

 