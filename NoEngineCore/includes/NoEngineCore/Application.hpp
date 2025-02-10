#pragma once

#include "NoEngineCore/Event.hpp"
#include "NoEngineCore/Camera.hpp"
#include "NoEngineCore/ResourceManager.hpp"

#include <memory>
#include <string>

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

		virtual void on_mouse_button_event(const MouseButton button_code, const double x_pos, const double y_pos, const bool pressed) {}

		glm::vec2 get_current_coursor_position() const;

		Camera camera{ glm::vec3(-5, 0, 0) };

		int frameCount = 0;
		float light_source_position[3] = { 0.f, 0.f, 1.f };
		float m_background_color[4] = { 0.11f, 0.12f, 0.132f, 0.0f };
		float light_source_color[3] = { 1.f, 1.f, 1.f };
		float ambient_factor = 0.2f;
		float diffuse_factor = 0.5f;
		float specular_factor = 1.0f;
		float shininess = 64.f;
		double current_frame = 0.0f;
		bool check_shader = false;

		//dirLight
		float dirLight_direction[3] = { -0.3f, -0.2f, -1.0f };
		float dirLight_ambient[3] = { 0.392f, 0.392f, 0.392f };
		float dirLight_diffuse[3] = { 1.0f, 1.0f, 1.0f };
		float dirLight_specular[3] = { 1.0f, 1.0f, 1.0f };
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
				{1.0f}, {0.09f}, {0.32f}
			},
			{
				{1.0f}, {0.09f}, {0.32f}
			},
			{
				{1.0f}, {0.09f}, {0.32f}
			},
			{
				{1.0f}, {0.09f}, {0.32f}
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
		bool check_pointLights[4] = { true, true, true, true };

	private:
		void draw();

		std::unique_ptr<class Window> m_pWindow;

		EventDispatcher m_event_dispatcher;
		bool m_bCloseWindow = false;
		std::string m_executable_path;

	};
}