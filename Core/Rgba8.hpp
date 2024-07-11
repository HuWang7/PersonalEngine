#pragma once

struct Rgba8
{
	unsigned char r = 255;
	unsigned char g = 255;
	unsigned char b = 255;
	unsigned char a = 255;

	Rgba8() = default;

	Rgba8(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);

	static const Rgba8 WHITE;
	static const Rgba8 RED;
	static const Rgba8 LIGHTRED;
	static const Rgba8 YELLOW;
	static const Rgba8 GREEN;
	static const Rgba8 BLACK;
	static const Rgba8 BLUE;
	static const Rgba8 MAGENTA;
	static const Rgba8 CYAN;
	static const Rgba8 GOLD;
	static const Rgba8 GRAY;


	bool Equals(const Rgba8& other) const
	{
		return r == other.r && g == other.g && b == other.b && a == other.a;
	}

	void SetFromText(const char* text);

	Rgba8 RgbaInterpolate(Rgba8 start, Rgba8 end, float fractionOfEnd);
	Rgba8 Lerp(const Rgba8& startColor, const Rgba8& endColor, float t);
	void GetAsFloats(float* colorAsFloats) const;
	
};