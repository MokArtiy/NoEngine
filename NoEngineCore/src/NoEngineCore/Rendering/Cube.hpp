#pragma once

#include "Actor.hpp"

class Cube : public Actor
{
public:
	Cube(glm::mat4 view_matrix, glm::mat4 projection_matrix, glm::vec3 camera_position,
		std::shared_ptr<NoEngine::ShaderProgram> shader,
		std::shared_ptr<NoEngine::ShaderProgram> outline_shader,
		 const glm::vec3& position = glm::vec3(0.0f),
		 const glm::vec3& rotation = glm::vec3(0.0f),
		 const glm::vec3& scale    = glm::vec3(1.0f),
		 const std::string name = "")
		: Actor(view_matrix, projection_matrix, camera_position, shader, outline_shader, position, rotation, scale, name)
	{
		p_vao = std::make_unique<NoEngine::VertexArray>();
		p_position_vbo = std::make_unique<NoEngine::VertexBuffer>(pos_norm_uv, sizeof(pos_norm_uv), buffer_layout_vec3_vec3_vec2);
		p_index_buffer = std::make_unique<NoEngine::IndexBuffer>(indices, sizeof(indices) / sizeof(GLuint));

		p_vao->add_vertex_buffer(*p_position_vbo);
		p_vao->set_index_buffer(*p_index_buffer);
		p_vao->unbind();
	}

	~Cube() 
	{
		this->p_shader->unbind();
		this->p_vao->unbind();
	}

	void draw(std::string param) override
	{
		if (param == "stencil")
		{
			p_outline_shader->bind();
			glm::mat4 model = get_model_matrix();
			model = glm::scale(model, glm::vec3(1.01f));
			p_outline_shader->set_matrix4("model_matrix", model);
			NoEngine::Renderer_OpenGL::draw(*p_vao);
		}
		else
		{
			p_shader->bind();
			set_material_shader();
			p_shader->set_bool("use_texture", false);
			p_shader->set_matrix4("model_matrix", get_model_matrix());
			p_shader->set_matrix3("normal_matrix", glm::transpose(glm::inverse(glm::mat3(get_model_matrix()))));

			NoEngine::Renderer_OpenGL::draw(*p_vao);
		}
		p_vao->unbind();
	}

	bool intersect(const glm::vec3& ray_origin, const glm::vec3& ray_direction, float& distance) const override
	{
		glm::vec3 cube_min = glm::vec3(get_model_matrix() * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
		glm::vec3 cube_max = glm::vec3(get_model_matrix() * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		const float epsilon = 1e-6f;
		//glm::vec3 invDir = 1.0f / ray_direction;

		glm::vec3 invDir = 1.0f / glm::vec3(
			fabs(ray_direction.x) < epsilon ? epsilon : ray_direction.x,
			fabs(ray_direction.y) < epsilon ? epsilon : ray_direction.y,
			fabs(ray_direction.z) < epsilon ? epsilon : ray_direction.z
		);

		glm::vec3 t1 = (cube_min - ray_origin) * invDir;
		glm::vec3 t2 = (cube_max - ray_origin) * invDir;

		glm::vec3 tmin = glm::min(t1, t2);
		glm::vec3 tmax = glm::max(t1, t2);

		float tminMax = glm::max(glm::max(tmin.x, tmin.y), tmin.z);
		float tmaxMin = glm::min(glm::min(tmax.x, tmax.y), tmax.z);

		if (tminMax > tmaxMin) {
			return false;
		}

		if (tmaxMin < 0) {
			return false;
		}

		distance = tminMax;
		return true;
	}

private:
	std::unique_ptr<NoEngine::VertexArray> p_vao;
	std::unique_ptr<NoEngine::VertexBuffer> p_position_vbo;
	std::unique_ptr<NoEngine::IndexBuffer> p_index_buffer;

	GLfloat pos_norm_uv[192] = {
		//    position             normal            UV                  index
		// FRONT
		-1.0f, -1.f, -1.f,    -1.f,  0.f,  0.f,     0.f, 0.f,              // 0
		-1.0f,  1.f, -1.f,    -1.f,  0.f,  0.f,     1.f, 0.f,              // 1
		-1.0f,  1.f,  1.f,    -1.f,  0.f,  0.f,     1.f, 1.f,              // 2
		-1.0f, -1.f,  1.f,    -1.f,  0.f,  0.f,     0.f, 1.f,              // 3
		// BACK                                  
		 1.0f, -1.f, -1.f,     1.f,  0.f,  0.f,     1.f, 0.f,              // 4
		 1.0f,  1.f, -1.f,     1.f,  0.f,  0.f,     0.f, 0.f,              // 5
		 1.0f,  1.f,  1.f,     1.f,  0.f,  0.f,     0.f, 1.f,              // 6
		 1.0f, -1.f,  1.f,     1.f,  0.f,  0.f,     1.f, 1.f,              // 7
		 // RIGHT
		 -1.0f,  1.f, -1.f,     0.f,  1.f,  0.f,     0.f, 0.f,              // 8
		  1.0f,  1.f, -1.f,     0.f,  1.f,  0.f,     1.f, 0.f,              // 9
		  1.0f,  1.f,  1.f,     0.f,  1.f,  0.f,     1.f, 1.f,              // 10
		 -1.0f,  1.f,  1.f,     0.f,  1.f,  0.f,     0.f, 1.f,              // 11
		 // LEFT
		 -1.0f, -1.f, -1.f,     0.f, -1.f,  0.f,     1.f, 0.f,              // 12
		  1.0f, -1.f, -1.f,     0.f, -1.f,  0.f,     0.f, 0.f,              // 13
		  1.0f, -1.f,  1.f,     0.f, -1.f,  0.f,     0.f, 1.f,              // 14
		 -1.0f, -1.f,  1.f,     0.f, -1.f,  0.f,     1.f, 1.f,              // 15
		 // TOP
		 -1.0f, -1.f,  1.f,     0.f,  0.f,  1.f,     0.f, 0.f,              // 16
		 -1.0f,  1.f,  1.f,     0.f,  0.f,  1.f,     1.f, 0.f,              // 17
		  1.0f,  1.f,  1.f,     0.f,  0.f,  1.f,     1.f, 1.f,              // 18
		  1.0f, -1.f,  1.f,     0.f,  0.f,  1.f,     0.f, 1.f,              // 19
		  // BOTTOM
		  -1.0f, -1.f, -1.f,    0.f,  0.f, -1.f,     0.f, 1.f,              // 20
		  -1.0f,  1.f, -1.f,    0.f,  0.f, -1.f,     1.f, 1.f,              // 21
		   1.0f,  1.f, -1.f,    0.f,  0.f, -1.f,     1.f, 0.f,              // 22
		   1.0f, -1.f, -1.f,    0.f,  0.f, -1.f,     0.f, 0.f,              // 23
	};
	GLuint indices[36] = {
		0,   1,  2,  2,  3,  0, // front
		4,   5,  6,  6,  7,  4, // back
		8,   9, 10, 10, 11,  8, // right
		12, 13, 14, 14, 15, 12, // left
		16, 17, 18, 18, 19, 16, // top
		20, 21, 22, 22, 23, 20  // bottom
	};

	NoEngine::BufferLayout buffer_layout_vec3_vec3_vec2
	{
		NoEngine::ShaderDataType::Float3,
		NoEngine::ShaderDataType::Float3,
		NoEngine::ShaderDataType::Float2
	};
};