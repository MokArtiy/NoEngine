#include "NoEngineCore/Application.hpp"
#include "NoEngineCore/Log.hpp"
#include "NoEngineCore/Window.hpp"
#include "NoEngineCore/Event.hpp"
#include "NoEngineCore/Input.hpp"
#include "NoEngineCore/ResourceManager.hpp"

#include "NoEngineCore/Rendering/OpenGL/ShaderProgram.hpp"
#include "NoEngineCore/Rendering/OpenGL/Texture2D.hpp"
#include "NoEngineCore/Rendering/OpenGL/VertexBuffer.hpp"
#include "NoEngineCore/Rendering/OpenGL/VertexArray.hpp"
#include "NoEngineCore/Rendering/OpenGL/IndexBuffer.hpp"
#include "NoEngineCore/Rendering/UI-Utils/Grid.hpp"
#include "NoEngineCore/Camera.hpp"
#include "NoEngineCore/Rendering/OpenGL/Renderer_OpenGL.hpp"
#include "NoEngineCore/Rendering/EditorObject.hpp"
#include "NoEngineCore/Modules/UIModule.hpp"
#include "NoEngineCore/Rendering/OpenGL/FrameBuffer.hpp"
#include "NoEngineCore/Physics/PhysicsSystem.hpp"

#include <imgui/imgui.h>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

namespace NoEngine {
	std::shared_ptr<FrameBuffer> p_frame_buffer;

	std::shared_ptr<ShaderProgram> p_new_shader;
	std::shared_ptr<ShaderProgram> p_outline_shader;
	std::shared_ptr<ShaderProgram> p_grid_shader;
	std::shared_ptr<ShaderProgram> p_light_point_shader;

	std::unique_ptr<VertexBuffer> p_cube_position_vbo;
	std::unique_ptr<IndexBuffer> p_cube_index_buffer;
	std::shared_ptr<Texture2D> p_texture_smile;
	std::shared_ptr<Texture2D> p_texture_container;
	std::shared_ptr<Texture2D> p_texture_container_specular;
	std::unique_ptr<VertexArray> p_cube_vao;
	std::unique_ptr<Grid> p_grid;
	/*float scale[3] = { 1.f, 1.f, 1.f };
	float rotate = 0.f;
	float translate[3] = { 0.f, 0.f, 0.f };*/

	std::array<glm::vec3, 5> positions = {
		glm::vec3(-2.f, -2.f, -4.f),
		glm::vec3(-5.f,  0.f,  3.f),
		glm::vec3(2.f,  1.f, -2.f),
		glm::vec3(10.f, -7.f,  3.f),
		glm::vec3(1.f, -7.f,  1.f)
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.0f,  0.0f,  1.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};


	Application::Application()
	{
		LOG_INFO("Staring Application");
	}

	Application::~Application()
	{
		LOG_INFO("Closing Application");
	}

