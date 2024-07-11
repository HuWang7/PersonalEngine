#include "Engine/Renderer/ConstantBuffer.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include "Engine/Renderer/Renderer.hpp"

ConstantBuffer::ConstantBuffer(size_t size)
	:m_size(size)
{

}

ConstantBuffer::~ConstantBuffer()
{
	DX_SAFE_RELEASE(m_buffer);
}

