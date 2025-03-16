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
		Plane,
		Cube,
		Sphere
	};
	enum class EngineState
	{
		Run,
		Pause,
		Stop
	};

	class EditorScene
	{
	public:
		static void add_object(
			std::shared_ptr<NoEngine::ShaderProgram> outline_shader,
			std::shared_ptr<NoEngine::ShaderProgram> shader,
			ObjectType type = ObjectType::Cube,
			std::string object_name = "",
			const glm::vec3& position = glm::vec3(0.0f),
			const glm::vec3& rotation = glm::vec3(0.0f),
			const glm::vec3& scale = glm::vec3(1.0f)
		);
		static void remove_object();
		static void draw_objects();
		static void update_objets(float deltaTime, EngineState state);
		static void pick_object(const glm::vec2& mouse_pos, glm::mat4 view_matrix, glm::mat4 projection, glm::vec3 camera_position);
		static std::shared_ptr<Actor> get_selected_obj();
		static glm::vec3 get_selected_location();
		static void set_selected_location(float x, float y, float z);
		static glm::vec3 get_selected_rotation();
		static void set_selected_rotation(float x, float y, float z);
		static glm::vec3 get_selected_scale();
		static void set_selected_scale(float x, float y, float z);

	private:
		static std::unordered_map<std::string, std::shared_ptr<Actor>> m_scene_objects;
		static std::map<std::string, std::vector<std::string>> m_scene_objects_names;
		static bool m_saving_scene;
	};
}
