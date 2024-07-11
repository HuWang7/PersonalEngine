#include "Engine/Renderer/DebugRenderer.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include <string.h>
#include <sstream>
#include <iomanip> 
#include "Game/Game.hpp"
//#include "Game/Chunk.hpp"

DebugRenderConfig g_debugRenderConfig;

extern Game* m_theGame;
extern BitmapFont* g_theFont; 
extern DevConsole* g_theConsole;
extern EventSystem* g_theEventSystem;
Texture* fontTexture;
Clock	m_clock;
bool m_debugDraw = true;

struct DebugObject 
{
	std::vector<Vertex_PCU> vertices;
	float duration; 
	Rgba8 startColor;
	Rgba8 endColor;
	DebugRendererMode debugMode;
	Vec3 position;

	DebugObject(const std::vector<Vertex_PCU>& verts, float dur, const Rgba8& startCol = Rgba8::WHITE, const Rgba8& endCol = Rgba8::WHITE, const DebugRendererMode& mode = DebugRendererMode::USE_DEPTH, Vec3 pos = Vec3(0.f, 0.f, 0.f))
		: vertices(verts)
		, duration(dur)
		, startColor(startCol)
		, endColor(endCol)
		, debugMode(mode)
		, position(pos)
	{

	}
};

std::vector<DebugObject> g_debugPoints;
std::vector<DebugObject> g_debugLines;
std::vector<DebugObject> g_debugSpheres;
std::vector<DebugObject> g_debugCylinders;
std::vector<DebugObject> g_debugArrows;
std::vector<DebugObject> g_debugBillboardeds;
std::vector<DebugObject> g_debugScreenTexts;

float deltaSeconds;
Vec2 textPosition;
float textSize;
Camera m_worldCamera;

void RemoveExpiredDebugObjects(std::vector<DebugObject>& debugObjects)
{
	auto endIt = std::remove_if(debugObjects.begin(), debugObjects.end(), [](const DebugObject& obj) {
		return obj.duration != -1.0f && obj.duration <= 0.0f;
		});
	debugObjects.erase(endIt, debugObjects.end());
}

void SetRendererMode(const DebugRendererMode& mode)
{
	if (mode == DebugRendererMode::AlWAYS)
	{
		g_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::DISABLED);
	}
	else if (mode == DebugRendererMode::USE_DEPTH)
	{
		g_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::ENABLED);
	}
	else if (mode == DebugRendererMode::X_RAY)
	{
		g_debugRenderConfig.m_renderer->SetBlendMode(BlendMode::ALPHA);
		g_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::DISABLED);
	}
}

void CheckDebugRendererMode(DebugObject& object)
{
	if (object.debugMode == DebugRendererMode::X_RAY)
	{
		Rgba8 lightenedColor = Rgba8(object.startColor.r + 50, object.startColor.g + 50, object.startColor.b + 50, object.startColor.a - 100);
		for (Vertex_PCU& vert : object.vertices) vert.m_color = lightenedColor;
		g_debugRenderConfig.m_renderer->DrawVertexArray((int)object.vertices.size(), object.vertices.data());
		g_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::ENABLED);
		for (Vertex_PCU& vert : object.vertices) vert.m_color = object.startColor;
		g_debugRenderConfig.m_renderer->DrawVertexArray((int)object.vertices.size(), object.vertices.data());
	}
}

void DebugrenderSystemStartup(const DebugRenderConfig& config)
{
	g_debugRenderConfig = config;
	fontTexture = &g_theFont->GetTexture();
	Clock::TickSystemClock();
	g_theEventSystem->SubscribeEventCallbackFunction("clear", Command_DebugRenderClear);
	g_theEventSystem->SubscribeEventCallbackFunction("changeDebugMode", Command_DebugRenderToggle);
}

void DebugRenderSystemShutdown()
{
}

void DebugRenderSetVisible()
{
	m_debugDraw = true;
}

void DebugRenderSetHidden()
{
	m_debugDraw = false;
}

void DebugRenderClear()
{
	g_debugPoints.clear();
	g_debugPoints.clear();
	g_debugLines.clear();
	g_debugSpheres.clear();
	g_debugCylinders.clear();
	g_debugArrows.clear();
	g_debugBillboardeds.clear();
	g_debugScreenTexts.clear();
}

