#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

#include "../OpenGL/VertexArray.hpp"
#include "../OpenGL/VertexBuffer.hpp"
#include "../OpenGL/IndexBuffer.hpp"
#include "../OpenGL/ShaderProgram.hpp"
#include "../OpenGL/Renderer_OpenGL.hpp"
#include "../NoEngineCore/includes/NoEngineCore/Camera.hpp"
#include "../NoEngineCore/includes/NoEngineCore/ResourceManager.hpp"

#include <iostream>

namespace NoEngine {

	class Grid
	{
	public:
		Grid()
		{
			p_axes_vao = std::make_unique<NoEngine::VertexArray>();
			p_axes_vbo = std::make_unique<NoEngine::VertexBuffer>(axes, sizeof(axes), buffer_layout_vec3_vec3);
			p_axes_index = std::make_unique<NoEngine::IndexBuffer>(axes_indices, sizeof(axes_indices) / sizeof(GLuint));

			p_axes_vao->add_vertex_buffer(*p_axes_vbo);
			p_axes_vao->set_index_buffer(*p_axes_index);

			p_axes_vao->unbind();

			generate_grid();
			p_grid_vao = std::make_unique<NoEngine::VertexArray>();
			p_grid_vbo = std::make_unique<NoEngine::VertexBuffer>(m_vertices, sizeof(m_vertices), buffer_layout_vec3_vec3);
			p_indices_buffer = std::make_unique<NoEngine::IndexBuffer>(m_indices, sizeof(m_indices) / sizeof(GLuint));
			p_grid_vao->add_vertex_buffer(*p_grid_vbo);
			p_grid_vao->set_index_buffer(*p_indices_buffer);

			p_grid_vao->unbind();
		}
		~Grid()
		{
			this->p_grid_shader->unbind();
			this->p_grid_vao->unbind();
			this->p_axes_vao->unbind();
		}


		void draw(std::shared_ptr<NoEngine::ShaderProgram> grid_shader)
		{
			grid_shader->bind();
			Renderer_OpenGL::draw(*p_axes_vao, NoEngine::DrawMode::Lines);
			p_axes_vao->unbind();
			Renderer_OpenGL::draw(*p_grid_vao, NoEngine::DrawMode::Lines);
			p_grid_vao->unbind();
		}

	private:
		const int size = 75;
		const int step = 1;
		static const int grid_lines = (100 * 2 / 1) + 1;
		static const int num_vertices = grid_lines * 4;
		GLfloat m_vertices[num_vertices * 6];
		static const int numIndices = grid_lines * 4; // 2 индекса на линию, 2 направления (X и Z)
		GLuint m_indices[numIndices];

		std::unique_ptr<NoEngine::VertexBuffer> p_grid_vbo;
		std::unique_ptr<NoEngine::IndexBuffer> p_indices_buffer;
		std::unique_ptr<NoEngine::VertexArray> p_grid_vao;

		std::shared_ptr<NoEngine::ShaderProgram> p_grid_shader;

		std::unique_ptr<NoEngine::VertexBuffer> p_axes_vbo;
		std::unique_ptr<NoEngine::VertexArray> p_axes_vao;
		std::unique_ptr<NoEngine::IndexBuffer> p_axes_index;

		NoEngine::BufferLayout buffer_layout_vec3_vec3
		{
			NoEngine::ShaderDataType::Float3,
			NoEngine::ShaderDataType::Float3
		};

		GLfloat axes[36] = {
			// Ось X
			-75.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f, // Начало
			75.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f, // Конец

			// Ось Y
			0.0f, -75.0f, 0.0f,		0.0f, 1.0f, 0.0f, // Начало
			0.0f, 75.0f, 0.0f,		0.0f, 1.0f, 0.0f, // Конец

			// Ось Z
			0.0f, 0.0f, -75.0f,		0.0f, 0.0f, 1.0f, // Начало
			0.0f, 0.0f, 75.0f,		0.0f, 0.0f, 1.0f  // Конец
		};
		GLuint axes_indices[6] = {
			0, 1,
			2, 3,
			4, 5
		};

		void generate_grid()
		{
			int index = 0;
			for (float y = -size; y <= size; y += step) {
				// Начало линии
				m_vertices[index++] = -size;   // X
				m_vertices[index++] = y;       // Y
				m_vertices[index++] = 0.0f;    // Z
				m_vertices[index++] = 0.5f;    // R (серый цвет)
				m_vertices[index++] = 0.5f;    // G
				m_vertices[index++] = 0.5f;    // B

				// Конец линии
				m_vertices[index++] = size;      // X
				m_vertices[index++] = y;	   // Y
				m_vertices[index++] = 0.0f;    // Z
				m_vertices[index++] = 0.5f;    // R
				m_vertices[index++] = 0.5f;    // G
				m_vertices[index++] = 0.5f;    // B
			}
			// Линии, параллельные оси X
			for (float x = -size; x <= size; x += step) {
				// Начало линии
				m_vertices[index++] = x;        // X
				m_vertices[index++] = -size;     // Y
				m_vertices[index++] = 0.0f;		// Z
				m_vertices[index++] = 0.5f;     // R
				m_vertices[index++] = 0.5f;     // G
				m_vertices[index++] = 0.5f;     // B

				// Конец линии
				m_vertices[index++] = x;        // X
				m_vertices[index++] = size;       // Y
				m_vertices[index++] = 0.0f;		// Z
				m_vertices[index++] = 0.5f;     // R
				m_vertices[index++] = 0.5f;     // G
				m_vertices[index++] = 0.5f;     // B
			}

			int idx = 0;
			// Индексы для X
			for (int i = 0; i < grid_lines; ++i) {
				m_indices[idx++] = i * 2;
				m_indices[idx++] = i * 2 + 1;
			}
			// Индексы для Y
			int offset = grid_lines * 2;
			for (int i = 0; i < grid_lines; ++i) {
				m_indices[idx++] = offset + i * 2;
				m_indices[idx++] = offset + i * 2 + 1;
			}
		}
	};
}
