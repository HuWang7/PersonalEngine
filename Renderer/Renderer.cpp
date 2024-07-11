#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include "Engine/Renderer/Window.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Renderer/DefaultShader.hpp"

extern Window*		 g_theWindow;

#if defined(_DEBUG)
#define ENGINE_DEBUG_RENDER
#endif

#if defined(ENGINE_DEBUG_RENDER)
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif


Renderer::Renderer(RenderConfig const& config)
	:m_config( config )
{

}

Renderer::~Renderer()
{

}

void Renderer::Startup()
{
	// Render startup
	unsigned int deviceFlags = 0;
#if defined(ENGINE_DEBUG_RENDER)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create debug module
#if defined(ENGINE_DEBUG_RENDER)
	m_dxgiDebugModule = (void*)::LoadLibraryA("dxgidebug.dll");
	if (m_dxgiDebugModule == nullptr)
	{
		ERROR_AND_DIE("Could not load dxgidebug.dll.");
	}
	typedef HRESULT(WINAPI* GetDebugModleCB)(REFIID, void**);
	((GetDebugModleCB)::GetProcAddress((HMODULE)m_dxgiDebugModule, "DXGIGetDebugInterface"))
		(__uuidof(IDXGIDebug), &m_dxgiDebug);

	if (m_dxgiDebug == nullptr)
	{
		ERROR_AND_DIE("Could not load debug module.");
	}
#endif

	// Create device and swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferDesc.Width = g_theWindow->GetClientDimensions().x;
	swapChainDesc.BufferDesc.Height = g_theWindow->GetClientDimensions().y;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = (HWND)g_theWindow->GetHwnd();
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD ;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags,
		nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
		&m_swapChain, &m_device, nullptr, &m_deviceContext);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create D3D 11 device and swap chain.");
	}

	ID3D11Texture2D* backBuffer;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not get swap chain buffer.");
	}

	hr = m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could create render target view for swap chain buffer.");
	}

	backBuffer->Release();	

	// Create and bind the "Default" shader

	m_currentShader = CreateShader("Default", shaderSource);

	m_defaultShader = m_currentShader;
	BindShader(m_currentShader);

	//m_diffuseShader = CreateShader("Diffuse", VertexType::Vertex_PCUTBN);
	//BindShader(m_diffuseShader);

	// Create vertex buffer
	m_immediateVBO = CreateVertexBuffer(sizeof(Vertex_PCU));
	m_immediateVBO_VertexPCUTBN = CreateVertexBuffer(sizeof(Vertex_PCUTBN));
	// Create index buffer
	m_immediateIBO = CreateIndexBuffer(sizeof(16));

	// Set rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc = { };
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.AntialiasedLineEnable = true;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(int)(RasterizerMode::SOLID_CULL_NONE)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create rasterizer state.");
	}

	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(int)(RasterizerMode::SOLID_CULL_BACK)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create rasterizer state.");
	}

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(int)(RasterizerMode::WIREFRAME_CULL_NONE)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create rasterizer state.");
	}

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(int)(RasterizerMode::WIREFRAME_CULL_BACK)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create rasterizer state.");
	}

	BindVertexBuffer(m_immediateVBO, VertexType::Vertex_PCU);
	BindIndexBuffer(m_immediateIBO);

	size_t cameraConstantsSize = sizeof(CameraConstants);
	m_cameraCBO = CreateConstantBuffer(cameraConstantsSize);

	size_t modelCboSize = sizeof(ModelConstants);
	m_modelCBO = CreateConstantBuffer(modelCboSize);

	size_t lightCboSize = sizeof(LightConstants);
	m_lightCBO = CreateConstantBuffer(lightCboSize);

	D3D11_BLEND_DESC blendDesc = { };
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)(BlendMode::OPAQUE)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateBlendState for BlendMode::OPAQUE failed.");
	}

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)BlendMode::ALPHA]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("CreateBlendState for BlendMode::ALPHA failed.");
	}

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)BlendMode::ADDITIVE]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("CreateBlendState for BlendMode::ADDITIVE failed.");
	}

	Image defaultImage(IntVec2(2, 2), Rgba8::WHITE);
	Texture* whiteTexture = CreateTextureFromImage(defaultImage);
	whiteTexture->m_name = "Default";
	m_defaultTexture = whiteTexture;
	BindTexture(m_defaultTexture);

	// Point Clamp Sampler State
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerStates[(int)SamplerMode::POINT_CLAMP]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateSamplerState for SamplerMode::POINT_CLAMP failed.");
	}

	// Bilinear Wrap Sampler State
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerStates[static_cast<int>(SamplerMode::BILINEAR_WRAP)]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("CreateSamplerState for SamplerMode::BILINEAR_WRAP failed.");
	}

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = m_config.m_window->GetClientDimensions().x;
	textureDesc.Height = m_config.m_window->GetClientDimensions().y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.SampleDesc.Count = 1;

	hr = m_device->CreateTexture2D(&textureDesc, nullptr, &m_depthStencilTexture);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Cound create texture for depth stencil.");
	}

	hr = m_device->CreateDepthStencilView(m_depthStencilTexture, nullptr, &m_depthStencilView);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could create depth stencil view.");
	}

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStates[(int)(DepthMode::DISABLED)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateDepthStencilState for DepthMode::DISABLED failed.");
	}

	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStates[(int)(DepthMode::ENABLED)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateDepthStencilState for DepthMode::ENABLED failed.");
	}

	SetModelConstants();
}

