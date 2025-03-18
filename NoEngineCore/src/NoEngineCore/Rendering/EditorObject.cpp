#include "EditorObject.hpp"
#include "NoEngineCore/Physics/PhysicsSystem.hpp"

namespace NoEngine{

	std::unordered_map<std::string, std::shared_ptr<Actor>> EditorScene::m_scene_objects;
	std::unordered_map<std::string, std::shared_ptr<PointLight>> EditorScene::m_scene_lights;
	std::map<std::string, std::vector<std::string>> EditorScene::m_scene_objects_names;
	bool EditorScene::m_saving_scene = false;

	void EditorScene::add_object(
		std::shared_ptr<NoEngine::ShaderProgram> outline_shader,
		std::shared_ptr<NoEngine::ShaderProgram> shader, ObjectType type,
		std::string object_name, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
		const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular)
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
		case NoEngine::ObjectType::PointLight:
			shape_name = "PointLight";
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

		for (const auto& objs : m_scene_lights)
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
			m_scene_objects.emplace(object_name, std::make_shared<Plane>(shader, outline_shader, NE_PLANE, position, rotation, scale, ambient, diffuse, specular, 32.f, object_name));
			break;
		case NoEngine::ObjectType::Cube:
			m_scene_objects.emplace(object_name, std::make_shared<Cube>(shader, outline_shader, NE_CUBE, position, rotation, scale, ambient, diffuse, specular, 32.f, object_name));
			break;
		case NoEngine::ObjectType::Sphere:
			m_scene_objects.emplace(object_name, std::make_shared<Sphere>(shader, outline_shader, NE_SPHERE, position, rotation, scale, ambient, diffuse, specular, 32.f, object_name));
			break;
		case NoEngine::ObjectType::PointLight:
			m_scene_lights.emplace(object_name, std::make_shared<PointLight>(shader, outline_shader, NE_POINT_LIGHT, position, rotation, scale, glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(1.0f), 32.f, object_name));
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

