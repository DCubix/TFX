#include "GPUTexture.h"

#include "glad.h"
#include <assert.h>

#define GLIML_ASSERT assert
#define GLIML_NO_PVR
#define GLIML_NO_KTX
#include "gliml.h"

#include "../core/Logger.h"

namespace tfx {
	GPUTexture::GPUTexture(u32 width, u32 height, GPUTextureType textureType, bool clamp, bool mips, byte* tdata)
		: m_width(width), m_height(height)
	{
		glGenTextures(1, &m_bindCode);
		glBindTexture(GL_TEXTURE_2D, m_bindCode);

		if (mips) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		if (clamp) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		switch (textureType) {
			case GPUTextureType::RGB:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);
				break;
			case GPUTextureType::RGBA:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata);
				break;
			case GPUTextureType::RGBf:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
				break;
			case GPUTextureType::RGBAf:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
				break;
		}

		if (mips) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		m_target = GL_TEXTURE_2D;
	}

	GPUTexture::GPUTexture(byte* fileData, u32 length) {
		init_dds(fileData, length);
	}

	GPUTexture::GPUTexture(String fileName, bool clamp, bool mips) {
		if (fileName.endsWith(".dds")) {
			Vec<byte> tfile = Util::loadFile(fileName);
			init_dds(tfile.data(), tfile.size());
		} else {
			tpImage img = tpLoadPNG(fileName.str().c_str());
			
			glGenTextures(1, &m_bindCode);
			glBindTexture(GL_TEXTURE_2D, m_bindCode);

			if (mips) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			} else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}

			if (clamp) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			} else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img.w, img.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.pix);

			if (mips) {
				glGenerateMipmap(GL_TEXTURE_2D);
			}

			glBindTexture(GL_TEXTURE_2D, 0);

			m_width = img.w;
			m_height = img.h;
			m_target = GL_TEXTURE_2D;

			free(img.pix);
		}
	}

	void GPUTexture::bind(i32 slot) {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(m_target, m_bindCode);
	}

	void GPUTexture::unbind() {
		glBindTexture(m_target, 0);
	}

	void GPUTexture::setFilter(int min, int mag) {
		glBindTexture(m_target, m_bindCode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
		glBindTexture(m_target, 0);
	}

	void GPUTexture::generateMipmaps() {
		glGenerateMipmap(m_target);
	}

	void GPUTexture::init_dds(byte* data, u32 len) {
		gliml::context ctx;
		ctx.enable_dxt(true);
		ctx.enable_etc2(true);
		if (ctx.load(data, len)) {
			LogInfo("Successfully loaded DDS Texture.");
			glGenTextures(1, &m_bindCode);
			glBindTexture(ctx.texture_target(), m_bindCode);

			glTexParameteri(ctx.texture_target(), GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(ctx.texture_target(), GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(ctx.texture_target(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			if (ctx.num_mipmaps(0) > 1) {
				glTexParameteri(ctx.texture_target(), GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			} else {
				glTexParameteri(ctx.texture_target(), GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}

			for (int f = 0; f < ctx.num_faces(); f++) {
				for (int m = 0; m < ctx.num_mipmaps(f); m++) {
					if (ctx.is_2d()) {
						if (ctx.is_compressed()) {
							glCompressedTexImage2D(
								ctx.image_target(f),
								m,
								ctx.image_internal_format(),
								ctx.image_width(f, m),
								ctx.image_height(f, m),
								0,
								ctx.image_size(f, m),
								ctx.image_data(f, m)
							);
						} else {
							glTexImage2D(
								ctx.image_target(f),
								m,
								ctx.image_internal_format(),
								ctx.image_width(f, m),
								ctx.image_height(f, m),
								0,
								ctx.image_format(),
								ctx.image_type(),
								ctx.image_data(f, m)
							);
						}
					}
				}
			}

			m_target = ctx.texture_target();
			m_width = ctx.image_width(0, 0);
			m_height = ctx.image_height(0, 0);

			glBindTexture(ctx.texture_target(), 0);
		} else {
			switch (ctx.error()) {
				case GLIML_ERROR_ENDIAN_MISMATCH: LogError("Endian mismatch."); break;
				case GLIML_ERROR_INVALID_COMPRESSED_FORMAT: LogError("Invalid compressed format."); break;
				case GLIML_ERROR_UNKNOWN_FILE_FORMAT: LogError("Unknown file format."); break;
				case GLIML_ERROR_TEXTURE_ARRAYS_NOT_SUPPORTED: LogError("Texture arrays not supported."); break;
				default: LogError("Unknown error."); break;
			}
		}
	}

}