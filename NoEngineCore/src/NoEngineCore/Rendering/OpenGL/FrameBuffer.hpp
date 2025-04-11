#pragma once

namespace NoEngine {

    class FrameBuffer 
    {
    public:
        FrameBuffer();
        ~FrameBuffer();

        void create(unsigned int width, unsigned int height);
        void bind();
        void unbind();
        void resolveToTexture(); // Добавляем метод для resolve

        unsigned int getTextureID() const { return m_samples > 1 ? m_resolve_texture_id : m_texture_id; }
        void delete_buffers();
        unsigned int getFBO() const { return m_fbo_id; }

    private:
        unsigned int m_fbo_id = 0;
        unsigned int m_texture_id = 0;
        unsigned int m_depth_id = 0;
        unsigned int m_resolve_fbo_id = 0;    
        unsigned int m_resolve_texture_id = 0;
        unsigned int m_width = 0;
        unsigned int m_height = 0;
        int m_samples = 0;

        const char* get_framebuffer_status() const;
    };
}