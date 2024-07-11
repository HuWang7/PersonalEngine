#include "Engine/Renderer/Texture.hpp"
#include <D3D11.h>
#include "Engine/Renderer/Renderer.hpp"

Texture::Texture()
{

}

Texture::~Texture()
{
	DX_SAFE_RELEASE(m_texture);
	DX_SAFE_RELEASE(m_shaderResourceView);
}
