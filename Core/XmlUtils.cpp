#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/FloatRange.hpp"


int ParseXmlAttribute(XmlElement const& element, char const* attributeName, int defaultValue)
{
	const char* valueAsText = element.Attribute(attributeName);
	if (valueAsText)
	{
		return atoi(valueAsText);
	}
	return defaultValue;
}

char ParseXmlAttribute(XmlElement const& element, char const* attributeName, char defaultValue)
{
	const char* valueAsText = element.Attribute(attributeName);
	if (valueAsText)
	{
		return valueAsText[0];
	}
	return defaultValue;
}

bool ParseXmlAttribute(XmlElement const& element, char const* attributeName, bool defaultValue)
{
	const char* valueAsText = element.Attribute(attributeName);
	if (valueAsText) {
		return strcmp(valueAsText, "true") == 0 || strcmp(valueAsText, "1") == 0;
	}
	return defaultValue;
}

float ParseXmlAttribute(XmlElement const& element, char const* attributeName, float defaultValue)
{
	const char* valueAsText = element.Attribute(attributeName);
	if (valueAsText)
	{
		return static_cast<float>(atof(valueAsText));
	}
	return defaultValue;
}

Rgba8 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue)
{
	const char* valueAsText = element.Attribute(attributeName);
	Rgba8 rgba = defaultValue;
	if (valueAsText) {
		rgba.SetFromText(valueAsText);
	}
	return rgba;
}

Vec2 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec2 const& defaultValue)
{
	const char* valueAsText = element.Attribute(attributeName);
	Vec2 vec2 = defaultValue;
	if (valueAsText) {
		vec2.SetFromText(valueAsText);
	}
	return vec2;
}

IntVec2 ParseXmlAttribute(XmlElement const& element, char const* attributeName, IntVec2 const& defaultValue)
{
	char const* valueAsText = element.Attribute(attributeName);
	IntVec2 intVec2 = defaultValue;
	if (valueAsText)
	{
		intVec2.SetFromText(valueAsText);
	}
	return intVec2;
}

std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, std::string const& defaultValue)
{
	char const* valueAsText = element.Attribute(attributeName);
	if (valueAsText)
	{
		return std::string(valueAsText);
	}
	return defaultValue;
}

Strings ParseXmlAttribute(XmlElement const& element, char const* attributeName, Strings const& defaultValues)
{
	const char* valueAsText = element.Attribute(attributeName);
	if (valueAsText) {
		return SplitStringOnDelimiter(std::string(valueAsText), ',');
	}
	return defaultValues;
}

std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, char const* defaultValue)
{
	char const* valueAsText = element.Attribute(attributeName);
	if (valueAsText)
	{
		return std::string(valueAsText);
	}
	return defaultValue;
}

Vec3 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec3 const& defaultValue)
{
	const char* valueAsText = element.Attribute(attributeName);
	Vec3 vec3 = defaultValue;
	if (valueAsText) {
		vec3.SetFromText(valueAsText);
	}
	return vec3;
}

FloatRange ParseXmlAttribute(XmlElement const& element, char const* attributeName, FloatRange const& defaultValue, char delimeter)
{
	const char* valueAsText = element.Attribute(attributeName);
	FloatRange floatRange = defaultValue;
	if (valueAsText)
	{
		Strings values = SplitStringOnDelimiter(valueAsText, delimeter);
		if (values.size() == 2)
		{
			float min = (float)atof(values[0].c_str());
			float max = (float)atof(values[1].c_str());
			floatRange = FloatRange(min, max);
		}
	}
	return floatRange;
}

