#define STB_IMAGE_IMPLEMENTATION
#include "Engine/Core/Image.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

Image::Image(char const* imageFilePath)
	: m_imageFilePath(imageFilePath)
{
	IntVec2 dimensions = IntVec2::ZERO;	// This will be filled in for us to indicate image width & height
	int bytesPerTexel = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	//int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)
									
	// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
	stbi_set_flip_vertically_on_load(1); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* texelData = stbi_load(imageFilePath, &dimensions.x, &dimensions.y, &bytesPerTexel, 4); // Force load with 4 channels (RGBA)
	
	// Check if the load was successful
	GUARANTEE_OR_DIE(texelData, Stringf("Failed to load image \"%s\"", imageFilePath));

	
	m_dimensions = IntVec2(dimensions.x, dimensions.y);
	m_rgbaTexels.assign(reinterpret_cast<Rgba8*>(texelData), reinterpret_cast<Rgba8*>(texelData) + dimensions.x * dimensions.y);
	stbi_image_free(texelData);
}

Image::Image(IntVec2 size, Rgba8 color)
	:m_dimensions(size)
{
	m_rgbaTexels.resize(size.x * size.y, color);
}

std::string const& Image::GetImageFilePath() const
{
	return m_imageFilePath;
}

const void* Image::GetRawData() const
{
	if (!m_rgbaTexels.empty()) {
		return static_cast<const void*>(m_rgbaTexels.data());
	}
	return nullptr;
}

IntVec2 Image::GetDimensions() const
{
	return m_dimensions;
}

Rgba8 Image::GetTexelColor(IntVec2 const& texelCoords) const
{
	int index = texelCoords.y * m_dimensions.x + texelCoords.x;
	return m_rgbaTexels[index];
}

void Image::SetTexelColor(IntVec2 const& texelCoords, Rgba8 const& newColor)
{
	int index = texelCoords.y * m_dimensions.x + texelCoords.x;
	m_rgbaTexels[index] = newColor;
}