void Renderer::BeginFrame()
{
	// Set render target
	//m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

}

void Renderer::EndFrame()
{
	// Present
	HRESULT hr;
	hr = m_swapChain->Present(0, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		ERROR_AND_DIE("Device has been lost, application will now terminate.");
	}
}

void Renderer::Shutdown()
{
	for (auto shader : m_loadedShaders)
	{
		delete shader; // Delete the Shader object
		shader = nullptr;
	}

	for (auto texture : m_loadedTextures)
	{
		if (texture == nullptr)
		{
			continue;
		}
		else
		{
			delete texture;
			texture = nullptr;
		}
	}

	for (auto bitmapFont : m_loadedFonts) {
		delete bitmapFont;
	}

	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	delete m_immediateVBO_VertexPCUTBN;
	m_immediateVBO_VertexPCUTBN = nullptr;

	delete m_immediateIBO;
	m_immediateIBO = nullptr;

	delete m_cameraCBO;
	m_cameraCBO = nullptr;

	delete m_modelCBO;
	m_modelCBO = nullptr;

	delete m_lightCBO;
	m_lightCBO = nullptr;

	m_loadedShaders.clear();

	// Report error leaks and release debug module


	for (int i = 0; i < (int)BlendMode::COUNT; i++) {
		DX_SAFE_RELEASE(m_blendStates[i]);
	}
	for (int i = 0; i < (int)SamplerMode::COUNT; i++) {
		DX_SAFE_RELEASE(m_samplerStates[i]);
	}
	for (int i = 0; i < (int)(RasterizerMode::COUNT); ++i) {
		DX_SAFE_RELEASE(m_rasterizerStates[i]);
	}
	for (int i = 0; i < (int)(DepthMode::COUNT); ++i) {
		DX_SAFE_RELEASE(m_depthStencilStates[i]);
	}

	DX_SAFE_RELEASE(m_renderTargetView);
	DX_SAFE_RELEASE(m_swapChain);
	DX_SAFE_RELEASE(m_deviceContext);
	DX_SAFE_RELEASE(m_device);
	DX_SAFE_RELEASE(m_depthStencilView);
	DX_SAFE_RELEASE(m_depthStencilTexture);

#if defined(ENGINE_DEBUG_RENDER)
	((IDXGIDebug*)m_dxgiDebug)->ReportLiveObjects(
		DXGI_DEBUG_ALL,
		(DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
	);

	((IDXGIDebug*)m_dxgiDebug)->Release();
	m_dxgiDebug = nullptr;

	::FreeLibrary((HMODULE)m_dxgiDebugModule);
	m_dxgiDebugModule = nullptr;
#endif
}

void Renderer::ClearScreen(const Rgba8 clearColor)
{
	float colorAsFloats[4];
	clearColor.GetAsFloats(colorAsFloats);
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, colorAsFloats);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::BeginCamera(Camera& camera)
{
	if (camera.m_viewport.m_mins == Vec2::ZERO && camera.m_viewport.m_maxs == Vec2::ZERO)
	{
		camera.m_viewport.m_maxs.x = (float)m_config.m_window->GetClientDimensions().x;
		camera.m_viewport.m_maxs.y = (float)m_config.m_window->GetClientDimensions().y;
	}

	float viewportWidth = camera.m_viewport.m_maxs.x - camera.m_viewport.m_mins.x;
	float viewportHeight = camera.m_viewport.m_maxs.y - camera.m_viewport.m_mins.y;

	// Set viewport
	D3D11_VIEWPORT viewport = { 0 };
	viewport.TopLeftX = camera.m_viewport.m_mins.x;
	viewport.TopLeftY = camera.m_viewport.m_mins.y;
	viewport.Width = viewportWidth;
	viewport.Height = viewportHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	if (m_deviceContext)
	{
		m_deviceContext->RSSetViewports(1, &viewport);
	}


	CameraConstants cameraConstants;

	cameraConstants.ProjectionMatrix = camera.GetProjectionMatrix();
	cameraConstants.ViewMatrix = camera.GetViewMatrix();

	CopyCPUToGPU(&cameraConstants, sizeof(CameraConstants), m_cameraCBO);
	BindConstantBuffer(k_cameraConstantsSlot, m_cameraCBO);
}

void Renderer::EndCamera(const Camera& camera)
{
	UNUSED(camera);
}

void Renderer::DrawVertexArray(int numVertexes, const Vertex_PCU* Vertexes)
{
	CopyCPUToGPU(Vertexes, numVertexes * sizeof(Vertex_PCU), m_immediateVBO);
	SetStatesIfChanges();
	DrawVertexBuffer(m_immediateVBO, numVertexes, VertexType::Vertex_PCU);
}

void Renderer::DrawVertexArray(int numVertexes, const Vertex_PCUTBN* Vertexes, int numIndices, const unsigned int* indices)
{
	CopyCPUToGPU(Vertexes, numVertexes * sizeof(Vertex_PCUTBN), m_immediateVBO);
	CopyCPUToGPU(indices, numIndices * sizeof(unsigned int), m_immediateIBO);
	SetStatesIfChanges();
	DrawIndexBuffer(m_immediateIBO, numIndices, VertexType::Vertex_PCUTBN, m_immediateVBO);
}

void Renderer::DrawVertexArray(int numVertexes, Vertex_PCUTBN const* vertexes)
{
	CopyCPUToGPU(vertexes, (size_t)numVertexes * sizeof(Vertex_PCUTBN), m_immediateVBO_VertexPCUTBN);
	SetStatesIfChanges();
	DrawVertexBuffer(m_immediateVBO_VertexPCUTBN, numVertexes, VertexType::Vertex_PCUTBN);
}

Texture* Renderer::GetTextureForFileName(char const* imageFilePath)
{
	for (int i = 0; i < m_loadedTextures.size(); ++i)
	{
		if (m_loadedTextures[i] != nullptr && !strcmp(m_loadedTextures[i]->m_name.c_str(), imageFilePath))
		{
			return m_loadedTextures[i];
		}
	}
	return nullptr;
}

Texture* Renderer::CreateOrGetTextureFromFile(char const* imageFilePath)
{
	// See if we already have this texture previously loaded
	Texture* existingTexture = GetTextureForFileName(imageFilePath);
	if (existingTexture)
	{
		return existingTexture;
	}

	// Never seen this texture before!  Let's load it.
	Texture* newTexture = CreateTextureFromFile(imageFilePath);
	return newTexture;
}


Texture* Renderer::CreateTextureFromFile(char const* imageFilePath)
{
	Image image(imageFilePath);
	return CreateTextureFromImage(image);
}

Texture* Renderer::CreateTextureFromImage(const Image& image)
{
	// Check if texture already loaded
	Texture* existingTexture = GetTextureForFileName(image.GetImageFilePath().c_str());
	if (existingTexture) {
		return existingTexture;
	}

	// Prepare the texture description
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = image.GetDimensions().x;
	textureDesc.Height = image.GetDimensions().y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // Assuming the image is in RGBA format
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	// Prepare the subresource data
	D3D11_SUBRESOURCE_DATA textureData;
	textureData.pSysMem = image.GetRawData();
	textureData.SysMemPitch = 4 * image.GetDimensions().x;

	// Create the texture
	Texture* newTexture = new Texture();
	newTexture->m_name = image.GetImageFilePath();
	HRESULT hr = m_device->CreateTexture2D(&textureDesc, &textureData, &newTexture->m_texture);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE(Stringf("CreateTextureFromImage failed for image file \"%s\".", image.GetImageFilePath().c_str()));
	}

	// Create shader resource view
	hr = m_device->CreateShaderResourceView(newTexture->m_texture, NULL, &newTexture->m_shaderResourceView);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE(Stringf("CreateShaderResourceView failed for image file \"%s\".", image.GetImageFilePath().c_str()));
	}

	m_loadedTextures.push_back(newTexture);
	return newTexture;
	
}

