#ifndef TFX_GL_GPUFRAMEBUFFER_H
#define TFX_GL_GPUFRAMEBUFFER_H

#include "GPUTexture.h"
#include "../core/Util.h"

namespace tfx {
	class GPUFramebuffer {
	public:
		GPUFramebuffer();
		~GPUFramebuffer();

		void addColorAttachment(GPUTexture* texture);
		void withDepth();

		void bind();
		void unbind();

		u32 getBindCode() const { return m_bindCode; }

	private:
		u32 m_bindCode, m_rbo;
		u32 m_attachments;
		u32 m_width, m_height;
		i32 m_vp[4];
	};
}

#endif //TFX_GL_GPUFRAMEBUFFER_H