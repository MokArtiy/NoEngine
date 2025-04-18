#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <array>

#include <stb_image/include/stb_image.h>

#include "../NoEngineCore/src/NoEngineCore/Rendering/OpenGL/ShaderProgram.hpp"
#include "../NoEngineCore/src/NoEngineCore/Rendering/OpenGL/Texture2D.hpp"

namespace NoEngine {
	class ResourceManager
	{
	public:
		ResourceManager(std::string executable_path);
		~ResourceManager() = default;

		ResourceManager(const ResourceManager&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;
		ResourceManager& operator=(ResourceManager&&) = delete;
		ResourceManager(ResourceManager&&) = delete;

		static std::shared_ptr<NoEngine::ShaderProgram> load_shader(NoEngine::TypeShader type, const char* shader_name, const char* vertex_shader_path, const char* fragment_shader_path);
		static std::shared_ptr<NoEngine::Texture2D> load_texture(const char* texture_name, const char* texture_path, bool set_flip = true);
		static std::string get_path() { return m_path + "/"; }
		static void update_variables_shaders(
			glm::mat4 view_matrix,
			glm::mat4 projection_matrix,
			glm::vec3 camera_position,
			bool check_dirlight,
			std::array<std::array<float, 3>, 4> dirlight_variables
		);

	private:
		static std::string get_file_string(const char* relative_file_path);

		static std::unordered_map<const char*, std::shared_ptr<NoEngine::ShaderProgram>> m_shader_programs;
		static std::unordered_map<const char*, std::shared_ptr<NoEngine::Texture2D>> m_textures;
		
		static std::string m_path;
	};
}