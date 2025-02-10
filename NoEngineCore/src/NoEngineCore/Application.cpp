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
#include "NoEngineCore/Camera.hpp"
#include "NoEngineCore/Rendering/OpenGL/Renderer_OpenGL.hpp"
#include "NoEngineCore/Modules/UIModule.hpp"

#include <imgui/imgui.h>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

namespace NoEngine {

	GLfloat pos_norm_uv[] = {
		//    position             normal            UV                  index
		// FRONT
		-1.0f, -1.f, -1.f,    -1.f,  0.f,  0.f,     0.f, 0.f,              // 0
		-1.0f,  1.f, -1.f,    -1.f,  0.f,  0.f,     1.f, 0.f,              // 1
		-1.0f,  1.f,  1.f,    -1.f,  0.f,  0.f,     1.f, 1.f,              // 2
		-1.0f, -1.f,  1.f,    -1.f,  0.f,  0.f,     0.f, 1.f,              // 3
		// BACK                                  
		 1.0f, -1.f, -1.f,     1.f,  0.f,  0.f,     1.f, 0.f,              // 4
		 1.0f,  1.f, -1.f,     1.f,  0.f,  0.f,     0.f, 0.f,              // 5
		 1.0f,  1.f,  1.f,     1.f,  0.f,  0.f,     0.f, 1.f,              // 6
		 1.0f, -1.f,  1.f,     1.f,  0.f,  0.f,     1.f, 1.f,              // 7
		 // RIGHT
		 -1.0f,  1.f, -1.f,     0.f,  1.f,  0.f,     0.f, 0.f,              // 8
		  1.0f,  1.f, -1.f,     0.f,  1.f,  0.f,     1.f, 0.f,              // 9
		  1.0f,  1.f,  1.f,     0.f,  1.f,  0.f,     1.f, 1.f,              // 10
		 -1.0f,  1.f,  1.f,     0.f,  1.f,  0.f,     0.f, 1.f,              // 11
		 // LEFT
		 -1.0f, -1.f, -1.f,     0.f, -1.f,  0.f,     1.f, 0.f,              // 12
		  1.0f, -1.f, -1.f,     0.f, -1.f,  0.f,     0.f, 0.f,              // 13
		  1.0f, -1.f,  1.f,     0.f, -1.f,  0.f,     0.f, 1.f,              // 14
		 -1.0f, -1.f,  1.f,     0.f, -1.f,  0.f,     1.f, 1.f,              // 15
		 // TOP
		 -1.0f, -1.f,  1.f,     0.f,  0.f,  1.f,     0.f, 0.f,              // 16
		 -1.0f,  1.f,  1.f,     0.f,  0.f,  1.f,     1.f, 0.f,              // 17
		  1.0f,  1.f,  1.f,     0.f,  0.f,  1.f,     1.f, 1.f,              // 18
		  1.0f, -1.f,  1.f,     0.f,  0.f,  1.f,     0.f, 1.f,              // 19
		  // BOTTOM
		  -1.0f, -1.f, -1.f,    0.f,  0.f, -1.f,     0.f, 1.f,              // 20
		  -1.0f,  1.f, -1.f,    0.f,  0.f, -1.f,     1.f, 1.f,              // 21
		   1.0f,  1.f, -1.f,    0.f,  0.f, -1.f,     1.f, 0.f,              // 22
		   1.0f, -1.f, -1.f,    0.f,  0.f, -1.f,     0.f, 0.f,              // 23
	};

	GLuint indices[] = {
		0,   1,  2,  2,  3,  0, // front
		4,   5,  6,  6,  7,  4, // back
		8,   9, 10, 10, 11,  8, // right
		12, 13, 14, 14, 15, 12, // left
		16, 17, 18, 18, 19, 16, // top
		20, 21, 22, 22, 23, 20  // bottom
	};

	std::shared_ptr<ShaderProgram> p_shader_program;
	std::shared_ptr<ShaderProgram> p_new_shader;
	std::shared_ptr<ShaderProgram> p_light_source_shader_program;
	std::unique_ptr<VertexBuffer> p_cube_position_vbo;
	std::unique_ptr<IndexBuffer> p_cube_index_buffer;
	std::shared_ptr<Texture2D> p_texture_smile;
	std::shared_ptr<Texture2D> p_texture_container;
	std::shared_ptr<Texture2D> p_texture_container_specular;
	//std::shared_ptr<Texture2D> p_texture_container_emission;
	std::unique_ptr<VertexArray> p_cube_vao;
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

		//-------------------------------------//
		ResourceManager p_resource_manager(m_executable_path);
		p_shader_program = p_resource_manager.load_shader("p_shader_program", "res/shaders/default_obj.vert", "res/shaders/default_obj_PointLight.frag");
		p_new_shader = p_resource_manager.load_shader("p_new_shader", "res/shaders/default_obj.vert", "res/shaders/multiple_lights.frag");
		p_light_source_shader_program = p_resource_manager.load_shader("p_light_source_shader_program", "res/shaders/light_source.vert", "res/shaders/light_source.frag");
		
		p_texture_container = p_resource_manager.load_texture("container_texture", "res/textures/container_iron.png");
		p_texture_container->bind(0);
		p_texture_container_specular = p_resource_manager.load_texture("container_specular", "res/textures/container_specular.png");
		p_texture_container_specular->bind(1);

		if (!p_light_source_shader_program->isCompiled())
		{
			return false;
		}

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

