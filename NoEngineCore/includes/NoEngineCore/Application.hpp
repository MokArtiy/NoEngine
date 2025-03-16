#pragma once

#include "NoEngineCore/Event.hpp"
#include "NoEngineCore/Camera.hpp"
#include "NoEngineCore/ResourceManager.hpp"

#include <memory>
#include <string>
#include <array>

#define NE_PLANE 0x000001
#define NE_CUBE 0x000002
#define NE_SPHERE 0x000003

#define NE_RUN 0x000004
#define NE_PAUSE 0x000005
#define NE_STOP 0x000006

namespace NoEngine {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		virtual int start(unsigned int window_width, unsigned int window_height, const char* title, std::string executable_path);
		void close();
		virtual void on_update() {}
		virtual void on_ui_draw() {}
		virtual void on_ui_draw_in_scene() {}
		virtual void on_objects_draw() {}
		virtual void on_editor_init() {}

		virtual void on_mouse_button_event(const MouseButton button_code, const double x_pos, const double y_pos, const bool pressed) {}

		glm::vec2 get_current_cursor_position() const;
		glm::vec2 get_current_cursor_position_in_scene() const;
		bool check_cursor_in_scene();

		Camera camera{ glm::vec3(-5, 5, 2), glm::vec3(0.f, 15.f, -45.f) };

		void add_editor_object(int object_type,
			std::string object_name = "",
			const glm::vec3& position = glm::vec3(0.0f),
			const glm::vec3& rotation = glm::vec3(0.0f),
			const glm::vec3& scale = glm::vec3(1.0f));
		void remove_editor_object();
		void draw_main_scene();
		void pick_object(glm::vec2 mouse_pos);
		glm::vec3 get_selected_object_location();
		void set_selected_object_location(float x, float y, float z);
		glm::vec3 get_selected_object_rotation();
		void set_selected_object_rotation(float x, float y, float z);
		glm::vec3 get_selected_object_scale();
		void set_selected_object_scale(float x, float y, float z);

		//update_func_obj
		bool check_selected_obj();
		std::array<std::array<std::string, 3>, 3> get_function_in_selected_obj();
		void save_new_func();
		void reset_new_func();
		void set_update_scene(int state) { current_state_scene = state; }
		const int get_current_state_scene() { return current_state_scene; }
		int current_state_scene = NE_STOP;

		int frameCount = 0;
		float light_source_position[3] = { 0.f, 0.f, 1.f };
		float m_background_color[4] = { 0.11f, 0.12f, 0.132f, 0.0f };
		float light_source_color[3] = { 1.f, 1.f, 1.f };
		float ambient_factor = 0.2f;
		float diffuse_factor = 0.5f;
		float specular_factor = 1.0f;
		float shininess = 64.f;
		double current_frame = 0.0;
		double delta_time = 0.0;
		bool check_shader = false;

		glm::vec2 button_scene_pos = glm::vec2(0.f, 0.f);
		std::string function_editor_str[3][3];

		//dirLight
		float dirLight_direction[3] = { -0.3f, -0.2f, -1.0f };
		float dirLight_ambient[3] = { 0.2f, 0.2f, 0.2f };
		float dirLight_diffuse[3] = { 0.7f, 0.7f, 0.7f };
		float dirLight_specular[3] = { 0.3f, 0.3f, 0.3f };
		//PointLight
		float pointLights_colors[4][3] =
		{
			{1.f, 1.f, 1.f},
			{1.f, 1.f, 1.f},
			{1.f, 1.f, 1.f},
			{1.f, 1.f, 1.f}
		};
		float pointLights_vec3[4][4][3] = 
		{
			{
				{0.0f,  0.0f,  1.0f},    //position
				{pointLights_colors[0][0] * 0.1f, pointLights_colors[0][1] * 0.1f, pointLights_colors[0][2] * 0.1f}, //ambient
				{pointLights_colors[0][0], pointLights_colors[0][1], pointLights_colors[0][2]}, //diffuse
				{pointLights_colors[0][0], pointLights_colors[0][1], pointLights_colors[0][2]} //specular
			},
			{
				{2.3f, -3.3f, -4.0f},    //position
				{pointLights_colors[1][0] * 0.1f, pointLights_colors[1][1] * 0.1f, pointLights_colors[1][2] * 0.1f}, //ambient
				{pointLights_colors[1][0], pointLights_colors[1][1], pointLights_colors[1][2]}, //diffuse
				{pointLights_colors[1][0], pointLights_colors[1][1], pointLights_colors[1][2]} //specular
			},
			{
				{-4.0f,  2.0f, -12.0f},
				{pointLights_colors[2][0] * 0.1f, pointLights_colors[2][1] * 0.1f, pointLights_colors[2][2] * 0.1f}, //ambient
				{pointLights_colors[2][0], pointLights_colors[2][1], pointLights_colors[2][2]}, //diffuse
				{pointLights_colors[2][0], pointLights_colors[2][1], pointLights_colors[2][2]} //specular
			},
			{
				{0.0f,  0.0f, -3.0f},
				{pointLights_colors[3][0] * 0.1f, pointLights_colors[3][1] * 0.1f, pointLights_colors[3][2] * 0.1f}, //ambient
				{pointLights_colors[3][0], pointLights_colors[3][1], pointLights_colors[3][2]}, //diffuse
				{pointLights_colors[3][0], pointLights_colors[3][1], pointLights_colors[3][2]} //specular
			}
		};
		float pointLights_float[4][3] =
		{   // constant linear quadratic
			{
				{1.0f}, {0.14f}, {0.07f}
			},
			{
				{1.0f}, {0.14f}, {0.07f}
			},
			{
				{1.0f}, {0.14f}, {0.07f}
			},
			{
				{1.0f}, {0.14f}, {0.07f}
			}
		};
		//spotLight
		float spotLight_ambient[3] = { 0.0f, 0.0f, 0.0f };
		float spotLight_diffuse[3] = { 1.0f, 1.0f, 1.0f };
		float spotLight_specular[3] = { 1.0f, 1.0f, 1.0f };
		float spotLight_constant = 1.0f;
		float spotLight_linear = 0.09f;
		float spotLight_quadratic = 0.032f;
		float spotLight_cutOff = 12.5f;
		float spotLight_outerCutOff = 15.0f;
		//check markers
		bool check_dirLight = true;
		bool check_SpotLight = false;
		bool check_pointLights[4] = { false, false, false, true };
		bool check_grid = true;

	private:
		void draw();

		std::unique_ptr<class Window> m_pWindow;

		EventDispatcher m_event_dispatcher;
		bool m_bCloseWindow = false;
		bool m_cursor_in_scene = false;
		std::string m_executable_path;

		glm::vec2 m_cursor_pos_in_scene;
	};
}