		for (auto it = begin(m_scene_lights); it != end(m_scene_lights);)
		{
			if (it->second->get_selected())
			{
				for (auto pair : m_scene_objects_names)
				{
					auto& names = pair.second;
					names.erase(std::remove(names.begin(), names.end(), it->second->get_name()), names.end());
				}
				LOG_INFO("Remove light: {0} (X:{1} Y:{2} Z:{3})", it->second->get_name(), it->second->get_position().x, it->second->get_position().y, it->second->get_position().z);
				it = m_scene_lights.erase(it);
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

		if (m_scene_lights.size() != 0)
		{
			for (auto i : m_scene_lights)
			{
				if (i.second->get_selected())
				{
					i.second->draw("stencil");
				}
				else {
					i.second->draw();
				}
			}
		}
	}

	void EditorScene::update_objets(float current_time, float deltaTime, EngineState state)
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
				for (const auto& obj : m_scene_lights)
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
					obj.second->update_function(current_time, deltaTime, obj.second->get_update_func());
				}
			}
			for (const auto& obj : m_scene_lights)
			{
				if (obj.second->get_check_update_func())
				{
					obj.second->update_function(current_time, deltaTime, obj.second->get_update_func());
				}
			}
			break;
		case NoEngine::EngineState::Pause:
			for (const auto& obj : m_scene_objects)
			{
				if (obj.second->get_check_update_func())
				{
					obj.second->update_function(0 ,0, obj.second->get_update_func());
				}
			}
			for (const auto& obj : m_scene_lights)
			{
				if (obj.second->get_check_update_func())
				{
					obj.second->update_function(0, 0, obj.second->get_update_func());
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
				for (const auto& obj : m_scene_lights)
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
				for (const auto& obj : m_scene_lights)
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

		std::unordered_map<std::string, std::shared_ptr<Actor>> m_all_obj;
		for (auto& obj : m_scene_objects)
		{
			m_all_obj.emplace(obj);
		}
		for (auto& obj : m_scene_lights)
		{
			m_all_obj.emplace(obj);
		}

		for (const auto& obj : m_all_obj)
		{
			float distance;
			if (obj.second->intersect(camera_position, ray_direction, distance))
			{
				for (const auto& objs : m_all_obj)
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
		std::unordered_map<std::string, std::shared_ptr<Actor>> m_all_obj;

		for (const auto& obj : m_scene_objects)
		{
			if (obj.second->get_selected())
			{
				return obj.second;
			}
		}

		for (const auto& obj : m_scene_lights)
		{
			if (obj.second->get_selected())
			{
				return obj.second;
			}
		}
		
		return 0;
	}
	std::shared_ptr<PointLight> EditorScene::get_selected_light()
	{
		for (const auto& obj : m_scene_lights)
		{
			if (obj.second->get_selected())
			{
				return obj.second;
			}
		}

		return 0;
	}
	void EditorScene::set_draw_light(bool check)
	{
		for (const auto& obj : m_scene_lights)
		{
			if (obj.second != nullptr)
			{
				obj.second->set_check_draw(check);
			}
		}
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
			if (typeid(*obj).name() == typeid(PointLight).name()) {}
			else {
				obj->set_rotation(glm::vec3(x, y, z));
			}
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
			if (typeid(*obj).name() == typeid(PointLight).name()) {}
			else {
				obj->set_scale(glm::vec3(x, y, z));
			}
		}
	}
	float EditorScene::get_selected_constant()
	{
		auto& obj = get_selected_light();
		if (obj != 0)
		{
			return obj->get_constant();
		}
		return 0.0f;
	}
	void EditorScene::set_selected_constant(float constant)
	{
		auto& obj = get_selected_light();
		if (obj != 0)
		{
			obj->set_constant(constant);
		}
	}
	float EditorScene::get_selected_linear()
	{
		auto& obj = get_selected_light();
		if (obj != 0)
		{
			return obj->get_linear();
		}
		return 0.0f;
	}
	void EditorScene::set_selected_linear(float linear)
	{
		auto& obj = get_selected_light();
		if (obj != 0)
		{
			obj->set_linear(linear);
		}
	}
	float EditorScene::get_selected_quadratic()
	{
		auto& obj = get_selected_light();
		if (obj != 0)
		{
			return obj->get_quadratic();
		}
		return 0.0f;
	}
	void EditorScene::set_selected_quadratic(float quadratic)
	{
		auto& obj = get_selected_light();
		if (obj != 0)
		{
			obj->set_quadratic(quadratic);
		}
	}
	glm::vec3 EditorScene::get_selected_color()
	{
		auto& obj = get_selected_light();
		if (obj != 0)
		{
			return obj->get_color();
		}
		return glm::vec3(0.0f);
	}
	void EditorScene::set_selected_color(float r, float g, float b)
	{
		auto& obj = get_selected_light();
		if (obj != 0)
		{
			obj->set_color(glm::vec3(r, g, b));
		}
	}
	glm::vec3 EditorScene::get_selected_ambient()
	{
		auto& obj = get_selected_obj();
		if (obj != 0)
		{
			return obj->get_ambient();
		}
		return glm::vec3(0.0f);
	}
	void EditorScene::set_selected_ambient(float r, float g, float b)
	{
		auto& obj = get_selected_obj();
		if (obj != 0)
		{
			obj->set_ambient(glm::vec3(r, g, b));
		};
	}
	glm::vec3 EditorScene::get_selected_diffuse()
	{
		auto& obj = get_selected_obj();
		if (obj != 0)
		{
			return obj->get_diffuse();
		}
		return glm::vec3(0.0f);
	}
	void EditorScene::set_selected_diffuse(float r, float g, float b)
	{
		auto& obj = get_selected_obj();
		if (obj != 0)
		{
			obj->set_diffuse(glm::vec3(r, g, b));
		}
	}
	glm::vec3 EditorScene::get_selected_specular()
	{
		auto& obj = get_selected_obj();
		if (obj != 0)
		{
			return obj->get_specular();
		}
		return glm::vec3(0.0f);
	}
	void EditorScene::set_selected_specular(float r, float g, float b)
	{
		auto& obj = get_selected_obj();
		if (obj != 0)
		{
			obj->set_specular(glm::vec3(r, g, b));
		}
	}
	float EditorScene::get_selected_shininess()
	{
		auto& obj = get_selected_obj();
		if (obj != 0)
		{
			return obj->get_shininess();
		}
		return 32.f;
	}
	void EditorScene::set_selected_shininess(float shininess)
	{
		auto& obj = get_selected_obj();
		if (obj != 0)
		{
			obj->set_shininess(shininess);
		}
	}
}