#pragma once

#include <memory>
#include <string>
#include <unordered_map>

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

		std::shared_ptr<NoEngine::ShaderProgram> load_shader(const char* shader_name, const char* vertex_shader_path, const char* fragment_shader_path);
		std::shared_ptr<NoEngine::Texture2D> load_texture(const char* texture_name, const char* texture_path);

	private:
		std::string get_file_string(const char* relative_file_path) const;

		std::unordered_map<const char*, std::shared_ptr<NoEngine::ShaderProgram>> m_shader_programs;
		std::unordered_map<const char*, std::shared_ptr<NoEngine::Texture2D>> m_textures;
		
		std::string m_path;
	};
}