		p_cube_vao = std::make_unique<VertexArray>();
		p_cube_position_vbo = std::make_unique<VertexBuffer>(pos_norm_uv, sizeof(pos_norm_uv), buffer_layout_vec3_vec3_vec2);
		p_cube_index_buffer = std::make_unique<IndexBuffer>(indices, sizeof(indices) / sizeof(GLuint));

		p_cube_vao->add_vertex_buffer(*p_cube_position_vbo);
		p_cube_vao->set_index_buffer(*p_cube_index_buffer);
		//-------------------------------------//

		Renderer_OpenGL::enable_depth_testing();
		double previousTime = glfwGetTime();
		while (!m_bCloseWindow)
		{
			current_frame = glfwGetTime();
			draw();
		}
		m_pWindow = nullptr;

        return 0;
    }

	void Application::close()
	{
		m_bCloseWindow = true;
	}

	glm::vec2 Application::get_current_coursor_position() const
	{
		return m_pWindow->get_current_cursor_position();
	}

	void Application::draw()
	{
		Renderer_OpenGL::set_clear_color(m_background_color[0], m_background_color[1], m_background_color[2], m_background_color[3]);
		Renderer_OpenGL::clear();

		p_new_shader->bind();

		p_new_shader->set_vec3("view_position", camera.get_position());
		p_new_shader->set_float("material.shininess", 32.0f);
		p_new_shader->set_int("material.diffuse", 0);
		p_new_shader->set_int("material.specular", 1);
		// directional light
		p_new_shader->set_bool("check_dirLight", check_dirLight);
		if (check_dirLight)
		{
			p_new_shader->set_vec3("dirLight.direction", dirLight_direction);
			p_new_shader->set_vec3("dirLight.ambient", dirLight_ambient);
			p_new_shader->set_vec3("dirLight.diffuse", dirLight_diffuse);
			p_new_shader->set_vec3("dirLight.specular", dirLight_specular);
		}
		/* point lights */
		for (int i = 0; i < 4; ++i)
		{
			std::string point_number = std::to_string(i);
			p_new_shader->set_bool(("check_PointLight[" + point_number + "]").c_str(), check_pointLights[i]);
			if (check_pointLights[i])
			{
				p_new_shader->set_vec3(("pointLights[" + point_number + "].position").c_str(), pointLights_vec3[i][0]);
				p_new_shader->set_vec3(("pointLights[" + point_number + "].ambient").c_str(), pointLights_vec3[i][1]);
				p_new_shader->set_vec3(("pointLights[" + point_number + "].diffuse").c_str(), pointLights_vec3[i][2]);
				p_new_shader->set_vec3(("pointLights[" + point_number + "].specular").c_str(), pointLights_vec3[i][3]);
				p_new_shader->set_float(("pointLights[" + point_number + "].constant").c_str(), pointLights_float[i][0]);
				p_new_shader->set_float(("pointLights[" + point_number + "].linear").c_str(), pointLights_float[i][1]);
				p_new_shader->set_float(("pointLights[" + point_number + "].quadratic").c_str(), pointLights_float[i][2]);
			}
		}
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

		//cubes
		glm::mat4 model_matrix = glm::mat4(1.0f);
		glm::mat4 projection_view_matrix = camera.get_projection_matrix() * camera.get_view_matrix();
		p_new_shader->set_matrix4("model_matrix", model_matrix);
		p_new_shader->set_matrix4("projection_view_matrix", projection_view_matrix);
		int i = 0;
		for (const glm::vec3& current_position : positions)
		{
			++i;
			float angle = 0;
			angle = 20.f * glfwGetTime();
			glm::mat4 model_matrix = glm::translate(glm::mat4(1.f), current_position);
			//model_matrix = glm::translate(model_matrix, glm::vec3(-current_position.x + sqrt(current_position.y * current_position.y + current_position.x * current_position.x) * cos(current_frame * i / 2), (-current_position.y + sqrt(current_position.y * current_position.y + current_position.x * current_position.x) * (sin(current_frame * i / 2))), current_position.z));
			//model_matrix = glm::rotate(model_matrix, glm::radians(angle), glm::vec3(1.0f, 1.0f, 0.5f));
			p_new_shader->set_matrix4("model_matrix", model_matrix);
			p_new_shader->set_matrix3("normal_matrix", glm::transpose(glm::inverse(glm::mat3(model_matrix))));

			Renderer_OpenGL::draw(*p_cube_vao);
		}
		//light cubes
		p_light_source_shader_program->bind();
		p_light_source_shader_program->set_matrix4("projection_view_matrix", projection_view_matrix);
		for (int i = 0; i < 4; ++i)
		{
			if (check_pointLights[i])
			{
				glm::mat4 model_matrix = glm::translate(glm::mat4(1.f), glm::vec3(pointLights_vec3[i][0][0], pointLights_vec3[i][0][1], pointLights_vec3[i][0][2]));
				model_matrix = glm::scale(model_matrix, glm::vec3(0.2f));
				p_light_source_shader_program->set_matrix4("model_matrix", model_matrix);
				p_light_source_shader_program->set_vec3("light_color", pointLights_colors[i]);
				Renderer_OpenGL::draw(*p_cube_vao);
			}

		}

		UIModule::on_ui_draw_begin();
		on_ui_draw();
		UIModule::on_ui_draw_end();

		m_pWindow->on_update();
		on_update();
	}
}