#pragma once

#include <NoEngineCore/Application.hpp>
#include <iostream>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

class NoEngineEditor : public NoEngine::Application
{
	double m_initial_mouse_pos_x = 0.0;
	double m_initial_mouse_pos_y = 0.0;
	float camera_position[3] = { 0.f, 0.f, 1.f };
	float camera_rotation[3] = { 0.f, 0.f, 0.f };
	float camera_fov = 60.f;
	float camera_near_plane = 0.1f;
	float camera_far_plane = 100.f;
	//float delta_time = 0.0f;
	double last_frame = 0.0f;
	float camera_movement_speed = 5.f;
	bool perspective_camera = true;
	bool key_pressed = false;
	bool key_was_pressed = false;
	//transform
	float location[3] = { 0.0f, 0.0f, 0.0f };
	float rotation[3] = { 0.0f, 0.0f, 0.0f };
	float scale[3] = { 1.0f, 1.0f, 1.0f };
	ImVec4 colors[3] = {
		ImVec4(1.0f, 0.270f, 0.270f, 1.0f), // red
		ImVec4(0.413f, 1.0f, 0.288f, 1.0f), // green
		ImVec4(0.373f, 0.446f, 1.0f, 1.0f), // blue
	};
	bool locked = true;
	std::string shapes[3] = {"plane", "cube", "sphere"};
	const int shape_count = IM_ARRAYSIZE(shapes);
	const char* lights[3] = { "Directional", "Point ", "Spotlight" };
	const int light_count = IM_ARRAYSIZE(shapes);
	std::shared_ptr<NoEngine::Texture2D> texture_plane;
	std::shared_ptr<NoEngine::Texture2D> texture_cube;
	std::shared_ptr<NoEngine::Texture2D> texture_sphere;
	std::shared_ptr<NoEngine::Texture2D> texture_add;

	//update function
	bool run = false;
	bool pause = false;
	bool stop = true;
	bool check_selected = check_selected_obj();
	const char* edit_parametrs[3] = { "Position", "Rotation", "Scale" };
	char buffer_position_x[1024] = { "" };
	char buffer_position_y[1024] = { "" };
	char buffer_position_z[1024] = { "" };
	char buffer_rotation_x[1024] = { "" };
	char buffer_rotation_y[1024] = { "" };
	char buffer_rotation_z[1024] = { "" };
	char buffer_scale_x[1024] = { "" };
	char buffer_scale_y[1024] = { "" };
	char buffer_scale_z[1024] = { "" };
	int item_selected_idx = 0;
	bool text_copied = false;

	virtual void on_update() override;
	virtual void on_mouse_button_event(const NoEngine::MouseButton button_code, const double x_pos, const double y_pos, const bool pressed) override
	{
		m_initial_mouse_pos_x = x_pos;
		m_initial_mouse_pos_y = y_pos;

		key_pressed = pressed;
	}
	virtual void on_ui_draw() override;
	virtual void on_ui_draw_in_scene() override;
	virtual void on_editor_init() override;

	void setup_dockspace_menu();
	void setup_main_control_menu();
	void setup_object_menu();

	void draw_text_with_color(ImVec2 pos, const char* text, ImU32 text_color, ImU32 frame_color, ImVec2 padding);
};