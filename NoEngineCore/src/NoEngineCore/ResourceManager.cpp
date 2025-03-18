#include "NoEngineCore/ResourceManager.hpp"
#include "Rendering/EditorObject.hpp"
#include "NoEngineCore/Log.hpp"

#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include <sstream>
#include <fstream>

namespace NoEngine {
	std::string ResourceManager::m_path;
	std::unordered_map<const char*, std::shared_ptr<NoEngine::ShaderProgram>> ResourceManager::m_shader_programs;
	std::unordered_map<const char*, std::shared_ptr<NoEngine::Texture2D>> ResourceManager::m_textures;

	ResourceManager::ResourceManager(std::string executablePath)
	{
		size_t found = executablePath.find_last_of("/\\");
		m_path = executablePath.substr(0, found);
	}

	std::shared_ptr<NoEngine::ShaderProgram> ResourceManager::load_shader(NoEngine::TypeShader type, const char* shader_name, const char* vertex_shader_path, const char* fragment_shader_path)
	{
		std::string vertex_string = get_file_string(vertex_shader_path);
		if (vertex_string.empty())
		{
			LOG_CRITICAL("No vertex shader!");
			return nullptr;
		}

		std::string fragment_string = get_file_string(fragment_shader_path);
		if (fragment_string.empty())
		{
			LOG_CRITICAL("No fragment shader!");
			return nullptr;
		}

		std::shared_ptr<NoEngine::ShaderProgram>& new_shader_program = m_shader_programs.emplace(shader_name, std::make_shared<NoEngine::ShaderProgram>(vertex_string.c_str(), fragment_string.c_str(), type)).first->second;
		if (new_shader_program->isCompiled())
		{
			return new_shader_program;
		}

		LOG_CRITICAL("Can't load shader program:\n    Vertex: {0}\n    Fragment: {1}", vertex_shader_path, fragment_shader_path);
		return nullptr;
	}

	std::shared_ptr<NoEngine::Texture2D> ResourceManager::load_texture(const char* texture_name, const char* texture_path, bool set_flip)
	{
		int width = 0;
		int height = 0;
		int channels = 0;
		if (set_flip)
		{
			stbi_set_flip_vertically_on_load(true);
		}
		else
		{
			stbi_set_flip_vertically_on_load(false);
		}
		unsigned char* data = stbi_load(std::string(m_path + "/" + texture_path).c_str(), &width, &height, &channels, 0);
		if (!data)
		{
			LOG_CRITICAL("Failed to load texture: {0}\n    Error: {1}", texture_path, stbi_failure_reason());
			return nullptr;
		}
		std::shared_ptr<NoEngine::Texture2D> new_texture = m_textures.emplace(texture_name, std::make_shared<NoEngine::Texture2D>(data, width, height, channels)).first->second;
		stbi_image_free(data);
		return new_texture;
	}

	void ResourceManager::update_variables_shaders(glm::mat4 view_matrix, glm::mat4 projection_matrix, glm::vec3 camera_position, bool check_dirlight, std::array<std::array<float, 3>, 4> dirlight_variables)
	{
		for (auto& i : m_shader_programs)
		{
			NoEngine::TypeShader type = i.second->get_type();
			int count_light = 0;
			switch (type)
			{
			case NoEngine::TypeShader::Object:
				i.second->bind();
				i.second->set_matrix4("projection_view_matrix", projection_matrix * view_matrix);
				i.second->set_vec3("view_position", camera_position);

				i.second->set_bool("check_dirLight", check_dirlight);
				if (check_dirlight)
				{
					i.second->set_vec3("dirLight.direction", glm::vec3(dirlight_variables[0][0], dirlight_variables[0][1], dirlight_variables[0][2]));
					i.second->set_vec3("dirLight.ambient", glm::vec3(dirlight_variables[1][0], dirlight_variables[1][1], dirlight_variables[1][2]));
					i.second->set_vec3("dirLight.diffuse", glm::vec3(dirlight_variables[2][0], dirlight_variables[2][1], dirlight_variables[2][2]));
					i.second->set_vec3("dirLight.specular", glm::vec3(dirlight_variables[3][0], dirlight_variables[3][1], dirlight_variables[3][2]));
				}

				for (auto& light : NoEngine::EditorScene::get_scene_lights())
				{
					std::string point_number = std::to_string(count_light);
					i.second->set_int("POINT_COUNT", count_light + 1);
					i.second->set_vec3(("pointLights[" + point_number + "].position").c_str(), light.second->get_position());
					i.second->set_vec3(("pointLights[" + point_number + "].ambient").c_str(), light.second->get_ambient());
					i.second->set_vec3(("pointLights[" + point_number + "].diffuse").c_str(), light.second->get_diffuse());
					i.second->set_vec3(("pointLights[" + point_number + "].specular").c_str(), light.second->get_specular());
					i.second->set_float(("pointLights[" + point_number + "].constant").c_str(), light.second->get_constant());
					i.second->set_float(("pointLights[" + point_number + "].linear").c_str(), light.second->get_linear());
					i.second->set_float(("pointLights[" + point_number + "].quadratic").c_str(), light.second->get_quadratic());
					++count_light;
				}
				break;
			case NoEngine::TypeShader::PointLight:
				i.second->bind();
				i.second->set_matrix4("projection_view", projection_matrix * view_matrix);
				break;
			case NoEngine::TypeShader::Grid:
				i.second->bind();
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f));
				i.second->set_matrix4("mvp", projection_matrix * view_matrix * model);
				break;
			case NoEngine::TypeShader::Outline:
				i.second->bind();
				i.second->set_matrix4("vp", projection_matrix * view_matrix);
				break;
			default:
				break;
			}
		}
	}

	std::string ResourceManager::get_file_string(const char* relative_file_path)
	{
		std::ifstream f;
		f.open(m_path + "/" + relative_file_path, std::ios::in | std::ios::binary);
		if (!f.is_open())
		{
			LOG_CRITICAL("Failed to open file: {0} | {1}", relative_file_path, m_path);
			return std::string{};
		}

		std::stringstream buffer;
		buffer << f.rdbuf();
		return buffer.str();
	}

}