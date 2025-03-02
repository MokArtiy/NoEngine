#include "../includes/Editor.hpp"
#include "NoEngineCore/Input.hpp"

#include <iostream>

void NoEngineEditor::on_update()
{
	float delta_time = static_cast<float>(current_frame - last_frame);
	last_frame = current_frame;

	glm::vec3 rotation_delta{ 0, 0, 0 };
	if (NoEngine::Input::IsKeyPressed(NoEngine::KeyCode::KEY_W))
	{
		camera.process_keyboard(NoEngine::Camera::CameraDirectaion::Forward, delta_time);
	}
	if (NoEngine::Input::IsKeyPressed(NoEngine::KeyCode::KEY_S))
	{
		camera.process_keyboard(NoEngine::Camera::CameraDirectaion::Backward, delta_time);
	}
	if (NoEngine::Input::IsKeyPressed(NoEngine::KeyCode::KEY_A))
	{
		camera.process_keyboard(NoEngine::Camera::CameraDirectaion::Left, delta_time);
	}
	if (NoEngine::Input::IsKeyPressed(NoEngine::KeyCode::KEY_D))
	{
		camera.process_keyboard(NoEngine::Camera::CameraDirectaion::Right, delta_time);
	}
	if (NoEngine::Input::IsKeyPressed(NoEngine::KeyCode::KEY_SPACE))
	{
		camera.process_keyboard(NoEngine::Camera::CameraDirectaion::Up, delta_time);
	}
	if (NoEngine::Input::IsKeyPressed(NoEngine::KeyCode::KEY_LEFT_SHIFT))
	{
		camera.process_keyboard(NoEngine::Camera::CameraDirectaion::Down, delta_time);
	}

	if (NoEngine::Input::IsMouseButtonPressed(NoEngine::MouseButton::MOUSE_BUTTON_RIGHT) && check_cursor_in_scene())
	{
		glm::vec2 current_cursor_position = get_current_cursor_position();
		if (NoEngine::Input::IsMouseButtonPressed(NoEngine::MouseButton::MOUSE_BUTTON_LEFT))
		{
			camera.move_right(static_cast<float>(current_cursor_position.x - m_initial_mouse_pos_x) / 100.f);
			camera.move_up(static_cast<float>(m_initial_mouse_pos_y - current_cursor_position.y) / 100.f);
		}
		else
		{
			rotation_delta.z += static_cast<float>(m_initial_mouse_pos_x - current_cursor_position.x) / 7.f;
			rotation_delta.y -= static_cast<float>(m_initial_mouse_pos_y - current_cursor_position.y) / 7.f;
		}
		m_initial_mouse_pos_x = current_cursor_position.x;
		m_initial_mouse_pos_y = current_cursor_position.y;
		camera.rotation(rotation_delta);
	}
	if (NoEngine::Input::IsMouseButtonPressed(NoEngine::MouseButton::MOUSE_BUTTON_LEFT) && check_cursor_in_scene())
	{
		if (key_pressed && !key_was_pressed)
		{
			glm::vec2 mouse_pos = get_current_cursor_position_in_scene();
			pick_object(mouse_pos);
		}
	}
	key_was_pressed = key_pressed;
}