void DebugrenderBeginFrame()
{
	Clock::TickSystemClock();
	deltaSeconds =m_clock.GetDeltaSeconds();

	for (auto& object : g_debugPoints) {
		object.duration -= deltaSeconds;
	}
	for (auto& object : g_debugLines) {
		object.duration -= deltaSeconds;
	}
	for (auto& object : g_debugSpheres) {
		object.duration -= deltaSeconds;
	}
	for (auto& object : g_debugCylinders) {
		object.duration -= deltaSeconds;
	}
	for (auto& object : g_debugArrows) {
		object.duration -= deltaSeconds;
	}
	for (auto& object : g_debugBillboardeds) {
		object.duration -= deltaSeconds;
	}
	for (auto& object : g_debugScreenTexts) {
		object.duration -= deltaSeconds;
	}

	RemoveExpiredDebugObjects(g_debugPoints);
	RemoveExpiredDebugObjects(g_debugLines);
	RemoveExpiredDebugObjects(g_debugSpheres);
	RemoveExpiredDebugObjects(g_debugCylinders);
	RemoveExpiredDebugObjects(g_debugArrows);
	RemoveExpiredDebugObjects(g_debugBillboardeds);
	RemoveExpiredDebugObjects(g_debugScreenTexts);
}

void DebugRenderWorld( Camera& camera)
{
	m_worldCamera = camera;

	g_debugRenderConfig.m_renderer->BeginCamera(camera);
	for (auto object : g_debugPoints)
	{
		SetRendererMode(object.debugMode);
		g_debugRenderConfig.m_renderer->SetModelConstants();
		g_debugRenderConfig.m_renderer->DrawVertexArray((int)object.vertices.size(), object.vertices.data());
	}
	for (auto object : g_debugLines)
	{
		SetRendererMode(object.debugMode);
		g_debugRenderConfig.m_renderer->SetModelConstants();
		g_debugRenderConfig.m_renderer->DrawVertexArray((int)object.vertices.size(), object.vertices.data());
		CheckDebugRendererMode(object);
	}
	for (auto& object : g_debugSpheres)
	{
		float elapsedRatio = std::max(0.0f, object.duration) / 5.f;
		Rgba8 currentColor = currentColor.Lerp(object.endColor, object.startColor, elapsedRatio);
		SetRendererMode(object.debugMode);
		g_debugRenderConfig.m_renderer->SetRasterizerMode(RasterizerMode::WIREFRAME_CULL_NONE);
		for (auto& vert : object.vertices) {
			vert.m_color = currentColor;
		}
		g_debugRenderConfig.m_renderer->SetModelConstants();
		g_debugRenderConfig.m_renderer->DrawVertexArray((int)object.vertices.size(), object.vertices.data());
		g_debugRenderConfig.m_renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	}

	for (auto object : g_debugCylinders)
	{
		float elapsedRatio = std::max(0.0f, object.duration) / 10.f;
		Rgba8 currentColor = currentColor.Lerp(object.endColor, object.startColor, elapsedRatio);
		SetRendererMode(object.debugMode);
		g_debugRenderConfig.m_renderer->SetRasterizerMode(RasterizerMode::WIREFRAME_CULL_NONE);
		for (auto& vert : object.vertices) {
			vert.m_color = currentColor;
		}
		g_debugRenderConfig.m_renderer->SetModelConstants();
		g_debugRenderConfig.m_renderer->DrawVertexArray((int)object.vertices.size(), object.vertices.data());
		g_debugRenderConfig.m_renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	}

	for (auto object : g_debugArrows)
	{
		SetRendererMode(object.debugMode);
		g_debugRenderConfig.m_renderer->SetModelConstants();
		g_debugRenderConfig.m_renderer->DrawVertexArray((int)object.vertices.size(), object.vertices.data());
	}
	for (auto object : g_debugBillboardeds)
	{
		float elapsedRatio = std::max(0.0f, object.duration) / 10.f;
		Rgba8 currentColor = currentColor.Lerp(object.endColor, object.startColor, elapsedRatio);
		SetRendererMode(object.debugMode);
		Mat44 playerMatrix = camera.GetModelMatrix();
		Mat44 billboardMatrix = GetBillboardMatrix(BillboardType::FULL_CAMERA_OPPOSING, playerMatrix, object.position);
		billboardMatrix.AppendXRotation(90.f);
		billboardMatrix.AppendYRotation(90.f);
		g_debugRenderConfig.m_renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
		g_debugRenderConfig.m_renderer->BindTexture(fontTexture);
		g_debugRenderConfig.m_renderer->SetModelConstants(billboardMatrix, currentColor);
		g_debugRenderConfig.m_renderer->DrawVertexArray((int)object.vertices.size(), object.vertices.data());
		g_debugRenderConfig.m_renderer->BindTexture(nullptr);
		g_debugRenderConfig.m_renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	}
	g_debugRenderConfig.m_renderer->EndCamera(camera);
}

