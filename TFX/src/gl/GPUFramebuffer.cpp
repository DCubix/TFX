#include "GPUFramebuffer.h"

#include "glad.h"
#include "../core/Logger.h"

#include <algorithm>

static const char* getFramebufferError(GLenum status) {
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
	GPUFramebuffer::GPUFramebuffer() : m_attachments(0), m_width(0), m_height(0) {
		glGenFramebuffers(1, &m_bindCode);
	}

	GPUFramebuffer::~GPUFramebuffer() {
		if (m_bindCode) glDeleteFramebuffers(1, &m_bindCode);
	}

	void GPUFramebuffer::addColorAttachment(GPUTexture* texture) {
		if (!texture) return;

		m_width = std::max(m_width, texture->getWidth());
		m_height = std::max(m_height, texture->getHeight());

		glBindFramebuffer(GL_FRAMEBUFFER, m_bindCode);
		glFramebufferTexture(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + m_attachments,
			texture->getBindCode(), 0
		);

		GLenum* drawBuffers = new GLenum[m_attachments + 1];
		for (u32 i = 0; i < m_attachments+1; i++) {
			drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
		}
		glDrawBuffers(m_attachments + 1, drawBuffers);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			LogError(String("Framebuffer error(") + String(status) + String("): ") + String(getFramebufferError(status)));
			glDeleteFramebuffers(1, &m_bindCode);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return;
		}
		m_attachments++;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GPUFramebuffer::withDepth() {
		glGenRenderbuffers(1, &m_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_bindCode);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GPUFramebuffer::bind() {
		glGetIntegerv(GL_VIEWPORT, m_vp);
		glBindFramebuffer(GL_FRAMEBUFFER, m_bindCode);
		glViewport(0, 0, m_width, m_height);
	}

	void GPUFramebuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(m_vp[0], m_vp[1], m_vp[2], m_vp[3]);
	}

}