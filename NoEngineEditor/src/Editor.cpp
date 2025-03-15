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
		ImGui::Checkbox("Draw grid", &check_grid);
	}

	ImGui::End();
}

void NoEngineEditor::setup_object_menu()
{
	location[0] = get_selected_object_location().x;
	location[1] = get_selected_object_location().y;
	location[2] = get_selected_object_location().z;
	rotation[0] = get_selected_object_rotation().x;
	rotation[1] = get_selected_object_rotation().y;
	rotation[2] = get_selected_object_rotation().z;
	scale[0] = get_selected_object_scale().x;
	scale[1] = get_selected_object_scale().y;
	scale[2] = get_selected_object_scale().z;

	ImGui::Begin("Scene Details");

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::Text("Position   ");
		ImGui::SameLine();
		ImGui::PushItemWidth(115.0f);
		for (int i = 0; i < 3; i++)
		{
			ImGui::PushID(i + "location");
			ImGui::PushStyleColor(ImGuiCol_FrameBg, colors[i]);
			if (ImGui::DragFloat("", &location[i], 0.01f))
			{
				set_selected_object_location(location[0], location[1], location[2]);
			}
			ImGui::PopStyleColor();
			ImGui::PopID();

			if (i < 2) {
				ImGui::SameLine();
			}
		}
		ImGui::PopItemWidth();

		ImGui::Text("Rotation  ");
		ImGui::SameLine();
		ImGui::PushItemWidth(115.0f);
		for (int i = 0; i < 3; i++)
		{
			ImGui::PushID(i + "rotation");
			ImGui::PushStyleColor(ImGuiCol_FrameBg, colors[i]);
			if (ImGui::DragFloat("", &rotation[i], 0.1f))
			{
				set_selected_object_rotation(rotation[0], rotation[1], rotation[2]);
			}
			ImGui::PopStyleColor();
			ImGui::PopID();

			if (i < 2) {
				ImGui::SameLine();
			}
		}
		ImGui::PopItemWidth();

		ImGui::Text("Scale");
		ImGui::SameLine();
		if (ImGui::Button(u8"\uf023", ImVec2(24, 24)))
		{
			locked = !locked;
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(115.0f);
		for (int i = 0; i < 3; i++)
		{
			ImGui::PushID(i + "scale");
			ImGui::PushStyleColor(ImGuiCol_FrameBg, colors[i]);
			if (ImGui::DragFloat("", &scale[i], 0.01f, 0.001f, 100000.f))
			{
				if (locked)
				{
					for (int j = 0; j < 3; j++)
					{
						scale[j] = scale[i];
					}
				}
				set_selected_object_scale(scale[0], scale[1], scale[2]);
			}
			ImGui::PopStyleColor();
			ImGui::PopID();

			if (i < 2) {
				ImGui::SameLine();
			}
		}
		ImGui::PopItemWidth();

		ImGui::Spacing();

		if (ImGui::Button(u8"\uf044 EDIT UPDATE FUNCTION", ImVec2(ImGui::GetWindowWidth() / 2 - 12, 50.f)))
		{
			ImGui::OpenPopup("Edit update function");
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"\uf1f8 REMOVE OBJECT", ImVec2(ImGui::GetWindowWidth() / 2 - 12, 50.f)))
		{
			remove_editor_object();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Edit update function", NULL))
		{
			ImGui::BeginChild("ScrollArea", ImVec2(0, 470), false, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::SeparatorText("CHOOSE EDIT PARAMETR");
			if (ImGui::BeginCombo("##", edit_parametrs[item_selected_idx]))
			{
				for (int i = 0; i < IM_ARRAYSIZE(edit_parametrs); i++)
				{
					const bool is_selected = (item_selected_idx == 1);
					if (ImGui::Selectable(edit_parametrs[i], is_selected))
					{
						item_selected_idx = i;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::Spacing();

			switch (item_selected_idx)
			{
			case 0:	//position
				ImGui::Text("text0");
				ImGui::SameLine();
				if (ImGui::Button("copy text")) {
					ImGui::SetClipboardText("copy");
				}
				ImGui::InputTextMultiline("##source", buffer_position, 1024, ImVec2(675, ImGui::GetTextLineHeight() * 5), ImGuiInputTextFlags_AllowTabInput);
				break;
			case 1:	//rotation
				ImGui::Text("text1");
				ImGui::InputTextMultiline("##source", buffer_rotation, 1024, ImVec2(675, ImGui::GetTextLineHeight() * 5), ImGuiInputTextFlags_AllowTabInput);
				break;
			case 2:	//scale
				ImGui::Text("text2");
				ImGui::InputTextMultiline("##source", buffer_scale, 1024, ImVec2(675, ImGui::GetTextLineHeight() * 5), ImGuiInputTextFlags_AllowTabInput);
				break;
			}

			ImGui::Spacing();

			ImGui::EndChild();

			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();
			if (ImGui::Button("Close", ImVec2(50.f, 30.f)))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	}

	ImGui::End();
}

void NoEngineEditor::draw_text_with_color(ImVec2 pos, const char* text, ImU32 text_color, ImU32 frame_color, ImVec2 padding) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 text_size = ImGui::CalcTextSize(text);
	ImVec2 frame_min = ImVec2(pos.x - padding.x, pos.y - padding.y);
	ImVec2 frame_max = ImVec2(pos.x + text_size.x + padding.x, pos.y + text_size.y + padding.y);
	draw_list->AddRect(frame_min, frame_max, frame_color);
}

void NoEngineEditor::on_ui_draw()
{
	{
		setup_dockspace_menu();
		setup_object_menu();
		setup_main_control_menu();
	}
}

void NoEngineEditor::on_ui_draw_in_scene()
{
	ImGui::SetCursorPos(ImVec2(button_scene_pos.x - 52, button_scene_pos.y + 45));

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.27f, 0.27f, 0.27f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.38, 0.38, 0.38, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);

	if (ImGui::ImageButton(std::to_string(texture_add->get_id()).c_str(), (intptr_t)texture_add->get_id(), ImVec2(45, 45)))
	{
		ImGui::OpenPopup("Select scene object");
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Select scene object", NULL))
	{
		ImGui::BeginChild("ScrollArea", ImVec2(0, 200), false, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::SeparatorText("Shape");

		if (ImGui::ImageButton(std::to_string(texture_plane->get_id()).c_str(), (intptr_t)texture_plane->get_id(), ImVec2(100.f, 100.f)))
		{
			add_editor_object(NE_PLANE);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::ImageButton(std::to_string(texture_cube->get_id()).c_str(), (intptr_t)texture_cube->get_id(), ImVec2(100.f, 100.f)))
		{
			add_editor_object(NE_CUBE);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::ImageButton(std::to_string(texture_sphere->get_id()).c_str(), (intptr_t)texture_sphere->get_id(), ImVec2(100.f, 100.f)))
		{
			add_editor_object(NE_SPHERE);
			ImGui::CloseCurrentPopup();
		}

		ImGui::Spacing();
		ImGui::SeparatorText("Light");
		for (int i = 0; i < light_count; i++)
		{
			if (ImGui::Button(lights[i], ImVec2(100.f, 100.f)))
			{
				ImGui::CloseCurrentPopup();
			}

			if (i < 5) {
				ImGui::SameLine();
			}
		}
		ImGui::EndChild();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
		if (ImGui::Button("Close", ImVec2(50.f, 30.f)))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();
}

void NoEngineEditor::on_editor_init()
{
	texture_add = NoEngine::ResourceManager::load_texture("add_obj_texture", "res/textures/ui/add obj.png", false);
	texture_plane = NoEngine::ResourceManager::load_texture("texture_plane", "res/textures/ui/plane.png", false);
	texture_cube = NoEngine::ResourceManager::load_texture("texture_cube", "res/textures/ui/cube.png", false);
	texture_sphere = NoEngine::ResourceManager::load_texture("texture_sphere", "res/textures/ui/sphere.png", false);
}