Texture* Renderer::CreateTextureFromData(char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData)
{
	// Check if the load was successful
	GUARANTEE_OR_DIE(texelData, Stringf("CreateTextureFromData failed for \"%s\" - texelData was null!", name));
	GUARANTEE_OR_DIE(bytesPerTexel >= 3 && bytesPerTexel <= 4, Stringf("CreateTextureFromData failed for \"%s\" - unsupported BPP=%i (must be 3 or 4)", name, bytesPerTexel));
	GUARANTEE_OR_DIE(dimensions.x > 0 && dimensions.y > 0, Stringf("CreateTextureFromData failed for \"%s\" - illegal texture dimensions (%i x %i)", name, dimensions.x, dimensions.y));

	Texture* newTexture = new Texture();
	newTexture->m_name = name; // NOTE: m_name must be a std::string, otherwise it may point to temporary data!
	newTexture->m_dimensions = dimensions;

	m_loadedTextures.push_back(newTexture);
	return newTexture;
}


BitmapFont* Renderer::CreateOrGetBitmapFont(const char* bitmapFontFilePathWithNoExtension) {
	// Check if the font has already been loaded
	for (BitmapFont* font : m_loadedFonts) {
		if (font != nullptr && !strcmp(font->m_fontFilePathNameWithNoExtension.c_str(), bitmapFontFilePathWithNoExtension)) {
			return font; // Font already loaded
		}
	}

	// Font not found, so create and load a new one
	BitmapFont* newFont = CreateBitmapFontFromFile(bitmapFontFilePathWithNoExtension);
	if (newFont != nullptr) {
		m_loadedFonts.push_back(newFont);
	}
	return newFont;
}

