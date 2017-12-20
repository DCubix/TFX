#include "GPUVertexArray.h"

namespace tfx {
	GPUVertexArray::GPUVertexArray() : m_offset(0) {
		glGenVertexArrays(1, &m_bindCode);
	}

	GPUVertexArray::~GPUVertexArray() {
		if (m_bindCode) glDeleteVertexArrays(1, &m_bindCode);
	}

	void GPUVertexArray::bindAttribute(
		u32 location,
		GPUAttributeType attributeType,
		GPUBuffer* buffer,
		u32 stride,
		bool normalized)
	{
		glBindVertexArray(m_bindCode);
		if (buffer != nullptr) {
			glBindBuffer(GL_ARRAY_BUFFER, buffer->getBindCode());
		}
		glEnableVertexAttribArray(location);
		glVertexAttribPointer(
			location,
			attributeType,
			GL_FLOAT,
			normalized ? GL_TRUE : GL_FALSE,
			stride,
			OFF(m_offset)
		);

		m_offset += (u32) attributeType * sizeof(float);
	}

	void GPUVertexArray::bindElement(GPUBuffer* buffer) {
		glBindVertexArray(m_bindCode);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->getBindCode());
	}

	void GPUVertexArray::bind() {
		glBindVertexArray(m_bindCode);
	}

	void GPUVertexArray::unbind() {
		glBindVertexArray(0);
	}

}