	int Application::start(unsigned int window_width, unsigned int window_height, const char* title, std::string executable_path)
	{
		m_executable_path = executable_path;
		ResourceManager p_resource_manager(m_executable_path);
		m_pWindow = std::make_unique<Window>(title, window_width, window_height);
		camera.set_viewport_size(static_cast<float>(window_width), static_cast<float>(window_height));

		m_event_dispatcher.add_event_listener<EventMouseMoved>(
			[](EventMouseMoved& event)
			{
				//LOG_INFO("[MouseMoved] Mouse moved to {0}x{1}", event.x, event.y);
			});

		m_event_dispatcher.add_event_listener<EventWindowResize>(
			[&](EventWindowResize& event)
			{
				LOG_INFO("[Resized] Changed size to {0}x{1}", event.width, event.height);
				p_frame_buffer->create(event.width, event.height);
				camera.set_viewport_size(event.width, event.height);
				draw();
			});

		m_event_dispatcher.add_event_listener<EventWindowClose>(
			[&](EventWindowClose& event)
			{
				LOG_INFO("[WindowClose]");
				close();
			});
		
		m_event_dispatcher.add_event_listener<EventMouseButtonPressed>(
			[&](EventMouseButtonPressed& event)
			{
				LOG_INFO("Mouse button pressed: {0}, at ({1}, {2})", static_cast<int>(event.mouse_button), event.x_pos, event.y_pos);
				Input::PressMouseButton(event.mouse_button);
				on_mouse_button_event(event.mouse_button, event.x_pos, event.y_pos, true);
			});

		m_event_dispatcher.add_event_listener<EventMouseButtonReleased>(
			[&](EventMouseButtonReleased& event)
			{
				LOG_INFO("Mouse button released: {0}, at ({1}, {2})", static_cast<int>(event.mouse_button), event.x_pos, event.y_pos);
				Input::ReleaseMouseButton(event.mouse_button);
				on_mouse_button_event(event.mouse_button, event.x_pos, event.y_pos, false);
			});

		m_event_dispatcher.add_event_listener<EventKeyPressed>(
			[&](EventKeyPressed& event)
			{
				if (event.ket_code <= KeyCode::KEY_Z)
				{
					if (event.repeated)
					{
						LOG_INFO("Key pressed: {0}, repeated", static_cast<char>(event.ket_code));
					}
					else
					{
						LOG_INFO("Key pressed: {0}", static_cast<char>(event.ket_code));
					}
				}
				Input::PressKey(event.ket_code);
			});

		m_event_dispatcher.add_event_listener<EventKeyReleased>(
			[&](EventKeyReleased& event)
			{
				if (event.ket_code <= KeyCode::KEY_Z)
				{
					LOG_INFO("Key release: {0}", static_cast<char>(event.ket_code));
				}
				Input::ReleaseKey(event.ket_code);
			});

		m_pWindow->set_event_callback(
			[&](BaseEvent& event)
			{
				m_event_dispatcher.dispatch(event);
			}
		);

		on_editor_init();
		//-------------------------------------//
		p_new_shader = ResourceManager::load_shader(NoEngine::TypeShader::Object, "p_new_shader", "res/shaders/default_obj.vert", "res/shaders/multiple_lights.frag");
		p_outline_shader = ResourceManager::load_shader(NoEngine::TypeShader::Outline, "p_outline_shader", "res/shaders/object_outlining.vert", "res/shaders/object_outlining.frag");
		p_grid_shader = ResourceManager::load_shader(NoEngine::TypeShader::Grid, "p_grid_shader", "res/shaders/grid.vert", "res/shaders/grid.frag");
		p_light_point_shader = ResourceManager::load_shader(NoEngine::TypeShader::PointLight, "p_light_point_shader", "res/shaders/light_source.vert", "res/shaders/light_source.frag");
		
		p_texture_container = ResourceManager::load_texture("container_texture", "res/textures/container_iron.png");
		p_texture_container->bind(1);
		p_texture_container_specular = ResourceManager::load_texture("container_specular", "res/textures/container_specular.png");
		p_texture_container_specular->bind(2);

		BufferLayout buffer_layout_1vec3
		{
			ShaderDataType::Float3
		};

		BufferLayout buffer_layout_vec3_vec3_vec2
		{
			ShaderDataType::Float3,
			ShaderDataType::Float3,
			ShaderDataType::Float2
		};

		p_grid = std::make_unique<Grid>();
		p_frame_buffer = std::make_shared<FrameBuffer>();

		p_frame_buffer->create(m_pWindow->get_width(), m_pWindow->get_height());
		//-------------------------------------//
		double last_time = glfwGetTime();
		Renderer_OpenGL::enable_depth_testing();
		while (!m_bCloseWindow)
		{
			current_frame = glfwGetTime();
			delta_time = current_frame - last_time;
			last_time = current_frame;

			draw();
		}
		m_pWindow = nullptr;

        return 0;
    }

	void Application::close()
	{
		m_bCloseWindow = true;
	}

	glm::vec2 Application::get_current_cursor_position() const
	{
		return m_pWindow->get_current_cursor_position();
	}

	glm::vec2 Application::get_current_cursor_position_in_scene() const
	{
		return m_cursor_pos_in_scene;
	}

	bool Application::check_cursor_in_scene()
	{
		return m_cursor_in_scene;
	}