BitmapFont* Renderer::CreateBitmapFontFromFile(const char* bitmapFontFilePathWithNoExtension) {
	// Load the font texture
	std::string fontTexturePath = std::string(bitmapFontFilePathWithNoExtension) + ".png"; // Assuming the font texture is in a .png file
	Texture* fontTexture = CreateOrGetTextureFromFile(fontTexturePath.c_str());

	// Check if the texture loading was successful
	if (fontTexture == nullptr) {
		return nullptr; // Could not load the font texture
	}

	// Create a new BitmapFont instance
	BitmapFont* newFont = new BitmapFont(bitmapFontFilePathWithNoExtension, *fontTexture);

	return newFont;
}


void Renderer::BindTexture(const Texture* texture)
{
	// Use the default texture if nullptr is passed
	if (texture == nullptr) {
		texture = m_defaultTexture;
	}

	// Bind the texture
	if (texture && texture->m_shaderResourceView) {
		m_deviceContext->PSSetShaderResources(0, 1, &texture->m_shaderResourceView);
	}
}

void Renderer::SetBlendMode(BlendMode blendMode)
{
	m_desiredBlendMode = blendMode;
}

Shader* Renderer::CreateShader(char const* shaderName, char const* shader_Source,  VertexType type /*= VertexType::Vertex_PCU*/)
{
	ShaderConfig shaderConfig;
	shaderConfig.m_name = shaderName;
	Shader* shader = new Shader(shaderConfig);

	//Compile vertex shader
	std::vector<unsigned char> vertexShaderByteCode;
	if (!CompileShaderToByteCode(vertexShaderByteCode, "VertexShader", shader_Source, "VertexMain", "vs_5_0"))
	{
		// Handle error
		delete shader;
		return nullptr;
	}

	// Create vertex shader
	HRESULT hr = m_device->CreateVertexShader(
		vertexShaderByteCode.data(),
		vertexShaderByteCode.size(),
		NULL, &shader->m_vertexShader
	);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("Could not create vertex shader."));
	}

	//Compile pixel shader
	std::vector<unsigned char> pixelShaderByteCode;
	if (!CompileShaderToByteCode(pixelShaderByteCode, "PixelShader", shader_Source, "PixelMain", "ps_5_0"))
	{
		// Handle error
		shader->m_vertexShader->Release();
		delete shader;
		return nullptr;
	}

	// Create pixel shader
	hr = m_device->CreatePixelShader(
		pixelShaderByteCode.data(), 
		pixelShaderByteCode.size(),
		NULL, &shader->m_pixelShader
	);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("Could not create pixel shader."));
	}

	// Create input layout
	/*D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};*/

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc;
	inputElementDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	inputElementDesc.push_back({ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	inputElementDesc.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });

	if (type == VertexType::Vertex_PCUTBN) {
		inputElementDesc.push_back({ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		inputElementDesc.push_back({ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		inputElementDesc.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	}

	/*UINT numElements = ARRAYSIZE(inputElementDesc);
	hr = m_device->CreateInputLayout(
		inputElementDesc, numElements,
		vertexShaderByteCode.data(),
		vertexShaderByteCode.size(),
		&shader->m_inputLayout
	);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create vertex layout.");
	}*/

	hr = m_device->CreateInputLayout(
		inputElementDesc.data(),
		static_cast<UINT>(inputElementDesc.size()),
		vertexShaderByteCode.data(),
		vertexShaderByteCode.size(),
		&shader->m_inputLayout
	);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create vertex layout.");
	}

	m_loadedShaders.push_back(shader);
	return shader;
}

Shader* Renderer::CreateShader(char const* shaderName, VertexType type)
{
	std::string filename = std::string(shaderName) + ".hlsl";
	std::string shaderSourceFromFile;
	int result = FileReadToString(shaderSourceFromFile, "./Data/Shaders/" + filename);
	if (result <= 0) {
		std::cerr << "Failed to read shader file: " << filename << std::endl;
		return nullptr; // Return null if file reading fails
	}
	Shader* resultShader = CreateShader(shaderName, shaderSourceFromFile.c_str(), type);
	return resultShader;
}

bool Renderer::CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target)
{
	// Compile vertex shader
	DWORD shaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined(ENGINE_DEBUG_RENDER)
	shaderFlags = D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ID3DBlob* shaderBlob = NULL;
	ID3DBlob* errorBlob = NULL;

	HRESULT hr = D3DCompile(
		source, strlen(source),
		name, nullptr, nullptr,
		entryPoint, target, shaderFlags, 0,
		&shaderBlob, &errorBlob
	);
	if (SUCCEEDED(hr))
	{
		outByteCode.resize(shaderBlob->GetBufferSize());
		memcpy(
			outByteCode.data(),
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize()
		);
	}
	else
	{
		if (errorBlob != NULL)
		{
			DebuggerPrintf((char*)errorBlob->GetBufferPointer());
		}
		ERROR_AND_DIE(Stringf("Could not compile vertex shader."));
	}

	shaderBlob->Release();
	if (errorBlob != NULL)
	{
		errorBlob->Release();
	}
	return true;
}

void Renderer::BindShader(Shader* shader)
{
	if (shader == nullptr)
	{
		m_deviceContext->VSSetShader(m_defaultShader->m_vertexShader, nullptr, 0);
		m_deviceContext->PSSetShader(m_defaultShader->m_pixelShader, nullptr, 0);
		m_deviceContext->IASetInputLayout(m_defaultShader->m_inputLayout);
		m_currentShader = m_defaultShader; 
		return;
	}
	m_deviceContext->VSSetShader(shader->m_vertexShader, nullptr, 0);
	m_deviceContext->PSSetShader(shader->m_pixelShader, nullptr, 0);
	m_deviceContext->IASetInputLayout(shader->m_inputLayout);
}

VertexBuffer* Renderer::CreateVertexBuffer(const size_t size)
{
	VertexBuffer* vertexBuffer = new VertexBuffer(size);
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = (UINT)size;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = m_device->CreateBuffer(&bufferDesc, nullptr, &vertexBuffer->m_buffer);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create vertex buffer.");
	}
	return vertexBuffer;
}

