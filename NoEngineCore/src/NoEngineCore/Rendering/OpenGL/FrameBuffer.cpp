#include "FrameBuffer.hpp"
#include "../NoEngineCore/includes/NoEngineCore/Log.hpp"
#include "NoEngineCore/Rendering/OpenGL/Renderer_OpenGL.hpp"

#include <glad/glad.h>

namespace NoEngine {

	void FrameBuffer::create(unsigned int width, unsigned int height)
	{
		m_width = width;
		m_height = height;

		if (m_id)
		{
			delete_buffers();
		}

		glGenFramebuffers(1, &m_id);
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);

		//glCreateTextures(GL_TEXTURE_2D, 1, &m_texture_id);
		glGenTextures(1, &m_texture_id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_id, 0);


		glGenRenderbuffers(1, &m_depth_id);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depth_id);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, m_width, m_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_id);

		GLenum buffers[4] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(m_texture_id, buffers);

		unbind();
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void FrameBuffer::delete_buffers()
	{
		if (m_id)
		{
			glDeleteFramebuffers(GL_FRAMEBUFFER, &m_id);
			glDeleteTextures(1, &m_texture_id);
			//glDeleteTextures(1, &m_depth_id);
			glDeleteRenderbuffers(1, &m_depth_id);
			m_texture_id = 0;
			m_depth_id = 0;
		}
	}

	void FrameBuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		Renderer_OpenGL::set_viewport(m_width, m_height, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void FrameBuffer::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}