void DebugRenderScreen( Camera& camera)
{
	g_debugRenderConfig.m_renderer->BeginCamera(camera);
	// text for total time, fps and time dilation
	float totalTime = m_clock.GetTotalSeconds();
	float fps = 1.0f / m_clock.GetDeltaSeconds();
	float timeDilation = m_clock.GetTimeScale();

	std::stringstream ss;
	ss << std::fixed << std::setprecision(2);
	ss << "Time: " << totalTime << "s\n";
	ss << "FPS: " << fps << "\n";
	ss << "Time Dilation: " << timeDilation << "\n";
	std::string debugText = ss.str();

	// text for camera position
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(1);
	stream << " Position: " << m_worldCamera.m_position.x << ", " << m_worldCamera.m_position.y << ", " << m_worldCamera.m_position.z << "\n";
	/*int numOfVerts = 0;
	for (auto& chunk : m_theGame->m_world->m_activeChunks)
	{
		if (chunk.second)
		{
			numOfVerts += (int)chunk.second->m_cpuMesh.size();
		}
	}
	stream << " Chunk: " << m_theGame->m_world->m_activeChunks.size() << ", " << "Verts: " << numOfVerts;*/
	std::string text = stream.str();

	std::vector<Vertex_PCU> vertices;
	vertices.clear();
	g_theFont->AddVertsForText2D(vertices, Vec2(0.f, camera.GetOrthographicTopRight().y - 20.f), 15.f, text, Rgba8::WHITE, 1.f);
	g_theFont->AddVertsForText2D(vertices, Vec2(camera.GetOrthographicTopRight().x - 650.f, camera.GetOrthographicTopRight().y - 20.f), 15.f, debugText, Rgba8::WHITE, 1.f);
	SetRendererMode(DebugRendererMode::USE_DEPTH);
	g_debugRenderConfig.m_renderer->BindTexture(fontTexture);
	g_debugRenderConfig.m_renderer->SetModelConstants();
	g_debugRenderConfig.m_renderer->DrawVertexArray(static_cast<int>(vertices.size()), vertices.data());
	g_debugRenderConfig.m_renderer->BindTexture(nullptr);

	// text for camera orientation
	float lineHeight = 20.0f;
	float currentY = camera.GetOrthographicTopRight().y - 2 * lineHeight; 
														
	for (auto it = g_debugScreenTexts.rbegin(); it != g_debugScreenTexts.rend(); ++it) {
		DebugObject& textObject = *it;

		SetRendererMode(textObject.debugMode);
		g_debugRenderConfig.m_renderer->BindTexture(fontTexture);

		std::vector<Vertex_PCU> tempVertices = textObject.vertices; 
		for (Vertex_PCU& vert : tempVertices) {
			vert.m_position.y += currentY - it->position.y; 
		}
		g_debugRenderConfig.m_renderer->SetModelConstants();
		g_debugRenderConfig.m_renderer->DrawVertexArray(static_cast<int>(tempVertices.size()), tempVertices.data());
		g_debugRenderConfig.m_renderer->BindTexture(nullptr);

		currentY -= lineHeight;
	}
	g_debugRenderConfig.m_renderer->EndCamera(camera);
}

void DebugRenderEndFrame()
{

}

