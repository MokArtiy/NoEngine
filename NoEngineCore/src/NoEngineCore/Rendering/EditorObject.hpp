#pragma once

#include "Actor.hpp"
#include "Cube.hpp"

#include <unordered_map>
#include <map>
#include <string>
#include <vector>

namespace NoEngine {

	enum class ObjectType
	{
		Cube,
		Piramide
	};

	class EditorScene
	{
	public:
		static void add_object(
			std::shared_ptr<NoEngine::ShaderProgram> shader,
			ObjectType type = ObjectType::Cube,
			std::string object_name = "",
			const glm::vec3& position = glm::vec3(0.0f),
			const glm::vec3& rotation = glm::vec3(0.0f),
			const glm::vec3& scale = glm::vec3(1.0f)
		);

		static void remove_object(std::string object_name);

		static void draw_objects();

	private:
		static std::unordered_map<std::string, std::shared_ptr<Actor>> m_scene_objects;
		static std::map<std::string, std::vector<std::string>> m_scene_objects_names;
	};
}