	void Application::add_editor_object(int object_type, std::string object_name, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	{
		switch (object_type)
		{
		case NE_PLANE:
			EditorScene::add_object(p_outline_shader, p_new_shader, ObjectType::Plane, object_name, position, rotation, scale);
			break;
		case NE_CUBE:
			EditorScene::add_object(p_outline_shader, p_new_shader, ObjectType::Cube, object_name, position, rotation, scale);
			break;
		case NE_SPHERE:
			EditorScene::add_object(p_outline_shader, p_new_shader, ObjectType::Sphere, object_name, position, rotation, scale);
			break;
		case NE_POINT_LIGHT:
			EditorScene::add_object(p_outline_shader, p_light_point_shader, ObjectType::PointLight, object_name, position, rotation, scale);
			break;
		}
	}

	void Application::remove_editor_object()
	{
		EditorScene::remove_object();
	}

	void Application::draw_main_scene()
	{
		p_frame_buffer->unbind();

		ImGui::Begin("Scene");

		const float window_width = ImGui::GetContentRegionAvail().x;
		const float window_height = ImGui::GetContentRegionAvail().y;

		camera.set_viewport_size(window_width, window_height);

		if (ImGui::IsWindowHovered()) {
			m_cursor_in_scene = true;
		}
		else {
			m_cursor_in_scene = false;
		}
		NoEngine::PhysicsSystem::set_window_size(glm::vec2(window_width, window_height));
		ImVec2 mouse_pos = ImGui::GetMousePos();
		ImVec2 scene_pos = ImGui::GetWindowPos();
		ImVec2 scene_size = ImGui::GetWindowSize();
		m_cursor_pos_in_scene = glm::vec2(mouse_pos.x - scene_pos.x, mouse_pos.y - scene_pos.y);

		GLuint textureID = p_frame_buffer->get_texture_id();
		ImGui::Image(textureID, ImVec2(window_width, window_height), ImVec2(0, 1), ImVec2(1, 0));

		ImVec2 content_avail = ImGui::GetContentRegionAvail();

		button_scene_pos = glm::vec2(content_avail.x, content_avail.y);
		on_ui_draw_in_scene();

		ImGui::End();
	}

	void Application::pick_object(glm::vec2 mouse_pos)
	{
		EditorScene::pick_object(mouse_pos, camera.get_view_matrix(), camera.get_projection_matrix(), camera.get_position());
	}

	void Application::set_check_lights(bool check)
	{
		EditorScene::set_draw_light(check);
	}

	glm::vec3 Application::get_selected_object_location()
	{
		return EditorScene::get_selected_location();
	}

	void Application::set_selected_object_location(float x, float y, float z)
	{
		EditorScene::set_selected_location(x, y, z);
	}

	glm::vec3 Application::get_selected_object_rotation()
	{
		return EditorScene::get_selected_rotation();
	}

	void Application::set_selected_object_rotation(float x, float y, float z)
	{
		EditorScene::set_selected_rotation(x, y, z);
	}

	glm::vec3 Application::get_selected_object_scale()
	{
		return EditorScene::get_selected_scale();
	}

	void Application::set_selected_object_scale(float x, float y, float z)
	{
		EditorScene::set_selected_scale(x, y, z);
	}

	//material

	float Application::get_selected_object_constant()
	{
		return EditorScene::get_selected_constant();
	}

	void Application::set_selected_object_constant(float constant)
	{
		EditorScene::set_selected_constant(constant);
	}

	float Application::get_selected_object_linear()
	{
		return EditorScene::get_selected_linear();
	}

	void Application::set_selected_object_linear(float linear)
	{
		EditorScene::set_selected_linear(linear);
	}

	float Application::get_selected_object_quadratic()
	{
		return EditorScene::get_selected_quadratic();
	}

	void Application::set_selected_object_quadratic(float quadratic)
	{
		EditorScene::set_selected_quadratic(quadratic);
	}

	glm::vec3 Application::get_selected_object_color()
	{
		return EditorScene::get_selected_color();
	}

	void Application::set_selected_object_color(float r, float g, float b)
	{
		EditorScene::set_selected_color(r, g, b);
		EditorScene::set_selected_ambient(r * 0.1, g * 0.1, b * 0.1);
		EditorScene::set_selected_diffuse(r, g, b);
		EditorScene::set_selected_specular(r, g, b);
	}

	glm::vec3 Application::get_selected_object_ambient()
	{
		return EditorScene::get_selected_ambient();
	}

	void Application::set_selected_object_ambient(float r, float g, float b)
	{
		EditorScene::set_selected_ambient(r, g, b);
	}

	glm::vec3 Application::get_selected_object_diffuse()
	{
		return EditorScene::get_selected_diffuse();
	}

	void Application::set_selected_object_diffuse(float r, float g, float b)
	{
		EditorScene::set_selected_diffuse(r, g, b);
	}

	glm::vec3 Application::get_selected_object_specular()
	{
		return EditorScene::get_selected_specular();
	}

	void Application::set_selected_object_specular(float r, float g, float b)
	{
		EditorScene::set_selected_specular(r, g, b);
	}

	float Application::get_selected_object_shininess()
	{
		return EditorScene::get_selected_shininess();
	}

	void Application::set_selected_object_shininess(float shininess)
	{
		EditorScene::set_selected_shininess(shininess);
	}


	bool Application::check_selected_obj()
	{
		auto& obj = EditorScene::get_selected_obj();
		if (obj != 0)
			return true;
		return false;
	}

	int Application::get_type_selected_obj()
	{
		auto& obj = EditorScene::get_selected_obj();
		
		if (obj != 0) {
			return obj->get_obj_type();
		}

		return -1;
	}

	std::array<std::array<std::string, 3>, 3> Application::get_function_in_selected_obj()
	{
		auto& obj = EditorScene::get_selected_obj();
		if (obj != 0)
			return obj->get_update_func();
	}

	void Application::save_new_func()
	{
		if (function_editor_str[0][0] != "" || function_editor_str[0][1] != "" || function_editor_str[0][2] != ""
			|| function_editor_str[1][0] != "" || function_editor_str[1][1] != "" || function_editor_str[1][2] != ""
			|| function_editor_str[2][0] != "" || function_editor_str[2][1] != "" || function_editor_str[2][2] != "")
		{
			auto& obj = NoEngine::EditorScene::get_selected_obj();
			if (obj != 0)
			{
				obj->set_new_function(function_editor_str);
				obj->set_check_update_func(true);
			}
		}
	}

	void Application::reset_new_func()
	{
		auto& obj = NoEngine::EditorScene::get_selected_obj();
		if (obj != 0)
		{
			obj->set_new_function(function_editor_str);
			obj->set_check_update_func(false);
		}
	}

	//DRAW
	void Application::draw()
	{
		Renderer_OpenGL::set_clear_color(m_background_color[0], m_background_color[1], m_background_color[2], m_background_color[3]);
		Renderer_OpenGL::clear();
		p_frame_buffer->bind();

		glm::mat4 projection_view_matrix = camera.get_projection_matrix() * camera.get_view_matrix();

		std::array<std::array<float, 3>, 4> dirlight = {
			dirLight_direction[0], dirLight_direction[1], dirLight_direction[2],
			dirLight_ambient[0], dirLight_ambient[1], dirLight_ambient[2],
			dirLight_diffuse[0], dirLight_diffuse[1], dirLight_diffuse[2],
			dirLight_specular[0], dirLight_specular[1], dirLight_specular[2]
		};

		ResourceManager::update_variables_shaders(
			camera.get_view_matrix(),
			camera.get_projection_matrix(),
			camera.get_position(),
			check_dirLight,
			dirlight
		);

		p_new_shader->bind();
		// spotLight
		p_new_shader->set_bool("check_SpotLight", check_SpotLight);
		if (check_SpotLight)
		{
			p_new_shader->set_vec3("spotLight.position", camera.get_position());
			p_new_shader->set_vec3("spotLight.direction", camera.get_direction());
			p_new_shader->set_vec3("spotLight.ambient", spotLight_ambient);
			p_new_shader->set_vec3("spotLight.diffuse", spotLight_diffuse);
			p_new_shader->set_vec3("spotLight.specular", spotLight_specular);
			p_new_shader->set_float("spotLight.constant", spotLight_constant);
			p_new_shader->set_float("spotLight.linear", spotLight_linear);
			p_new_shader->set_float("spotLight.quadratic", spotLight_quadratic);
			p_new_shader->set_float("spotLight.cutOff", glm::cos(glm::radians(spotLight_cutOff)));
			p_new_shader->set_float("spotLight.outerCutOff", glm::cos(glm::radians(spotLight_outerCutOff)));
		}

		//DRAW
		Renderer_OpenGL::set_stencil_mask(0x00);

		if (check_grid)
		{
			p_grid->draw(p_grid_shader);
		}

		switch (current_state_scene)
		{
		case NE_STOP:
			EditorScene::update_objets(current_frame, delta_time, NoEngine::EngineState::Stop);
			break;
		case NE_RUN:
			EditorScene::update_objets(current_frame, delta_time, NoEngine::EngineState::Run);
			break;
		case NE_PAUSE:
			EditorScene::update_objets(current_frame, delta_time, NoEngine::EngineState::Pause);
			break;
		default:
			EditorScene::update_objets(current_frame, delta_time, NoEngine::EngineState::Stop);
			break;
		}
		EditorScene::draw_objects();

		p_frame_buffer->unbind();

		UIModule::on_ui_draw_begin();
		on_ui_draw();
		draw_main_scene();

		UIModule::on_ui_draw_end();

		m_pWindow->on_update();
		on_update();
	}
}