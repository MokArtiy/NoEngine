#pragma once

#include "Actor.hpp"

class Plane : public Actor
{
public:
	Plane(std::shared_ptr<NoEngine::ShaderProgram> shader,
		std::shared_ptr<NoEngine::ShaderProgram> outline_shader,
		const glm::vec3& position = glm::vec3(0.0f),
		const glm::vec3& rotation = glm::vec3(0.0f),
		const glm::vec3& scale = glm::vec3(1.0f),
		const std::string name = "")
		: Actor(shader, outline_shader, position, rotation, scale, name)
	{
		p_vao = std::make_unique<NoEngine::VertexArray>();
		p_position_vbo = std::make_unique<NoEngine::VertexBuffer>(vertices, sizeof(vertices), buffer_layout_vec3_vec3_vec2);
		p_index_buffer = std::make_unique<NoEngine::IndexBuffer>(indices, sizeof(indices) / sizeof(GLuint));

		p_vao->add_vertex_buffer(*p_position_vbo);
		p_vao->set_index_buffer(*p_index_buffer);
		p_vao->unbind();
	}

	~Plane()
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
		glm::vec3 plane_min = glm::vec3(get_model_matrix() * glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f));
		glm::vec3 plane_max = glm::vec3(get_model_matrix() * glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

		const float epsilon = 1e-6f;
		//glm::vec3 invDir = 1.0f / ray_direction;

		glm::vec3 invDir = 1.0f / glm::vec3(
			fabs(ray_direction.x) < epsilon ? epsilon : ray_direction.x,
			fabs(ray_direction.y) < epsilon ? epsilon : ray_direction.y,
			fabs(ray_direction.z) < epsilon ? epsilon : ray_direction.z
		);

		glm::vec3 t1 = (plane_min - ray_origin) * invDir;
		glm::vec3 t2 = (plane_max - ray_origin) * invDir;

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

	GLfloat vertices[32] = {
	//  posX, posY, posZ,		normX,normY,normZ,		uvX,  uvY
		-1.f, -1.f,  0.0f,		0.0f, 0.0f,	1.0f,	0.0f, 0.0f,
		1.f,  -1.f,  0.0f, 		0.0f, 0.0f,	1.0f,	1.0f, 0.0f,
		1.f,   1.f,  0.0f, 		0.0f, 0.0f,	1.0f,	1.0f, 1.0f,
		-1.f,  1.f,  0.0f,		0.0f, 0.0f,	1.0f,	0.0f, 1.0f
	};

	GLuint indices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	NoEngine::BufferLayout buffer_layout_vec3_vec3_vec2
	{
		NoEngine::ShaderDataType::Float3,
		NoEngine::ShaderDataType::Float3,
		NoEngine::ShaderDataType::Float2
	};
};