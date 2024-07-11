#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/Window.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Camera.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Core/Image.hpp"
#include <vector>

struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11BlendState;
struct ID3D11SamplerState;
struct ID3D11DepthStencilView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilState;

enum class SamplerMode
{
	POINT_CLAMP,
	BILINEAR_WRAP,
	COUNT
};

struct CameraConstants
{
	Mat44 ViewMatrix;
	Mat44 ProjectionMatrix;
};

struct ModelConstants {
	Mat44 ModelMatrix;
	float ModelColor[4] = {1.f,1.f,1.f,1.f};
};

struct LightConstants
{
	Vec3 SunDirection;
	float SunIntensity;
	float AmbientIntensity;
	Vec3 memoryHolder;
};

static const int k_cameraConstantsSlot = 2;

#define DX_SAFE_RELEASE(dxObject)	\
{									\
	if ((dxObject) != nullptr)		\
	{								\
		(dxObject)->Release();		\
		(dxObject) = nullptr;		\
	}								\
}									

#if defined(OPAQUE)
#undef OPAQUE
#endif

enum class BlendMode
{
	ALPHA,
	ADDITIVE,
	OPAQUE,
	COUNT,
};

enum class RasterizerMode
{
	SOLID_CULL_NONE,
	SOLID_CULL_BACK,
	WIREFRAME_CULL_NONE,
	WIREFRAME_CULL_BACK,
	COUNT
};

enum class DepthMode
{
	DISABLED,
	ENABLED,
	COUNT
};

enum class VertexType {
	Vertex_PCU,
	Vertex_PCUTBN
};

struct RenderConfig
{
	Window* m_window = nullptr;
};


class Renderer
{

public:
	Renderer(RenderConfig const& config);
	~Renderer();


	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void ClearScreen( const Rgba8 clearColor );
	void BeginCamera( Camera& camera );
	void EndCamera( const Camera& camera );
	void DrawVertexArray(int numVertexes, const Vertex_PCU* Vertexes);
	void DrawVertexArray(int numVertexes, const Vertex_PCUTBN* Vertexes, int numIndices, const unsigned int* indices);
	void DrawVertexArray(int numVertexes, Vertex_PCUTBN const* vertexes);


	Texture* GetTextureForFileName(char const* imageFilePath);
	Texture* CreateOrGetTextureFromFile(char const* imageFilePath);
	Texture* CreateTextureFromFile(char const* imageFilePath);
	Texture* CreateTextureFromImage(const Image& image);
	Texture* CreateTextureFromData(char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData);
	BitmapFont* CreateOrGetBitmapFont(const char* bitmapFontFilePathWithNoExtension);
	BitmapFont* CreateBitmapFontFromFile(const char* bitmapFontFilePathWithNoExtension);
	void BindTexture(const Texture* texture);
	void SetBlendMode(BlendMode blendMode);
	Shader* CreateShader(char const* shaderName, char const* shaderSource, VertexType type = VertexType::Vertex_PCU);
	Shader* CreateShader(char const* shaderName, VertexType type = VertexType::Vertex_PCU);
	bool CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target);
	void BindShader(Shader* shader);

	VertexBuffer* CreateVertexBuffer(const size_t size);
	void CopyCPUToGPU(const void* data, size_t size, VertexBuffer*& vbo);
	void BindVertexBuffer(VertexBuffer* vbo, VertexType type);
	void DrawVertexBuffer(VertexBuffer* vbo, int vertexCount, VertexType type, int vertexOffset = 0);

	IndexBuffer* CreateIndexBuffer(const size_t size);
	void CopyCPUToGPU(const void* data, size_t size, IndexBuffer*& ibo);
	void BindIndexBuffer(IndexBuffer* ibo);
	void DrawIndexBuffer(IndexBuffer* ibo, int indexCount, VertexType type, VertexBuffer* vbo);

	ConstantBuffer* CreateConstantBuffer(const size_t size);
	void CopyCPUToGPU(const void* data, size_t size, ConstantBuffer*& cbo);
	void BindConstantBuffer(int slot, ConstantBuffer* cbo);
	void SetStatesIfChanges();
	void SetSamplerMode(SamplerMode samplerMode);
	void SetRasterizerMode(RasterizerMode mode);
	void SetDepthMode(DepthMode mode);
	void SetModelConstants(const Mat44& modelMatrix = Mat44(), const Rgba8& modelColor = Rgba8::WHITE);
	void SetLightConstants(Vec3 sunDirection = Vec3(2, 1, -1), float sunIntensity = 0.85f, float ambientIntensity = 0.35f);

	Shader*					m_defaultShader = nullptr;

protected:
	RenderConfig			m_config;
	void*					m_hglrc = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Device*			m_device = nullptr;
	ID3D11DeviceContext*	m_deviceContext = nullptr;
	IDXGISwapChain*			m_swapChain = nullptr;
	void*					m_dxgiDebugModule = nullptr;
	void*					m_dxgiDebug = nullptr;
	std::vector<Shader*>	m_loadedShaders;
	Shader*					m_currentShader = nullptr;
	VertexBuffer*			m_immediateVBO = nullptr;
	VertexBuffer*			m_immediateVBO_VertexPCUTBN = nullptr;
	IndexBuffer*			m_immediateIBO = nullptr;
	ConstantBuffer*			m_cameraCBO = nullptr;
	ConstantBuffer*			m_modelCBO = nullptr;
	ConstantBuffer*			m_lightCBO = nullptr;
	const Texture*			m_defaultTexture = nullptr;

	ID3D11BlendState*		m_blendState = nullptr;
	BlendMode				m_desiredBlendMode = BlendMode::ALPHA;
	ID3D11BlendState*		m_blendStates[(int)(BlendMode::COUNT)] = {};

	ID3D11SamplerState*		m_samplerState = nullptr;
	SamplerMode				m_desiredSamplerMode = SamplerMode::POINT_CLAMP;
	ID3D11SamplerState*		m_samplerStates[(int)(SamplerMode::COUNT)] = {};

	ID3D11RasterizerState*	m_rasterizerState = nullptr;
	RasterizerMode			m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	ID3D11RasterizerState*	m_rasterizerStates[(int)(RasterizerMode::COUNT)] = {};

	ID3D11DepthStencilView*		m_depthStencilView = nullptr;
	ID3D11Texture2D*			m_depthStencilTexture = nullptr;
	ID3D11DepthStencilState*	m_depthStencilState = nullptr;
	DepthMode					m_desiredDepthMode = DepthMode::DISABLED;
	ID3D11DepthStencilState*	m_depthStencilStates[(int)(DepthMode::COUNT)] = {};

	

private:
	std::vector<Texture*> m_loadedTextures;
	std::vector<BitmapFont*> m_loadedFonts;
};
