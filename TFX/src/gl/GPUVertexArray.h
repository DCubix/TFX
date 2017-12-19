#ifndef TFX_GPU_VERTEX_ARRAY_H
#define TFX_GPU_VERTEX_ARRAY_H

#include "GPUBuffer.h"

namespace tfx {
	class GPUVertexArray {
	public:
		enum GPUAttributeType {
			Float = 1,
			Float2,
			Float3,
			Float4
		};

		GPUVertexArray();
		~GPUVertexArray();

		void bindAttribute(u32 location,
						   GPUAttributeType attributeType,
						   GPUBuffer* buffer,
						   u32 stride,
						   bool normalized = false);

		void bindElement(GPUBuffer* buffer);

		void bind();
		void unbind();

	private:
		u32 m_bindCode;
		u32 m_offset;
	};
}

#endif // TFX_GPU_VERTEX_ARRAY_H