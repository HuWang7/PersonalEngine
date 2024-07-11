#include "Engine/Core/Rgba8.hpp"	
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

Rgba8::Rgba8(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
	:r (red)
	,g (green)
	,b (blue)
	,a (alpha)
{
}

const Rgba8 Rgba8::WHITE = Rgba8(255, 255, 255, 255);
const Rgba8 Rgba8::RED = Rgba8(255, 0, 0, 255);
const Rgba8 Rgba8::LIGHTRED = Rgba8(255, 132, 136, 255);
const Rgba8 Rgba8::YELLOW = Rgba8(255, 255, 0, 255);
const Rgba8 Rgba8::GREEN = Rgba8(0, 255, 0, 255);
const Rgba8 Rgba8::BLACK = Rgba8(0, 0, 0, 255);
const Rgba8 Rgba8::BLUE = Rgba8(0, 0, 255, 255);
const Rgba8 Rgba8::MAGENTA = Rgba8(255, 0, 255, 255);
const Rgba8 Rgba8::CYAN = Rgba8(0, 255, 255, 255);
const Rgba8 Rgba8::GOLD = Rgba8(255, 215, 0, 255);
const Rgba8 Rgba8::GRAY = Rgba8(128, 128, 128, 255);


void Rgba8::SetFromText(const char* text) {
	Strings parts = SplitStringOnDelimiter(std::string(text), ',');
	if (parts.size() == 3) {
		r = static_cast<unsigned char>(Clamp(atoi(parts[0].c_str()), 0, 255));
		g = static_cast<unsigned char>(Clamp(atoi(parts[1].c_str()), 0, 255));
		b = static_cast<unsigned char>(Clamp(atoi(parts[2].c_str()), 0, 255));
		a = 255;  // Default alpha
	}
	else if (parts.size() == 4) {
		r = static_cast<unsigned char>(Clamp(atoi(parts[0].c_str()), 0, 255));
		g = static_cast<unsigned char>(Clamp(atoi(parts[1].c_str()), 0, 255));
		b = static_cast<unsigned char>(Clamp(atoi(parts[2].c_str()), 0, 255));
		a = static_cast<unsigned char>(Clamp(atoi(parts[3].c_str()), 0, 255));
	}
}

Rgba8 Rgba8::RgbaInterpolate(Rgba8 start, Rgba8 end, float fractionOfEnd)
{
	float temR = Interpolate(NormalizeByte(start.r), NormalizeByte(end.r), fractionOfEnd);
	float temG = Interpolate(NormalizeByte(start.g), NormalizeByte(end.g), fractionOfEnd);
	float temB = Interpolate(NormalizeByte(start.b), NormalizeByte(end.b), fractionOfEnd);
	float temA = Interpolate(NormalizeByte(start.a), NormalizeByte(end.a), fractionOfEnd);
	return Rgba8(DenormalizeByte(temR), DenormalizeByte(temG), DenormalizeByte(temB), DenormalizeByte(temA));
}

void Rgba8::GetAsFloats(float* colorAsFloats) const {
	if (colorAsFloats != nullptr) {
		colorAsFloats[0] = static_cast<float>(r) / 255.0f; // Convert red to float and store
		colorAsFloats[1] = static_cast<float>(g) / 255.0f; // Convert green to float and store
		colorAsFloats[2] = static_cast<float>(b) / 255.0f; // Convert blue to float and store
		colorAsFloats[3] = static_cast<float>(a) / 255.0f; // Convert alpha to float and store
	}
}

Rgba8 Rgba8::Lerp(const Rgba8& startColor, const Rgba8& endColor, float t)
{
	float inverseT = 1.0f - t;
	Rgba8 resultColor;
	resultColor.r = static_cast<unsigned char>(startColor.r * inverseT + endColor.r * t);
	resultColor.g = static_cast<unsigned char>(startColor.g * inverseT + endColor.g * t);
	resultColor.b = static_cast<unsigned char>(startColor.b * inverseT + endColor.b * t);
	resultColor.a = static_cast<unsigned char>(startColor.a * inverseT + endColor.a * t);
	return resultColor;
}