void Renderer::CopyCPUToGPU(const void* data, size_t size, VertexBuffer*& vbo)
{
	// Check if the vertex buffer is large enough for the new data
	if (vbo->m_size < size)
	{
		delete vbo;
		vbo = CreateVertexBuffer(size);
	}

	// Copy vertices
	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(vbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(vbo->m_buffer, 0);
}

void Renderer::BindVertexBuffer(VertexBuffer* vbo, VertexType type)
{
	UINT stride;
	if (type == VertexType::Vertex_PCU) {
		stride = sizeof(Vertex_PCU);
	}
	else if  (type == VertexType::Vertex_PCUTBN) 
	{
		stride = sizeof(Vertex_PCUTBN);
	}
	UINT startOffset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, &vbo->m_buffer, &stride, &startOffset);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::DrawVertexBuffer(VertexBuffer* vbo, int vertexCount, VertexType type, int vertexOffset /*= 0*/)
{
	BindVertexBuffer(vbo, type);
	// Draw
	m_deviceContext->Draw(vertexCount, vertexOffset);
}

IndexBuffer* Renderer::CreateIndexBuffer(const size_t size)
{
	IndexBuffer* indexBuffer = new IndexBuffer((unsigned int)size);
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = (UINT)size;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = m_device->CreateBuffer(&bufferDesc, nullptr, &indexBuffer->m_buffer);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create vertex buffer.");
	}
	return indexBuffer;
}

