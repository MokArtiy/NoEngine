#pragma once

namespace NoEngine {

	class FrameBuffer
	{
	public:
		FrameBuffer() : m_id{ 0 }, m_depth_id{ 0 } {}
		void create(unsigned int width, unsigned int height);
		void delete_buffers();
		void bind();
		static void unbind();

		unsigned int get_texture_id() const { return m_texture_id; }

	private:
		unsigned int m_id = 0;
		unsigned int m_texture_id = 0;
		unsigned int m_depth_id = 0;
		unsigned int m_width = 0;
		unsigned int m_height = 0;
	};
}