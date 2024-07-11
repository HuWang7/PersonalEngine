#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

enum class DebugRendererMode
{
	AlWAYS,
	USE_DEPTH,
	X_RAY,
};

struct DebugRenderConfig
{
	Renderer* m_renderer = nullptr;
	std::string m_fontName = "SquirrelFixedFont";
};

// Setup
void DebugrenderSystemStartup(const DebugRenderConfig& config);
void DebugRenderSystemShutdown();

// Control
void DebugRenderSetVisible();
void DebugRenderSetHidden();
void DebugRenderClear();

// Output
void DebugrenderBeginFrame();
void DebugRenderWorld( Camera& camera);
void DebugRenderScreen( Camera& camera);
void DebugRenderEndFrame();

// Geometry
void DebugAddWorldPoint(const Vec3& pos, float radius, float duration, const Rgba8& startColor = Rgba8::WHITE, const Rgba8& endColor = Rgba8::WHITE, DebugRendererMode mode = DebugRendererMode::USE_DEPTH);
void DebugAddWorldLine(const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor = Rgba8::WHITE, const Rgba8& endColor = Rgba8::WHITE, DebugRendererMode mode = DebugRendererMode::USE_DEPTH);
void DebugAddWorldWireCylinder(const Vec3& base, const Vec3& top, float radius, float duration, const Rgba8& startColor = Rgba8::WHITE, const Rgba8& endColor = Rgba8::WHITE, DebugRendererMode mode = DebugRendererMode::USE_DEPTH);
void DebugAddWorldWireSphere(const Vec3& center, float radius, float duration, const Rgba8& startColor = Rgba8::WHITE, const Rgba8& endColor = Rgba8::WHITE, DebugRendererMode mode = DebugRendererMode::USE_DEPTH);
void DebugAddWorldArrow(const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor = Rgba8::WHITE, const Rgba8& endColor = Rgba8::WHITE, DebugRendererMode mode = DebugRendererMode::USE_DEPTH);
void DebugAddWorldText(const std::string& text, const Mat44& transform, float textHeight, const Vec2& alignment, float duration, const Rgba8& startColor = Rgba8::WHITE, const Rgba8& endColor = Rgba8::WHITE, DebugRendererMode mode = DebugRendererMode::USE_DEPTH);
void DebugAddWorldBillboardText(const std::string& text, const Vec3& origin, float textHeight, const Vec2& alignment, float duration, const Rgba8& startColor = Rgba8::WHITE, const Rgba8& endColor = Rgba8::WHITE, DebugRendererMode mode = DebugRendererMode::USE_DEPTH);
void DebugAddWorldBasis(const Mat44& transform, float duration, DebugRendererMode mode = DebugRendererMode::USE_DEPTH);
void DebugAddScreenText(const std::string& text, const Vec2& position, float size, const Vec2& alignment, float duration, const Rgba8& startColor = Rgba8::WHITE, const Rgba8& endColor = Rgba8::WHITE);
void DebugAddMessage(const std::string& text, float duration, const Rgba8& startColor = Rgba8::WHITE, const Rgba8& endColor = Rgba8::WHITE);

// Console commands
bool Command_DebugRenderClear(EventArgs& args);
bool Command_DebugRenderToggle(EventArgs& args);