void NoEngineEditor::setup_dockspace_menu()
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoWindowMenuButton;
	static ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	window_flags |= ImGuiWindowFlags_NoBackground;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", nullptr, window_flags);
	ImGui::PopStyleVar(3);

	ImGuiIO& io = ImGui::GetIO();
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene...", NULL))
			{

			}
			if (ImGui::MenuItem("Open Scene...", NULL))
			{

			}
			if (ImGui::MenuItem("Save Scene...", NULL))
			{

			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", NULL))
			{
				close();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	ImGui::End();
}

void NoEngineEditor::setup_main_control_menu()
{
	ImGui::Begin("Scene Editor");

	//Directional light
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Directional Light"))
	{
		std::string text;
		check_dirLight ? text = "Active##dir" : text = "Non Active##dir";
		ImGui::Checkbox(text.c_str(), &check_dirLight);

		ImGui::DragFloat3("Direction##dir", dirLight_direction, 0.001f, -1.0f, 1.0f);
		ImGui::Text("Ambient:");
		ImGui::ColorEdit3("Ambient##dir", dirLight_ambient, ImGuiColorEditFlags_Float);
		ImGui::Text("Diffuse:");
		ImGui::ColorEdit3("Diffuse##dir", dirLight_diffuse, ImGuiColorEditFlags_Float);
		ImGui::Text("Specular:");
		ImGui::ColorEdit3("Specular##dir", dirLight_specular, ImGuiColorEditFlags_Float);
	}
	//Point lights
	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Point Lights"))
	{
		std::string text;
		for (int i = 0; i < sizeof(check_pointLights); ++i)
		{
			std::string number = std::to_string(i);
			check_pointLights[i] ? text = ("Active##point" + number).c_str() : text = ("Non Active##point" + number).c_str();
			ImGui::Checkbox(text.c_str(), &check_pointLights[i]);
			if (!check_pointLights[i])
				ImGui::BeginDisabled(true);
			ImGui::DragFloat3(("Position##point" + number).c_str(), pointLights_vec3[i][0], 0.01f, -10.f, 10.f);
			ImGui::DragFloat(("Constant##point" + number).c_str(), &pointLights_float[i][0], 0.001f, -1.0f, 1.0f);
			ImGui::DragFloat(("Linear##point" + number).c_str(), &pointLights_float[i][1], 0.001f, -1.0f, 1.0f);
			ImGui::DragFloat(("Quadratic##point" + number).c_str(), &pointLights_float[i][2], 0.001f, 0.0f, 1.0f);

			ImGui::Text("Color:");
			if (ImGui::ColorEdit3(("Color##point" + number).c_str(), pointLights_colors[i], ImGuiColorEditFlags_Float))
			{
				pointLights_vec3[i][1][0] = pointLights_colors[i][0] * 0.1f;
				pointLights_vec3[i][1][1] = pointLights_colors[i][1] * 0.1f;
				pointLights_vec3[i][1][2] = pointLights_colors[i][2] * 0.1f;

				pointLights_vec3[i][2][0] = pointLights_colors[i][0];
				pointLights_vec3[i][2][1] = pointLights_colors[i][1];
				pointLights_vec3[i][2][2] = pointLights_colors[i][2];

				pointLights_vec3[i][3][0] = pointLights_colors[i][0];
				pointLights_vec3[i][3][1] = pointLights_colors[i][1];
				pointLights_vec3[i][3][2] = pointLights_colors[i][2];
			}
			ImGui::Text("Ambient:");
			ImGui::ColorEdit3(("Ambient##point" + number).c_str(), pointLights_vec3[i][1], ImGuiColorEditFlags_Float);
			ImGui::Text("Diffuse:");
			ImGui::ColorEdit3(("Diffuse##point" + number).c_str(), pointLights_vec3[i][2], ImGuiColorEditFlags_Float);
			ImGui::Text("Specular:");
			ImGui::ColorEdit3(("Specular##point" + number).c_str(), pointLights_vec3[i][3], ImGuiColorEditFlags_Float);
			if (!check_pointLights[i])
				ImGui::EndDisabled();
		}
	}
	//Spot light
	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Spot Light"))
	{
		std::string text;
		check_SpotLight ? text = "Active##spot" : text = "Non Active##spot";
		ImGui::Checkbox("Active##spot", &check_SpotLight);
		ImGui::InputFloat("Constant##spot", &spotLight_constant);
		ImGui::InputFloat("Linear##spot", &spotLight_linear);
		ImGui::InputFloat("Quadratic##spot", &spotLight_quadratic);
		ImGui::InputFloat("CutOff##spot", &spotLight_cutOff);
		ImGui::InputFloat("OuterCutOff##spot", &spotLight_outerCutOff);

		ImGui::Text("Ambient:");
		ImGui::ColorEdit3("Ambient##spot", spotLight_ambient, ImGuiColorEditFlags_Float);
		ImGui::Text("Diffuse:");
		ImGui::ColorEdit3("Diffuse##spot", spotLight_diffuse, ImGuiColorEditFlags_Float);
		ImGui::Text("Specular:");
		ImGui::ColorEdit3("Specular##spot", spotLight_specular, ImGuiColorEditFlags_Float);
	}
	//ImGui::SetCursorPosX((ImGui::GetWindowWidth()) / 2);
	if (ImGui::Button("+ New object", ImVec2(ImGui::GetWindowWidth() - 17, 20)))
	{
		add_editor_object();
	}
	if (ImGui::Button("Remove object", ImVec2(ImGui::GetWindowWidth() - 17, 20)))
	{
		remove_editor_object("Cube_0");
	}

	ImGui::End();

	ImGui::Begin("Info");
	//statistics
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SeparatorText("INFO");
	if (ImGui::IsMousePosValid()) {
		ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
	}
	else
		ImGui::Text("<INVALID>");
	ImGui::Text("%.1f FPS", io.Framerate);
	ImGui::Text("Player Coordinate: (X: %f, Y: %f, Z: %f)", camera.get_position().x, camera.get_position().y, camera.get_position().z);
	ImGui::End();

	ImGui::Begin("Main Editor");
	ImGui::ColorEdit3("Background color", m_background_color, ImGuiColorEditFlags_Float);
	//camera control
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Camera Control"))
	{
		camera_position[0] = camera.get_position().x;
		camera_position[1] = camera.get_position().y;
		camera_position[2] = camera.get_position().z;
		camera_rotation[0] = camera.get_rotation().x;
		camera_rotation[1] = camera.get_rotation().y;
		camera_rotation[2] = camera.get_rotation().z;

		camera_fov = camera.get_field_of_view();
		camera_near_plane = camera.get_near_clip_plane();
		camera_far_plane = camera.get_far_clip_plane();
		camera_movement_speed = camera.get_movement_speed();

		if (ImGui::SliderFloat("Movement speed", &camera_movement_speed, 0.1f, 100.f))
		{
			camera.set_movement_speed(camera_movement_speed);
		}
		if (ImGui::SliderFloat3("Position", camera_position, -10.f, 10.f))
		{
			camera.set_position(glm::vec3(camera_position[0], camera_position[1], camera_position[2]));
		}
		if (ImGui::SliderFloat3("Rotation", camera_rotation, 0.f, 360.f))
		{
			camera.set_rotation(glm::vec3(camera_rotation[0], camera_rotation[1], camera_rotation[2]));
		}
		if (ImGui::SliderFloat("FOV", &camera_fov, 1.f, 120.f))
		{
			camera.set_field_of_view(camera_fov);
		}
		if (ImGui::SliderFloat("Near clip plane", &camera_near_plane, 0.1f, 10.f))
		{
			camera.set_near_clip_plane(camera_near_plane);
		}
		if (ImGui::SliderFloat("Far clip plane", &camera_far_plane, 1.f, 100.f))
		{
			camera.set_far_clip_plane(camera_far_plane);
		}
		if (ImGui::Checkbox("Perspective camera", &perspective_camera))
		{
			camera.set_projection_mode(perspective_camera ? NoEngine::Camera::ProjectionMode::Perspective : NoEngine::Camera::ProjectionMode::Orthographic);
		}
	}

	ImGui::End();
}

void NoEngineEditor::on_ui_draw()
{
	{
		setup_dockspace_menu();
		setup_main_control_menu();
	}
}
