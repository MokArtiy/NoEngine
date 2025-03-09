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

namespace NoEngine {

	class Grid
	{
	public:
		Grid()
		{
			generate_vertices();
			p_grid_vao = std::make_unique<NoEngine::VertexArray>();
			p_grid_vbo = std::make_unique<NoEngine::VertexBuffer>(m_vertices.data(), m_vertices.size() * sizeof(float), buffer_layout_vec3);
			p_grid_vao->add_vertex_buffer(*p_grid_vbo);
		}
		~Grid()
		{
			this->p_grid_shader->unbind();
			this->p_grid_vao->unbind();
		}


		void draw(std::shared_ptr<NoEngine::ShaderProgram> grid_shader)
		{
			grid_shader->bind();
			Renderer_OpenGL::draw(*p_grid_vao);
		}

	private:
		std::vector<float> m_vertices;
		float m_size;
		float m_step;

		std::unique_ptr<NoEngine::VertexBuffer> p_grid_vbo;
		std::unique_ptr<NoEngine::VertexArray> p_grid_vao;
		std::shared_ptr<NoEngine::ShaderProgram> p_grid_shader;

		NoEngine::BufferLayout buffer_layout_vec3
		{
			NoEngine::ShaderDataType::Float3,
		};

		void generate_vertices(float size = 10.f, float step = 1.f)
		{
			m_size = size;
			m_step = step;

			for (float z = -size; z <= size; z += step) {
				m_vertices.push_back(-size);
				m_vertices.push_back(0.0f);
				m_vertices.push_back(z);

				m_vertices.push_back(size);
				m_vertices.push_back(0.0f);
				m_vertices.push_back(z);
			}

			for (float x = -size; x <= size; x += step) {
				m_vertices.push_back(x);
				m_vertices.push_back(0.0f);
				m_vertices.push_back(-size);

				m_vertices.push_back(x);
				m_vertices.push_back(0.0f);
				m_vertices.push_back(size);
			}
		}
	};
}
