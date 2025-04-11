#include "FrameBuffer.hpp"
#include "../NoEngineCore/includes/NoEngineCore/Log.hpp"
#include "NoEngineCore/Rendering/OpenGL/Renderer_OpenGL.hpp"
#include <glad/glad.h>
#include <string>

namespace NoEngine {

    FrameBuffer::FrameBuffer() {
        glGetIntegerv(GL_MAX_SAMPLES, &m_samples);
        m_samples = std::min(4, m_samples);
        if (m_samples < 1) m_samples = 1;

        LOG_INFO("Use samples: {0}", m_samples);
    };

    FrameBuffer::~FrameBuffer() {
        delete_buffers();
    }

    void FrameBuffer::create(unsigned int width, unsigned int height) {
        if (width == 0 || height == 0) return;

        delete_buffers();
        m_width = width;
        m_height = height;

        // 2. Создаем основной FBO
        glGenFramebuffers(1, &m_fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);

        // 3. Цветовой буфер
        glGenTextures(1, &m_texture_id);
        if (m_samples > 1) {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_texture_id);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_samples, GL_RGBA8, width, height, GL_TRUE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_texture_id, 0);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, m_texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_id, 0);
        }

        // 4. Буфер глубины
        glGenRenderbuffers(1, &m_depth_id);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depth_id);
        if (m_samples > 1) {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_samples, GL_DEPTH24_STENCIL8, width, height);
        }
        else {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        }
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_id);

        // 5. Проверка основного FBO
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            LOG_ERROR("Main FBO incomplete: {0}", get_framebuffer_status());
            delete_buffers();
            return;
        }

        // 6. Для мультисэмплинга создаем resolve FBO
        if (m_samples > 1) {
            glGenFramebuffers(1, &m_resolve_fbo_id);
            glBindFramebuffer(GL_FRAMEBUFFER, m_resolve_fbo_id);

            glGenTextures(1, &m_resolve_texture_id);
            glBindTexture(GL_TEXTURE_2D, m_resolve_texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_resolve_texture_id, 0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                LOG_ERROR("Resolve FBO incomplete: {0}", get_framebuffer_status());
                delete_buffers();
                return;
            }
        }

        unbind();
    }

    void FrameBuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);
        Renderer_OpenGL::set_viewport(m_width, m_height, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void FrameBuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::resolveToTexture() {
        if (m_samples <= 1) return;

        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo_id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_resolve_fbo_id);
        glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height,
            GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::delete_buffers() {
        if (m_fbo_id) glDeleteFramebuffers(1, &m_fbo_id);
        if (m_resolve_fbo_id) glDeleteFramebuffers(1, &m_resolve_fbo_id);
        if (m_texture_id) glDeleteTextures(1, &m_texture_id);
        if (m_resolve_texture_id) glDeleteTextures(1, &m_resolve_texture_id);
        if (m_depth_id) glDeleteRenderbuffers(1, &m_depth_id);

        m_fbo_id = m_resolve_fbo_id = m_texture_id = m_resolve_texture_id = m_depth_id = 0;
    }

    const char* FrameBuffer::get_framebuffer_status() const {
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        switch (status) {
        case GL_FRAMEBUFFER_COMPLETE: return "Complete";
        case GL_FRAMEBUFFER_UNDEFINED: return "Undefined";
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return "Incomplete attachment";
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "Missing attachment";
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: return "Incomplete draw buffer";
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: return "Incomplete read buffer";
        case GL_FRAMEBUFFER_UNSUPPORTED: return "Unsupported";
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: return "Incomplete multisample";
        default: return "Unknown error";
        }
    }
}