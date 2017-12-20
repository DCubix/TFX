#ifndef TFX_GL_GPUTEXTURE_H
#define TFX_GL_GPUTEXTURE_H

#include "../core/Util.h"

namespace tfx {
	class GPUTexture {
	public:
		enum GPUTextureType {
			RGB,
			RGBA,
			RGBf,
			RGBAf,
			Mono
		};
		GPUTexture(u32 width, u32 height,
			GPUTextureType textureType = GPUTextureType::RGBA,
			bool clamp = false,
			bool mips = true,
			byte* tdata = nullptr);
		GPUTexture(byte* fileData, u32 length);
		GPUTexture(String fileName, bool clamp = false, bool mips = false);

		u32 getWidth() const { return m_width; }
		u32 getHeight() const { return m_height; }
		u32 getBindCode() const { return m_bindCode; }
		u32 getTarget() const { return m_target; }

		void bind(i32 slot = 0);
		void unbind();
		
		void setFilter(int min, int mag);

		void generateMipmaps();

	private:
		void init_dds(byte* data, u32 len);

		u32 m_target;
		u32 m_bindCode;
		u32 m_width, m_height;
	};
}

#endif // TFX_GL_GPUTEXTURE_H