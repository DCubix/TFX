#include "GPUBuffer.h"

namespace tfx {
	GPUBuffer::GPUBuffer(GPUBufferType bufferType)
		: GPUBuffer(bufferType, GPUBufferUsage::Static) {}

	GPUBuffer::GPUBuffer(GPUBufferType bufferType, GPUBufferUsage bufferUsage)
		: m_bufferType(bufferType),
		  m_bufferUsage(bufferUsage),
		  m_dataSize(0)
	{
		glGenBuffers(1, &m_bindCode);
	}

	GPUBuffer::~GPUBuffer() {
		if (m_bindCode) glDeleteBuffers(1, &m_bindCode);
	}

	void GPUBuffer::bind() {
		glBindBuffer(m_bufferType, m_bindCode);
	}
	
	void GPUBuffer::unbind() {
		glBindBuffer(m_bufferType, 0);
	}
}