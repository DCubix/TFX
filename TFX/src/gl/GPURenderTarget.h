#ifndef TFX_GL_GPURENDERTARGET_H
#define TFX_GL_GPURENDERTARGET_H

#include "GPUTexture.h"
#include "../core/Util.h"

namespace tfx {
	class GPURenderTarget {
	public:
		enum BindMode {
			ReadWrite = 0,
			Read,
			Write
		};

		GPURenderTarget(
			u32 width, u32 height,
			GPUTexture::GPUTextureType attachmentType = GPUTexture::RGBA,
			bool mips = false
		);
		~GPURenderTarget();

		void bind(BindMode bindMode = BindMode::ReadWrite);
		void unbind();

		u32 getWidth() const { return m_width; }
		u32 getHeight() const { return m_height; }
		u32 getBindCode() const { return m_bindCode; }

		GPUTexture* getTexture() { return m_attachment; }

	private:
		u32 m_bindCode;
		u32 m_width, m_height;
		BindMode m_tmp_bindMode;
		i32 m_vp[4];

		GPUTexture* m_attachment;
		GPUTexture::GPUTextureType m_savedAttachmentType;
		bool m_generateMipmaps;
	};
}

#endif // TFX_GL_GPURENDERTARGET_H