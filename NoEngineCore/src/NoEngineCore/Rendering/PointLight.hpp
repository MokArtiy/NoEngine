#pragma once

#include "Actor.hpp"

class PointLight : public Actor
{
public:
	PointLight(std::shared_ptr<NoEngine::ShaderProgram> shader,
		std::shared_ptr<NoEngine::ShaderProgram> outline_shader,
		int type = NE_POINT_LIGHT,
		const glm::vec3& position = glm::vec3(0.0f),
		const glm::vec3& rotation = glm::vec3(0.0f),
		const glm::vec3& scale = glm::vec3(1.0f),
		const glm::vec3 ambient = glm::vec3(0.1),
		const glm::vec3 diffuse = glm::vec3(1.0f),
		const glm::vec3 specular = glm::vec3(1.0f),
		const float shininess = 32.f,
		const std::string name = "")
		: Actor(shader, outline_shader, type, position, rotation, scale, ambient, diffuse, specular, shininess, name)
	{
		generate_sphere();

		m_color = glm::vec3(1.0f, 1.0f, 1.0f);

		m_constant = 1.0f;
		m_linear = 0.14f;
		m_quadratic = 0.07f;

		p_vao = std::make_unique<NoEngine::VertexArray>();
		p_position_vbo = std::make_unique<NoEngine::VertexBuffer>(m_vertices.data(), m_vertices.size() * sizeof(float), buffer_layout_vec3_vec3_vec2);
		p_index_buffer = std::make_unique<NoEngine::IndexBuffer>(m_indices.data(),m_indices.size());

		p_vao->add_vertex_buffer(*p_position_vbo);
		p_vao->set_index_buffer(*p_index_buffer);
		p_vao->unbind();
	}

	~PointLight()
	{
		this->p_shader->unbind();
		this->p_vao->unbind();
	}

	void set_color(glm::vec3 color) { m_color = color; }
	glm::vec3 get_color() const { return m_color; }

	void set_constant(float constant) { m_constant = constant; }
	float get_constant() const { return m_constant; }

	void set_linear(float linear) { m_linear = linear; }
	float get_linear() const { return m_linear; }

	void set_quadratic(float quadratic) { m_quadratic = quadratic; }
	float get_quadratic() const { return m_quadratic; }

	void draw(std::string param = "default") override
	{
		if (m_check_draw)
		{
			NoEngine::Renderer_OpenGL::disable_poligon_mode();
			if (param == "stencil")
			{
				p_shader->bind();
				p_shader->set_bool("stencil", true);
				p_shader->set_matrix4("model", get_model_matrix());

				NoEngine::Renderer_OpenGL::draw(*p_vao);
			}
			else
			{
				p_shader->bind();
				p_shader->set_bool("stencil", false);
				p_shader->set_matrix4("model", get_model_matrix());

				NoEngine::Renderer_OpenGL::draw(*p_vao);
			}
			p_vao->unbind();
			NoEngine::Renderer_OpenGL::enable_poligon_mode();
		}
	}

	bool intersect(const glm::vec3& ray_origin, const glm::vec3& ray_direction, float& distance) const override
	{
		glm::vec3 world_sphere_center = glm::vec3(get_model_matrix() * glm::vec4(m_local_center, 1.0f));
		float world_radius = (m_radius + 0.2) * glm::max(glm::max(get_scale().x, get_scale().y), get_scale().z);

		glm::vec3 oc = ray_origin - world_sphere_center;
		float a = glm::dot(ray_direction, ray_direction);
		float b = 2.0f * glm::dot(oc, ray_direction);
		float c = glm::dot(oc, oc) - world_radius * world_radius;
		float discriminant = b * b - 4 * a * c;

		return (discriminant > 0);
	}

private:
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indices;

	int m_sectors = 24;
	int m_stacks = 10;
	float m_radius = 0.2f;

	glm::vec3 m_local_center = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_color;
	float m_constant;
	float m_linear;
	float m_quadratic;

	std::unique_ptr<NoEngine::VertexArray> p_vao;
	std::unique_ptr<NoEngine::VertexBuffer> p_position_vbo;
	std::unique_ptr<NoEngine::IndexBuffer> p_index_buffer;

	NoEngine::BufferLayout buffer_layout_vec3_vec3_vec2
	{
		NoEngine::ShaderDataType::Float3,
		NoEngine::ShaderDataType::Float3,
		NoEngine::ShaderDataType::Float2
	};

	void generate_sphere()
	{
		for (int i = 0; i <= m_stacks; ++i)
		{
			// Угол наклона (от верхнего полюса к нижнему)
			float stackAngle = M_PI / 2 - i * M_PI / m_stacks;
			float xy = m_radius * cosf(stackAngle); // Проекция на плоскость XZ
			float z = m_radius * sinf(stackAngle); // Координата Z

			for (int j = 0; j <= m_sectors; ++j) {
				// Угол поворота вокруг оси Y
				float sectorAngle = j * 2 * M_PI / m_sectors; // От 0 до 2π

				// Координаты вершины
				float x = xy * cosf(sectorAngle); // Координата X
				float y = xy * sinf(sectorAngle); // Координата Y

				// Позиция вершины
				m_vertices.push_back(x);
				m_vertices.push_back(y);
				m_vertices.push_back(z);

				// Нормаль (совпадает с координатами вершины, так как сфера центрирована)
				m_vertices.push_back(x / m_radius);
				m_vertices.push_back(y / m_radius);
				m_vertices.push_back(z / m_radius);

				// UV-координаты
				m_vertices.push_back((float)j / m_sectors); // U (0..1)
				m_vertices.push_back((float)i / m_stacks);  // V (0..1)
			}
		}

		for (int i = 0; i < m_stacks; ++i) {
			for (int j = 0; j < m_sectors; ++j) {
				// Индексы вершин текущего четырёхугольника
				int first = (i * (m_sectors + 1)) + j;
				int second = first + m_sectors + 1;

				// Первый треугольник
				m_indices.push_back(first);
				m_indices.push_back(second);
				m_indices.push_back(first + 1);

				// Второй треугольник
				m_indices.push_back(second);
				m_indices.push_back(second + 1);
				m_indices.push_back(first + 1);
			}
		}
	}
};