#include "GPURenderTarget.h"

#include "glad.h"
#include "../core/Logger.h"

static const char* GPUFramebufferError(GLenum status) {
	switch (status) {
		case GL_FRAMEBUFFER_COMPLETE: return "";
		case GL_INVALID_OPERATION: return "Invalid operation";
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return "Incomplete attachment";
		case GL_FRAMEBUFFER_UNSUPPORTED: return "Unsupported framebuffer format";
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "Missing attachment";
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: return "Missing draw buffer";
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: return "Missing read buffer";
	}
	return "Unknown error";
}

namespace tfx {
	GPURenderTarget::GPURenderTarget(
		u32 width, u32 height,
		GPUTexture::GPUTextureType attachmentType,
		bool mips
	) :	m_width(width),
		m_height(height),
		m_savedAttachmentType(attachmentType),
		m_generateMipmaps(mips)
	{
		glGenFramebuffers(1, &m_bindCode);
		m_attachment = new GPUTexture(width, height, attachmentType, false, mips);
		m_attachment->setFilter(GL_LINEAR, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, m_bindCode);
		glFramebufferTexture(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			m_attachment->getBindCode(),
			0
		);

		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			LogError(
				String("Framebuffer error(") +
				String(status) + String("): ") +
				String(GPUFramebufferError(status))
			);
			glDeleteFramebuffers(1, &m_bindCode);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	GPURenderTarget::~GPURenderTarget() {
		if (m_bindCode) glDeleteFramebuffers(1, &m_bindCode);
		delete m_attachment;
	}

	void GPURenderTarget::bind(BindMode bindMode) {
		m_tmp_bindMode = bindMode;

		glGetIntegerv(GL_VIEWPORT, m_vp);
		switch (bindMode) {
			case BindMode::Read: glBindFramebuffer(GL_READ_FRAMEBUFFER, m_bindCode); break;
			case BindMode::Write: glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_bindCode); break;
			case BindMode::ReadWrite: glBindFramebuffer(GL_FRAMEBUFFER, m_bindCode); break;
		}
		glViewport(0, 0, m_width, m_height);
	}

	void GPURenderTarget::unbind() {
		switch (m_tmp_bindMode) {
			case BindMode::Read: glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); break;
			case BindMode::Write: glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); break;
			case BindMode::ReadWrite: glBindFramebuffer(GL_FRAMEBUFFER, 0); break;
		}

		if (m_generateMipmaps) {
			m_attachment->generateMipmaps();
		}

		glViewport(m_vp[0], m_vp[1], m_vp[2], m_vp[3]);
	}
}