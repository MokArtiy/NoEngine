#include "EditorObject.hpp"
#include "NoEngineCore/Physics/PhysicsSystem.hpp"

namespace NoEngine{

	std::unordered_map<std::string, std::shared_ptr<Actor>> EditorScene::m_scene_objects;
	std::map<std::string, std::vector<std::string>> EditorScene::m_scene_objects_names;

	void EditorScene::add_object(std::shared_ptr<NoEngine::ShaderProgram> outline_shader,
		std::shared_ptr<NoEngine::ShaderProgram> shader, ObjectType type,
		std::string object_name, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	{
		std::string shape_name= "";

		switch (type)
		{
		case NoEngine::ObjectType::Plane:
			shape_name = "Plane";
			break;
		case NoEngine::ObjectType::Cube:
			shape_name = "Cube";
			break;
		case NoEngine::ObjectType::Sphere:
			shape_name = "Sphere";
			break;
		default:
			break;
		}

		if (object_name == "")
		{
			std::string count;
			if (m_scene_objects_names.count(shape_name) != 0)
				count = std::to_string(m_scene_objects_names.find(shape_name)->second.size());
			else
				count = "0";
			object_name = shape_name + "_" + count;
		}

		for (const auto& objs : m_scene_objects)
		{
			if (objs.second->get_selected())
			{
				objs.second->set_selected(false);
			}
		}

		if (m_scene_objects_names.count(shape_name) == 0)
		{
			m_scene_objects_names.emplace(shape_name, std::vector<std::string>{object_name});
		}
		else
		{
			m_scene_objects_names[shape_name].push_back(object_name);
		}

		switch (type)
		{
		case NoEngine::ObjectType::Plane:
			
			break;
		case NoEngine::ObjectType::Cube:
			m_scene_objects.emplace(object_name, std::make_shared<Cube>(shader, outline_shader, position, rotation, scale, object_name));
			break;
		case NoEngine::ObjectType::Sphere:
			
			break;
		default:
			break;
		}

		LOG_INFO("Add object: {0} (X:{1} Y:{2} Z:{3})", object_name, position.x, position.y, position.z);
	}

	void EditorScene::remove_object()
	{
		for (auto it = begin(m_scene_objects); it != end(m_scene_objects);)
		{
			if (it->second->get_selected())
			{
				for (auto pair : m_scene_objects_names)
				{
					auto& names = pair.second; 
					names.erase(std::remove(names.begin(), names.end(), it->second->get_name()), names.end());
				}
				LOG_INFO("Remove object: {0} (X:{1} Y:{2} Z:{3})", it->second->get_name(), it->second->get_position().x, it->second->get_position().y, it->second->get_position().z);
				it = m_scene_objects.erase(it);
			}
			else {
				++it;
			}
		}
	}

	void EditorScene::draw_objects()
	{
		if (m_scene_objects.size() != 0)
		{
			for (auto i : m_scene_objects)
			{
				NoEngine::Renderer_OpenGL::configurate_opengl();
				Renderer_OpenGL::set_stencil_func(StencilFunc::Always, 1, 0xFF);
				Renderer_OpenGL::set_stencil_mask(0xFF);
				NoEngine::Renderer_OpenGL::clear_stencil();

				i.second->draw();
				
				if (i.second->get_selected())
				{
					
					NoEngine::Renderer_OpenGL::set_stencil_func(NoEngine::StencilFunc::Notequal, 1, 0xFF);
					NoEngine::Renderer_OpenGL::set_stencil_mask(0x00);
					NoEngine::Renderer_OpenGL::disable_depth_testing();

					i.second->draw("stencil");

					NoEngine::Renderer_OpenGL::set_stencil_mask(0xFF);
					NoEngine::Renderer_OpenGL::set_stencil_func(NoEngine::StencilFunc::Always, 0, 0xFF);
					NoEngine::Renderer_OpenGL::enable_depth_testing();
					NoEngine::Renderer_OpenGL::disable_stencil_testing();

					NoEngine::Renderer_OpenGL::clear_stencil();
				}
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
				for (const auto& objs : m_scene_objects)
				{
					if (objs.second->get_selected())
					{
						objs.second->set_selected(false);
					}
				}
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
	std::shared_ptr<Actor> EditorScene::get_selected_obj()
	{
		for (const auto& obj : m_scene_objects)
		{
			if (obj.second->get_selected())
			{
				return obj.second;
			}
		}
		return 0;
	}
	glm::vec3 EditorScene::get_selected_location()
	{
		auto& obj = get_selected_obj();
		if (obj != 0)
		{
			return obj->get_position();
		}
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}
	void EditorScene::set_selected_location(float x, float y, float z)
	{
		auto& obj = get_selected_obj();
		if (obj != 0)
		{
			obj->set_position(glm::vec3(x, y, z));
		}
	}
	glm::vec3 EditorScene::get_selected_rotation()
	{
		auto& obj = get_selected_obj();
		if (obj != 0)
		{
			return obj->get_rotation();
		}
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}
	void EditorScene::set_selected_rotation(float x, float y, float z)
	{
		auto& obj = get_selected_obj();
		if (obj != 0)
		{
			obj->set_rotation(glm::vec3(x, y, z));
		}
	}
	glm::vec3 EditorScene::get_selected_scale()
	{
		auto& obj = get_selected_obj();
		if (obj != 0)
		{
			return obj->get_scale();
		}
		return glm::vec3(1.0f, 1.0f, 1.0f);
	}
	void EditorScene::set_selected_scale(float x, float y, float z)
	{
		auto& obj = get_selected_obj();
		if (obj != 0)
		{
			obj->set_scale(glm::vec3(x, y, z));
		}
	}
}