void DebugAddWorldPoint(const Vec3& pos, float radius, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRendererMode mode /*= DebugRendererMode::USE_DEPTH*/)
{
	std::vector<Vertex_PCU> vertices;
	AddVertsForSphere3D(vertices, pos, radius, startColor, AABB2::ZERO_TO_ONE, 16);
	g_debugPoints.push_back(DebugObject(vertices, duration, startColor, endColor, mode, Vec3(0.f, 0.f, 0.f)));
}

void DebugAddWorldLine(const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRendererMode mode /*= DebugRendererMode::USE_DEPTH*/)
{
	std::vector<Vertex_PCU> vertices;
	AddVertsForCylinder3D(vertices, start, end, radius, startColor, AABB2::ZERO_TO_ONE, 16);
	g_debugLines.push_back(DebugObject(vertices, duration, startColor, endColor, mode, Vec3(0.f, 0.f, 0.f)));
}

void DebugAddWorldWireCylinder(const Vec3& base, const Vec3& top, float radius, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRendererMode mode /*= DebugRendererMode::USE_DEPTH*/)
{
	std::vector<Vertex_PCU> vertices;
	AddVertsForCylinder3D(vertices, base, top, radius);
	g_debugCylinders.push_back(DebugObject(vertices, duration, startColor, endColor, mode, Vec3(0.f, 0.f, 0.f)));
}

void DebugAddWorldWireSphere(const Vec3& center, float radius, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRendererMode mode /*= DebugRendererMode::USE_DEPTH*/)
{
	std::vector<Vertex_PCU> vertices;
	AddVertsForSphere3D(vertices, center, radius);
	g_debugSpheres.push_back(DebugObject(vertices, duration, startColor, endColor, mode, Vec3(0.f, 0.f, 0.f)));

}

void DebugAddWorldArrow(const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRendererMode mode /*= DebugRendererMode::USE_DEPTH*/)
{
	std::vector<Vertex_PCU> verticesCylinder;
	std::vector<Vertex_PCU> verticesCone;
	Vec3 cylinderEndConeStart = (start + end) / 2;
	AddVertsForCylinder3D(verticesCylinder, start, cylinderEndConeStart, radius, startColor);
	AddVertsForCone3D(verticesCone, cylinderEndConeStart, end, radius+0.1f, startColor);
	g_debugArrows.push_back(DebugObject(verticesCylinder, duration, startColor, endColor, mode, Vec3(0.f, 0.f, 0.f)));
	g_debugArrows.push_back(DebugObject(verticesCone, duration, startColor, endColor, mode, Vec3(0.f, 0.f, 0.f)));
}

//void DebugAddWorldText(const std::string& text, const Mat44& transform, float textHeight, const Vec2& alignment, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRendererMode mode /*= DebugRendererMode::USE_DEPTH*/)
//{
//	
//}

void DebugAddWorldBillboardText(const std::string& text, const Vec3& origin, float textHeight, const Vec2& alignment, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRendererMode mode /*= DebugRendererMode::USE_DEPTH*/)
{
	std::vector<Vertex_PCU> vertices;
	g_theFont->AddvertsForText3DAtOriginXForward(vertices, textHeight, text, startColor, 1.0f, alignment);
	g_debugBillboardeds.push_back(DebugObject(vertices, duration, startColor, endColor, mode, origin));
}

//void DebugAddWorldBasis(const Mat44& transform, float duration, DebugRendererMode mode /*= DebugRendererMode::USE_DEPTH*/)
//{
//
//}

void DebugAddScreenText(const std::string& text, const Vec2& position, float size, const Vec2& alignment, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/)
{
	UNUSED(alignment);
	std::vector<Vertex_PCU> vertices;
	g_theFont->AddVertsForText2D(vertices, position, size, text, startColor, 1.f);
	g_debugScreenTexts.push_back(DebugObject(vertices, duration, startColor, endColor, DebugRendererMode::USE_DEPTH, Vec3(position.x, position.y, 0.f)));
}


//void DebugAddMessage(const std::string& text, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/)
//{
//	
//}

bool Command_DebugRenderClear(EventArgs& args)
{
	UNUSED(args);
	DebugRenderClear();
	g_theConsole->Command_Clear(args);
	return true;
}

bool Command_DebugRenderToggle(EventArgs& args)
{
	UNUSED(args);
	m_debugDraw = !m_debugDraw;
	return true;
}
