#include "Renderer_OpenGL.hpp"
#include "NoEngineCore/ResourceManager.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VertexArray.hpp"
#include "NoEngineCore/Log.hpp"

#include <iostream>

namespace NoEngine {

	GLFWwindow* Renderer_OpenGL::m_pWindow;

	const char* gl_source_to_string(const GLenum source)
	{
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "DEBUG_SOURCE_API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "DEBUG_SOURCE_WINDOW_SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "GL_DEBUG_SOURCE_SHADER_COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "DEBUG_SOURCE_THIRD_PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "DEBUG_SOURCE_APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "DEBUG_SOURCE_OTHER";
		default: return "UNKNOWN_DEBUG_SOURCE";
		}
	}

	const char* gl_type_to_string(const GLenum type)
	{
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "DEBUG_TYPE_ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEBUG_TYPE_DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "DEBUG_TYPE_UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "DEBUG_TYPE_PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "DEBUG_TYPE_PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "DEBUG_TYPE_MARKER";
		case GL_DEBUG_TYPE_PUSH_GROUP: return "DEBUG_TYPE_PUSH_GROUP";
		case GL_DEBUG_TYPE_POP_GROUP: return "DEBUG_TYPE_POP_GROUP";
		case GL_DEBUG_TYPE_OTHER: return "DEBUG_TYPE_OTHER";
		default: return "UNKNOWN_DEBUG_TYPE";
		}
	}

	bool Renderer_OpenGL::init(GLFWwindow* pWindow)
	{
		m_pWindow = pWindow;

		int width, height, channels;
		unsigned char* pixels = stbi_load((ResourceManager::get_path() + "res/icon.png").c_str(), &width, &height, &channels, 4);

		if (pixels) {
			GLFWimage icon;
			icon.width = width;
			icon.height = height;
			icon.pixels = pixels;

			glfwSetWindowIcon(m_pWindow, 1, &icon);

			stbi_image_free(pixels);
		}

		glfwMakeContextCurrent(pWindow);
		//glfwSwapInterval(1);

		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		{
			LOG_CRITICAL("Failed to initialize GLAD");
			return false;
		}

		LOG_INFO("OpenGL context initialized:");
		LOG_INFO("  OpenGL Vendor {0}", get_vendor_str());
		LOG_INFO("  OpenGL Renderer {0}", get_rendere_str());
		LOG_INFO("  OpenGL Version {0}", get_version_str());

		int max_samples;
		glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
		LOG_INFO("  Max supported samples: {0}", max_samples);

		//glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		glDebugMessageCallback([](GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam)
			{
				switch (severity)
				{
				case GL_DEBUG_SEVERITY_HIGH:
					LOG_ERROR("OpenGL Error: [{0}:{1}]({2}): {3}",
						gl_source_to_string(source),
						gl_type_to_string(type),
						id,
						message);
					break;
				case GL_DEBUG_SEVERITY_MEDIUM:
					LOG_WARN("OpenGL Warning: [{0}:{1}]({2}): {3}",
						gl_source_to_string(source),
						gl_type_to_string(type),
						id,
						message);
					break;
				case GL_DEBUG_SEVERITY_LOW:
					LOG_INFO("OpenGL Info: [{0}:{1}]({2}): {3}",
						gl_source_to_string(source),
						gl_type_to_string(type),
						id,
						message);
					break;
				case GL_DEBUG_SEVERITY_NOTIFICATION:
					LOG_INFO("OpenGL Notificaton: [{0}:{1}]({2}): {3}",
						gl_source_to_string(source),
						gl_type_to_string(type),
						id,
						message);
					break;
				default:
					LOG_ERROR("OpenGL Error: [{0}:{1}] ({2}) : {3}",
						gl_source_to_string(source),
						gl_type_to_string(type),
						id,
						message);
				}
			}, nullptr);

		return true;
	}

	void Renderer_OpenGL::draw(const VertexArray& vertex_array, const DrawMode& mode)
	{
		vertex_array.bind();
		switch (mode)
		{
		case NoEngine::DrawMode::Triangles:
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vertex_array.get_indices_count()), GL_UNSIGNED_INT, nullptr);
			break;
		case NoEngine::DrawMode::Lines:
			glDrawElements(GL_LINES, static_cast<GLsizei>(vertex_array.get_indices_count()), GL_UNSIGNED_INT, nullptr);
			break;
		case NoEngine::DrawMode::Points:
			glDrawArrays(GL_POINTS, 0, 1);
			break;
		default:
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vertex_array.get_indices_count()), GL_UNSIGNED_INT, nullptr);
			break;
		}
	}

	void Renderer_OpenGL::set_clear_color(const float r, const float g, const float b, const float a)
	{
		glClearColor(r, g, b, a);
	}

	void Renderer_OpenGL::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer_OpenGL::clear_stencil()
	{
		glClear(GL_STENCIL_BUFFER_BIT);
	}

	void Renderer_OpenGL::set_viewport(const unsigned int width, const unsigned int height, const unsigned int left_offset, const unsigned int bottom_offset)
	{
		glViewport(left_offset, bottom_offset, width, height);
	}

	void Renderer_OpenGL::enable_depth_testing()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void Renderer_OpenGL::disable_depth_testing()
	{
		glDisable(GL_DEPTH_TEST);
	}

	void Renderer_OpenGL::disable_stencil_testing()
	{
		glDisable(GL_STENCIL_TEST);
	}

	void Renderer_OpenGL::enable_stencil_testing()
	{
		glEnable(GL_STENCIL_TEST);
	}

	void Renderer_OpenGL::save_depth_mask(unsigned char depth_mask)
	{
		glGetBooleanv(GL_DEPTH_WRITEMASK, &depth_mask);
		glDepthMask(GL_FALSE);
	}

	void Renderer_OpenGL::set_depth_mask(unsigned char depth_mask)
	{
		glDepthMask(depth_mask);
	}

	void Renderer_OpenGL::set_depth_func(DepthFunc func)
	{
		switch (func)
		{
		case NoEngine::DepthFunc::Never:
			glDepthFunc(GL_NEVER);
			break;
		case NoEngine::DepthFunc::Less:
			glDepthFunc(GL_LESS);
			break;
		case NoEngine::DepthFunc::Lequal:
			glDepthFunc(GL_LEQUAL);
			break;
		case NoEngine::DepthFunc::Greater:
			glDepthFunc(GL_GREATER);
			break;
		case NoEngine::DepthFunc::Gequal:
			glDepthFunc(GL_GEQUAL);
			break;
		case NoEngine::DepthFunc::Equal:
			glDepthFunc(GL_EQUAL);
			break;
		case NoEngine::DepthFunc::Notequal:
			glDepthFunc(GL_NOTEQUAL);
			break;
		case NoEngine::DepthFunc::Always:
			glDepthFunc(GL_ALWAYS);
			break;
		default:
			glDepthFunc(GL_ALWAYS);
			break;
		}
	}

	void Renderer_OpenGL::enable_input()
	{
		glfwSetInputMode(m_pWindow, GLFW_STICKY_KEYS, GLFW_TRUE);
	}

	void Renderer_OpenGL::disable_input()
	{
		glfwSetInputMode(m_pWindow, GLFW_STICKY_KEYS, GLFW_FALSE);
	}

	void Renderer_OpenGL::enable_poligon_mode()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void Renderer_OpenGL::disable_poligon_mode()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void Renderer_OpenGL::configurate_opengl()
	{
		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
	}

	void Renderer_OpenGL::set_stencil_mask(unsigned int value)
	{
		glStencilMask(value);
	}

	void Renderer_OpenGL::set_stencil_func(StencilFunc func, int ref, unsigned int mask)
	{
		switch (func)
		{
		case NoEngine::StencilFunc::Never:
			glStencilFunc(GL_NEVER, ref, mask);
			break;
		case NoEngine::StencilFunc::Less:
			glStencilFunc(GL_LESS, ref, mask);
			break;
		case NoEngine::StencilFunc::Lequal:
			glStencilFunc(GL_LEQUAL, ref, mask);
			break;
		case NoEngine::StencilFunc::Greater:
			glStencilFunc(GL_GREATER, ref, mask);
			break;
		case NoEngine::StencilFunc::Gequal:
			glStencilFunc(GL_GEQUAL, ref, mask);
			break;
		case NoEngine::StencilFunc::Equal:
			glStencilFunc(GL_EQUAL, ref, mask);
			break;
		case NoEngine::StencilFunc::Notequal:
			glStencilFunc(GL_NOTEQUAL, ref, mask);
			break;
		case NoEngine::StencilFunc::Always:
			glStencilFunc(GL_ALWAYS, ref, mask);
			break;
		default:
			glStencilFunc(GL_ALWAYS, ref, mask);
			break;
		}
	}

	void Renderer_OpenGL::set_stencil_op(StencilOp sfail, StencilOp dpfail, StencilOp dppass)
	{
		GLenum gl_sfail = convert_stencil_op(sfail);
		GLenum gl_dpfail = convert_stencil_op(dpfail);
		GLenum gl_dppass = convert_stencil_op(dppass);

		glStencilOp(gl_sfail, gl_dpfail, gl_dppass);
	}

	void Renderer_OpenGL::get_error()
	{
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG_ERROR("Error rendering scene: {0}", error);
		}
	}

	const char* Renderer_OpenGL::get_vendor_str()
	{
		return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	}

	const char* Renderer_OpenGL::get_rendere_str()
	{
		return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	}

	const char* Renderer_OpenGL::get_version_str()
	{
		return reinterpret_cast<const char*>(glGetString(GL_VERSION));
	}

	unsigned int Renderer_OpenGL::convert_stencil_op(StencilOp op)
	{
		switch (op) {
		case StencilOp::Keep:       return GL_KEEP;
		case StencilOp::Zero:       return GL_ZERO;
		case StencilOp::Replace:    return GL_REPLACE;
		case StencilOp::Incr:       return GL_INCR;
		case StencilOp::IncrWrap:   return GL_INCR_WRAP;
		case StencilOp::Decr:       return GL_DECR;
		case StencilOp::DecrWrap:   return GL_DECR_WRAP;
		case StencilOp::Invert:     return GL_INVERT;
		default:                    return GL_KEEP;
		}
	}
}