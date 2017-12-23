#include "GPUTexture.h"

#include "glad.h"

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
			case GPUTextureType::Mono:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
				break;
		}

		if (mips) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		m_target = GL_TEXTURE_2D;
	}

	GPUTexture::GPUTexture(byte* fileData, u32 length, bool clamp, bool mips) {
		tpImage img = tpLoadPNGMem(fileData, length);
		init((byte*) img.pix, img.w, img.h, clamp, mips);
		free(img.pix);
	}

	GPUTexture::GPUTexture(String fileName, bool clamp, bool mips) {
		tpImage img = tpLoadPNG(fileName.str().c_str());
		init((byte*) img.pix, img.w, img.h, clamp, mips);
		free(img.pix);
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

	void GPUTexture::init(byte* data, u32 w, u32 h, bool clamp, bool mips) {
		glGenTextures(1, &m_bindCode);
		glBindTexture(GL_TEXTURE_2D, m_bindCode);

		if (mips) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		if (mips) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		m_width = w;
		m_height = h;
		m_target = GL_TEXTURE_2D;
	}

}