#include "EditorObject.hpp"
#include "NoEngineCore/Physics/PhysicsSystem.hpp"

namespace NoEngine{

	std::unordered_map<std::string, std::shared_ptr<Actor>> EditorScene::m_scene_objects;
	std::map<std::string, std::vector<std::string>> EditorScene::m_scene_objects_names;
	bool EditorScene::m_saving_scene = false;

	void EditorScene::add_object(glm::mat4 view_matrix, glm::mat4 projection_matrix, glm::vec3 camera_position,
		std::shared_ptr<NoEngine::ShaderProgram> outline_shader,
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
			m_scene_objects.emplace(object_name, std::make_shared<Cube>(view_matrix, projection_matrix, camera_position, shader, outline_shader, position, rotation, scale, object_name));
			break;
		case NoEngine::ObjectType::Sphere:
			m_scene_objects.emplace(object_name, std::make_shared<Sphere>(view_matrix, projection_matrix, camera_position, shader, outline_shader, position, rotation, scale, object_name));
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

	void EditorScene::update_objets(float deltaTime, EngineState state)
	{
		switch (state)
		{
		case NoEngine::EngineState::Run:
			if (!m_saving_scene)
			{
				for (const auto& obj : m_scene_objects)
				{
					if (obj.second->get_check_update_func())
					{
						obj.second->set_default_position(obj.second->get_position());
						obj.second->set_default_rotation(obj.second->get_rotation());
						obj.second->set_default_scale(obj.second->get_scale());
					}
				}
				m_saving_scene = true;
			}
			for (const auto& obj : m_scene_objects)
			{
				if (obj.second->get_check_update_func())
				{
					obj.second->update_function(deltaTime, obj.second->get_update_func());
				}
			}
			break;
		case NoEngine::EngineState::Pause:
			for (const auto& obj : m_scene_objects)
			{
				if (obj.second->get_check_update_func())
				{
					obj.second->update_function(0, obj.second->get_update_func());
				}
			}
			break;
		case NoEngine::EngineState::Stop:
			if (m_saving_scene)
			{
				for (const auto& obj : m_scene_objects)
				{
					if (obj.second->get_check_update_func())
					{
						obj.second->set_position(obj.second->get_default_position());
						obj.second->set_rotation(obj.second->get_default_rotation());
						obj.second->set_scale(obj.second->get_default_scale());
					}
				}
				m_saving_scene = false;
			}
			else 
			{
				for (const auto& obj : m_scene_objects)
				{
					if (obj.second->get_check_update_func())
					{
						obj.second->set_default_position(obj.second->get_position());
						obj.second->set_default_rotation(obj.second->get_rotation());
						obj.second->set_default_scale(obj.second->get_scale());
					}
				}
			}
			break;
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

	void EditorScene::update_variables_shaders(
		glm::mat4 view_matrix, 
		glm::mat4 projection_matrix,
		glm::vec3 camera_position, 
		bool check_dirlight, 
		std::array<std::array<float, 3>, 4> dirlight_variables)
	{
		for (auto& obj : m_scene_objects)
		{
			auto shader = obj.second->get_shader();

			shader->bind();
			shader->set_matrix4("projection_view_matrix", projection_matrix * view_matrix);
			shader->set_vec3("view_position", camera_position);

			shader->set_bool("check_dirLight", check_dirlight);
			if (check_dirlight)
			{
				shader->set_vec3("dirLight.direction", glm::vec3(dirlight_variables[0][0], dirlight_variables[0][1], dirlight_variables[0][2]));
				shader->set_vec3("dirLight.ambient", glm::vec3(dirlight_variables[1][0], dirlight_variables[1][1], dirlight_variables[1][2]));
				shader->set_vec3("dirLight.diffuse", glm::vec3(dirlight_variables[2][0], dirlight_variables[2][1], dirlight_variables[2][2]));
				shader->set_vec3("dirLight.specular", glm::vec3(dirlight_variables[3][0], dirlight_variables[3][1], dirlight_variables[3][2]));
			}

			obj.second->set_shader(shader);
		}	
	}
}