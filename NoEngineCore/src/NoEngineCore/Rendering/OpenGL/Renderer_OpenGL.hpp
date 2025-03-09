#pragma once

struct GLFWwindow;

namespace NoEngine {
	enum class StencilFunc
	{
		Never,
		Less,
		Lequal,
		Greater,
		Gequal,
		Equal,
		Notequal,
		Always
	};

	class VertexArray;

	class Renderer_OpenGL {
	public:
		static bool init(GLFWwindow* pWindow);

		static void draw(const VertexArray& vertex_array);
		static void set_clear_color(const float r, const float g, const float b, const float a);
		static void clear();
		static void clear_stencil_func();
		static void set_viewport(const unsigned int width, const unsigned int height, const unsigned int left_offset = 0, const unsigned int bottom_offset = 0);
		static void enable_depth_testing();
		static void disable_depth_testing();
		static void configurate_opengl();
		static void set_stencil_mask(unsigned int mask);
		static void set_stencil_func(StencilFunc func, int ref, unsigned int mask);

		static const char* get_vendor_str();
		static const char* get_rendere_str();
		static const char* get_version_str();
	};
}