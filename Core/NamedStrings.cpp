#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"

void NamedStrings::PopulateFromXmlElementAttributes(XmlElement const& element)
{
	for (const XmlAttribute* attribute = element.FirstAttribute(); attribute; attribute = attribute->Next()) {
		std::string keyName = attribute->Name();
		std::string value = attribute->Value();
		m_keyValuePairs[keyName] = value;
	}
}

void NamedStrings::SetValue(std::string const& keyName, std::string const& newValue)
{
	m_keyValuePairs[keyName] = newValue;
}

std::string NamedStrings::GetValue(std::string const& keyName, std::string const& defaultValue) const
{
	auto findIt = m_keyValuePairs.find(keyName);
	if (findIt != m_keyValuePairs.end()) {
		return findIt->second;
	}
	return defaultValue;
}

bool NamedStrings::GetValue(std::string const& keyName, bool defaultValue) const
{
	auto findIt = m_keyValuePairs.find(keyName);
	if (findIt != m_keyValuePairs.end()) {
		return findIt->second == "true";
	}
	return defaultValue;
}

int NamedStrings::GetValue(std::string const& keyName, int defaultValue) const
{
	auto findIt = m_keyValuePairs.find(keyName);
	if (findIt != m_keyValuePairs.end()) {
		return std::atoi(findIt->second.c_str());
	}
	return defaultValue;
}

float NamedStrings::GetValue(std::string const& keyName, float defaultValue) const
{
	auto findIt = m_keyValuePairs.find(keyName);
	if (findIt != m_keyValuePairs.end()) {
		return std::stof(findIt->second);
	}
	return defaultValue;
}

std::string NamedStrings::GetValue(std::string const& keyName, char const* defaultValue) const
{
	return GetValue(keyName, std::string(defaultValue));
}

Rgba8 NamedStrings::GetValue(std::string const& keyName, Rgba8 const& defaultValue) const
{
	auto findIt = m_keyValuePairs.find(keyName);
	if (findIt != m_keyValuePairs.end()) {
		Rgba8 value;
		value.SetFromText(findIt->second.c_str());
		return value;
	}
	return defaultValue;
}

Vec2 NamedStrings::GetValue(std::string const& keyName, Vec2 const& defaultValue) const
{
	auto findIt = m_keyValuePairs.find(keyName);
	if (findIt != m_keyValuePairs.end()) {
		Vec2 value;
		value.SetFromText(findIt->second.c_str());
		return value;
	}
	return defaultValue;
}

IntVec2 NamedStrings::GetValue(std::string const& keyName, IntVec2 const& defaultValue) const
{
	auto findIt = m_keyValuePairs.find(keyName);
	if (findIt != m_keyValuePairs.end()) {
		IntVec2 value;
		value.SetFromText(findIt->second.c_str());
		return value;
	}
	return defaultValue;
}
