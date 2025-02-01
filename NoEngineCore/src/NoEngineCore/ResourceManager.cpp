#include "NoEngineCore/ResourceManager.hpp"
#include "NoEngineCore/Log.hpp"

#include <sstream>
#include <fstream>

namespace NoEngine {

	ResourceManager::ResourceManager(std::string executablePath)
	{
		size_t found = executablePath.find_last_of("/\\");
		m_path = executablePath.substr(0, found);
	}

	std::shared_ptr<NoEngine::ShaderProgram> ResourceManager::load_shader(const char* shader_name, const char* vertex_shader_path, const char* fragment_shader_path)
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

		std::shared_ptr<NoEngine::ShaderProgram>& new_shader_program = m_shader_programs.emplace(shader_name, std::make_shared<NoEngine::ShaderProgram>(vertex_string.c_str(), fragment_string.c_str())).first->second;
		if (new_shader_program->isCompiled())
		{
			return new_shader_program;
		}

		LOG_CRITICAL("Can't load shader program:\n    Vertex: {0}\n    Fragment: {1}", vertex_shader_path, fragment_shader_path);
		return nullptr;
	}

	std::string ResourceManager::get_file_string(const char* relative_file_path) const
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