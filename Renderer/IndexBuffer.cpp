#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

IndexBuffer::IndexBuffer(unsigned int size)
	:m_size(size)
{

}

IndexBuffer::~IndexBuffer()
{
	DX_SAFE_RELEASE(m_buffer);
}

unsigned int IndexBuffer::GetSize()
{
	return m_size;
}

