#include "EditorObject.hpp"
#include "NoEngineCore/Physics/PhysicsSystem.hpp"

namespace NoEngine{

	std::unordered_map<std::string, std::shared_ptr<Actor>> EditorScene::m_scene_objects;
	std::map<std::string, std::vector<std::string>> EditorScene::m_scene_objects_names;

	void EditorScene::add_object(std::shared_ptr<NoEngine::ShaderProgram> outline_shader,
		std::shared_ptr<NoEngine::ShaderProgram> shader, ObjectType type,
		std::string object_name, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	{
		if (object_name == "")
		{
			std::string count;
			if (m_scene_objects_names.count("Cube") != 0)
				count = std::to_string(m_scene_objects_names.find("Cube")->second.size());
			else
				count = "0";
			object_name = "Cube_" + count;
		}

		if (m_scene_objects_names.count("Cube") == 0)
		{
			m_scene_objects_names.emplace("Cube", std::vector<std::string>{object_name});
		}
		else
		{
			m_scene_objects_names["Cube"].push_back(object_name);
		}
		m_scene_objects.emplace(object_name, std::make_shared<Cube>(shader, outline_shader, position, rotation, scale, object_name));

		LOG_INFO("Add object: {0} (X:{1} Y:{2} Z:{3})", object_name, position.x, position.y, position.z);
	}

	void EditorScene::remove_object(std::string object_name)
	{
		if (m_scene_objects.size() != 0)
		{
			if (m_scene_objects.count(object_name) != 0)
			{
				auto& object = m_scene_objects.find(object_name);
				LOG_INFO("Remove object: {0} (X:{1} Y:{2} Z:{3})", object->second->get_name(), object->second->get_position().x, object->second->get_position().y, object->second->get_position().z);
				for (auto pair = m_scene_objects_names.begin(); pair != m_scene_objects_names.end(); pair++)
				{
					for (int i = 0; i < pair->second.size(); i++)
					{
						if (pair->second[i] == object_name)
						{
							pair->second.erase(pair->second.begin() + i);
							break;
						}
					}
				}
				m_scene_objects.erase(object);
			}
			else {
				LOG_CRITICAL("It is impossible to delete an object by the specified name: {0}", object_name);
			}
		}
		else {
			LOG_CRITICAL("There's not a single object on the scene right now!", object_name);
		}
		
	}

	void EditorScene::draw_objects()
	{
		if (m_scene_objects.size() != 0)
		{
			for (auto i = m_scene_objects.begin(); i != m_scene_objects.end(); i++)
			{
				i->second->draw();
			}
		}
	}

	void EditorScene::pick_object(const glm::vec2& mouse_pos, glm::mat4 view_matrix, glm::mat4 projection, glm::vec3 camera_position)
	{
		glm::vec3 ray_direction = NoEngine::PhysicsSystem::get_ray_direction(mouse_pos, view_matrix, projection);

		for (const auto& obj : m_scene_objects)
		{
			float distance;
			if (obj.second->intersect(camera_position, ray_direction, distance))
			{
				LOG_WARN("Pick object!");
				obj.second->set_selected(true);
			}
			else
			{
				if(obj.second->get_selected())
					obj.second->set_selected(false);
			}
		}
	}
}