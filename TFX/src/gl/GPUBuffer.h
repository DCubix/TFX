#ifndef TFX_GL_BUFFER_H
#define TFX_GL_BUFFER_H

#include "glad.h"
#include "../core/Util.h"

namespace tfx {
	class GPUBuffer {
	public:
		enum GPUBufferUsage {
			Static = GL_STATIC_DRAW,
			Dynamic = GL_DYNAMIC_DRAW
		};

		enum GPUBufferType {
			DataArray = GL_ARRAY_BUFFER,
			ElementArray = GL_ELEMENT_ARRAY_BUFFER
		};

		GPUBuffer(GPUBufferType bufferType);
		GPUBuffer(GPUBufferType bufferType, GPUBufferUsage bufferUsage);
		
		~GPUBuffer();

		template <typename D>
		void setData(D* data, u32 dataLength, u32 dataOffset = 0) {
			glBindBuffer(m_bufferType, m_bindCode);
			if (dataLength > m_dataSize) {
				glBufferData(m_bufferType, dataLength * sizeof(D), data, m_bufferUsage);
				m_dataSize = dataLength;
			} else {
				if (m_bufferUsage != GPUBufferUsage::Static)
					glBufferSubData(m_bufferType, dataOffset, dataLength * sizeof(D), data);
			}
		}

		void bind();
		void unbind();

		u32 getBindCode() const { return m_bindCode; }

	private:
		u32 m_bindCode;
		GPUBufferType m_bufferType;
		GPUBufferUsage m_bufferUsage;
		u32 m_dataSize;
	};
}

#endif // TFX_GL_BUFFER_H