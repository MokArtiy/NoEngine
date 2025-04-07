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
	enum class DrawMode
	{
		Triangles,
		Lines,
		Points
	};

	class VertexArray;

	class Renderer_OpenGL {
	public:
		static bool init(GLFWwindow* pWindow);

		static void draw(const VertexArray& vertex_array, const DrawMode& mode = DrawMode::Triangles);
		static void set_clear_color(const float r, const float g, const float b, const float a);
		static void clear();
		static void clear_stencil();
		static void set_viewport(const unsigned int width, const unsigned int height, const unsigned int left_offset = 0, const unsigned int bottom_offset = 0);
		static void enable_depth_testing();
		static void disable_depth_testing();
		static void disable_stencil_testing();
		static void enable_stencil_testing();
		static void enable_input();
		static void disable_input();
		static void enable_poligon_mode();
		static void disable_poligon_mode();
		static void configurate_opengl();
		static void set_stencil_mask(unsigned int mask);
		static void set_stencil_func(StencilFunc func, int ref, unsigned int mask);
		static void get_error();

		static const char* get_vendor_str();
		static const char* get_rendere_str();
		static const char* get_version_str();
	
	private:
		static GLFWwindow* m_pWindow;
	};
}