void Renderer::CopyCPUToGPU(const void* data, size_t size, IndexBuffer*& ibo)
{
	// Check if the index buffer is large enough for the new data
	if (ibo->m_size < size)
	{
		delete ibo;
		ibo = CreateIndexBuffer(size);
	}

	// Copy vertices
	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(ibo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(ibo->m_buffer, 0);
}

void Renderer::BindIndexBuffer(IndexBuffer* ibo)
{
	m_deviceContext->IASetIndexBuffer(ibo->m_buffer, DXGI_FORMAT_R32_UINT, 0);
}

void Renderer::DrawIndexBuffer(IndexBuffer* ibo, int indexCount, VertexType type, VertexBuffer* vbo)
{
	BindIndexBuffer(ibo);
	BindVertexBuffer(vbo, type);
	// Draw
	m_deviceContext->DrawIndexed(indexCount, 0, 0);
}

ConstantBuffer* Renderer::CreateConstantBuffer(const size_t size)
{
	ConstantBuffer* constantBuffer = new ConstantBuffer(size);
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = (UINT)size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = m_device->CreateBuffer(&bufferDesc, nullptr, &constantBuffer->m_buffer);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create vertex buffer.");
	}
	return constantBuffer;
}

void Renderer::CopyCPUToGPU(const void* data, size_t size, ConstantBuffer*& cbo)
{
	// Copy vertices
	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(cbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(cbo->m_buffer, 0);
}

void Renderer::BindConstantBuffer(int slot, ConstantBuffer* cbo)
{
	m_deviceContext->VSSetConstantBuffers(slot, 1, &cbo->m_buffer);
	m_deviceContext->PSSetConstantBuffers(slot, 1, &cbo->m_buffer);
}

void Renderer::SetStatesIfChanges()
{
	if (m_blendStates[(int)m_desiredBlendMode] != m_blendState)
	{
		m_blendState = m_blendStates[(int)m_desiredBlendMode];
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		UINT sampleMask = 0xffffffff;
		m_deviceContext->OMSetBlendState(m_blendState, blendFactor, sampleMask);
	}

	if (m_samplerState != m_samplerStates[(int)(m_desiredSamplerMode)]) {
		m_samplerState = m_samplerStates[(int)(m_desiredSamplerMode)];
		m_deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	}

	if (m_rasterizerState != m_rasterizerStates[(int)(m_desiredRasterizerMode)])
	{
		m_rasterizerState = m_rasterizerStates[(int)(m_desiredRasterizerMode)];
		m_deviceContext->RSSetState(m_rasterizerState);
	}

	if (m_depthStencilState != m_depthStencilStates[(int)(m_desiredDepthMode)])
	{
		m_depthStencilState = m_depthStencilStates[(int)(m_desiredDepthMode)];
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
	}
}

void Renderer::SetSamplerMode(SamplerMode samplerMode)
{
	m_desiredSamplerMode = samplerMode;
}

void Renderer::SetRasterizerMode(RasterizerMode mode)
{
	m_desiredRasterizerMode = mode;
}

void Renderer::SetDepthMode(DepthMode mode)
{
	m_desiredDepthMode = mode;
}

void Renderer::SetModelConstants(const Mat44& modelMatrix /*= Mat44()*/, const Rgba8& modelColor /*= Rgba8::WHITE*/)
{
	ModelConstants matConstants;
	matConstants.ModelMatrix = modelMatrix;
	modelColor.GetAsFloats(matConstants.ModelColor);
	CopyCPUToGPU(&matConstants, m_modelCBO->m_size, m_modelCBO);
	BindConstantBuffer(3, m_modelCBO);
}

void Renderer::SetLightConstants(Vec3 sunDirection /*= Vec3(2, 1, -1)*/, float sunIntensity /*= 0.85f*/, float ambientIntensity /*= 0.35f*/)
{
	LightConstants lightConstants;
	lightConstants.SunDirection = sunDirection;
	lightConstants.SunIntensity = sunIntensity;
	lightConstants.AmbientIntensity = ambientIntensity;
	CopyCPUToGPU(&lightConstants, m_lightCBO->m_size, m_lightCBO);
	BindConstantBuffer(1, m_lightCBO);
}

