#include "../includes/Editor.hpp"
#include "NoEngineCore/Input.hpp"

#include <iostream>
#include <cstring>

void NoEngineEditor::on_update()
{
	float delta_time = static_cast<float>(current_frame - last_frame);
	last_frame = current_frame;
	glm::vec3 rotation_delta{ 0, 0, 0 };

	ImGuiIO& io = ImGui::GetIO();

	if (!io.WantCaptureKeyboard)
	{
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
	}
	else {

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

	ImGui::ColorEdit3("Background color", m_background_color, ImGuiColorEditFlags_Float);

	ImGui::Spacing();
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
	}

	ImGui::Spacing();

	//Directional light
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Directional Light"))
	{

		std::string text;
		check_dirLight ? text = "Active##dir" : text = "Non Active##dir";
		ImGui::Checkbox(text.c_str(), &check_dirLight);

		ImGui::Text("Direction ");
		ImGui::SameLine();
		ImGui::PushItemWidth(115.0f);
		for (int i = 0; i < 3; i++)
		{
			ImGui::PushID(i + "direction_dir");
			ImGui::DragFloat("", &dirLight_direction[i], 0.001f, -1.0f, 1.0f);
			ImGui::PopID();

			if (i < 2) {
				ImGui::SameLine();
			}
		}
		ImGui::PopItemWidth();
		
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::PushItemWidth(361.0f);

		ImGui::Text("Ambient  ");
		ImGui::SameLine();
		ImGui::ColorEdit3("##Ambient_dir", dirLight_ambient, ImGuiColorEditFlags_Float);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 2.0f);
		ImGui::Text("Diffuse    ");
		ImGui::SameLine();
		ImGui::ColorEdit3("##Diffuse_dir", dirLight_diffuse, ImGuiColorEditFlags_Float);

		ImGui::Text("Specular  ");
		ImGui::SameLine();
		ImGui::ColorEdit3("##Specular_dir", dirLight_specular, ImGuiColorEditFlags_Float);

		ImGui::PopItemWidth();
	}

	ImGui::Spacing();

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

	constant = get_selected_object_constant();
	linear = get_selected_object_linear();
	quadratic = get_selected_object_quadratic();
	color[0] = get_selected_object_color().x;
	color[1] = get_selected_object_color().y;
	color[2] = get_selected_object_color().z;
	ambiend[0] = get_selected_object_ambient().x;
	ambiend[1] = get_selected_object_ambient().y;
	ambiend[2] = get_selected_object_ambient().z;
	diffuse[0] = get_selected_object_diffuse().x;
	diffuse[1] = get_selected_object_diffuse().y;
	diffuse[2] = get_selected_object_diffuse().z;
	specular[0] = get_selected_object_specular().x;
	specular[1] = get_selected_object_specular().y;
	specular[2] = get_selected_object_specular().z;
	shininess = get_selected_object_shininess();

	ImGui::Begin("Scene Details");

	check_selected = check_selected_obj();
	if (!check_selected) {
		ImGui::BeginDisabled(true);
	}

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

		if (get_type_selected_obj() == NE_POINT_LIGHT) {
			ImGui::BeginDisabled(true);
		}

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
		if (ImGui::Button(locked ? u8"\uf023" : u8"\uf09c", ImVec2(24, 24)))
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

		if (get_type_selected_obj() == NE_POINT_LIGHT) {
			ImGui::EndDisabled();
		}

	//ImGui::Spacing();

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::CollapsingHeader("Material"))
	{
		if (get_type_selected_obj() == NE_POINT_LIGHT)
		{
			ImGui::Text("Color       ");
			ImGui::SameLine();
			ImGui::PushItemWidth(361.0f);
			if (ImGui::ColorEdit3("##color", color, ImGuiColorEditFlags_Float))
			{
				set_selected_object_color(color[0], color[1], color[2]);
			}

			ImGui::Text("Constant ");
			ImGui::SameLine();
			if (ImGui::DragFloat("##constant", &constant, 0.001, -1.0f, 1.0f))
			{
				set_selected_object_constant(constant);
			}

			ImGui::Text("Linear      ");
			ImGui::SameLine();
			if (ImGui::DragFloat("##linear", &linear, 0.001, -1.0f, 1.0f))
			{
				set_selected_object_linear(linear);
			}

			ImGui::Text("Quadratic");
			ImGui::SameLine();
			if (ImGui::DragFloat("##quadratic", &quadratic, 0.001, -1.0f, 1.0f))
			{
				set_selected_object_quadratic(quadratic);
			}

			ImGui::PopItemWidth();
		}

		ImGui::Text("Ambinent");
		ImGui::SameLine();
		ImGui::PushItemWidth(115.0f);
		for (int i = 0; i < 3; i++)
		{
			ImGui::PushID(i + "ambinent");
			if (ImGui::DragFloat("", &ambiend[i], 0.01f))
			{
				set_selected_object_ambient(ambiend[0], ambiend[1], ambiend[2]);
			}
			ImGui::PopID();

			if (i < 2) {
				ImGui::SameLine();
			}
		}
		ImGui::PopItemWidth();

		ImGui::Text("Diffuse     ");
		ImGui::SameLine();
		ImGui::PushItemWidth(115.0f);
		for (int i = 0; i < 3; i++)
		{
			ImGui::PushID(i + "diffuse");
			if (ImGui::DragFloat("", &diffuse[i], 0.01f))
			{
				set_selected_object_diffuse(diffuse[0], diffuse[1], diffuse[2]);
			}
			ImGui::PopID();

			if (i < 2) {
				ImGui::SameLine();
			}
		}
		ImGui::PopItemWidth();

		ImGui::Text("Specular  ");
		ImGui::SameLine();
		ImGui::PushItemWidth(115.0f);
		for (int i = 0; i < 3; i++)
		{
			ImGui::PushID(i + "specular");
			if(i == 0)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 2.0f);
			if (ImGui::DragFloat("", &specular[i], 0.01f))
			{
				set_selected_object_specular(specular[0], specular[1], specular[2]);
			}
			ImGui::PopID();

			if (i < 2) {
				ImGui::SameLine();
			}
		}
		ImGui::PopItemWidth();

		if (get_type_selected_obj() != NE_POINT_LIGHT && get_type_selected_obj() != -1)
		{
			ImGui::PushItemWidth(360.0f);
			ImGui::Text("Shininess ");
			ImGui::SameLine();
			if (ImGui::DragFloat("##shininess", &shininess, 0.1, 0.1, 128.0f))
			{
				set_selected_object_shininess(shininess);
			}
		}
	}

	ImGui::Spacing();

	if (ImGui::Button(u8"\uf044 EDIT UPDATE FUNCTION", ImVec2(ImGui::GetWindowWidth() / 2 - 12, 50.f)))
	{
		std::array<std::array<std::string, 3>, 3> function_obj = get_function_in_selected_obj();
		strncpy(buffer_position_x, function_obj[0][0].c_str(), sizeof(buffer_position_x) - 1);
		buffer_position_x[sizeof(buffer_position_x) - 1] = '\0';
		strncpy(buffer_position_y, function_obj[0][1].c_str(), sizeof(buffer_position_y) - 1);
		buffer_position_y[sizeof(buffer_position_y) - 1] = '\0';
		strncpy(buffer_position_z, function_obj[0][2].c_str(), sizeof(buffer_position_z) - 1);
		buffer_position_z[sizeof(buffer_position_z) - 1] = '\0';
		strncpy(buffer_rotation_x, function_obj[1][0].c_str(), sizeof(buffer_rotation_x) - 1);
		buffer_rotation_x[sizeof(buffer_rotation_x) - 1] = '\0';
		strncpy(buffer_rotation_y, function_obj[1][1].c_str(), sizeof(buffer_rotation_y) - 1);
		buffer_rotation_y[sizeof(buffer_rotation_y) - 1] = '\0';
		strncpy(buffer_rotation_z, function_obj[1][2].c_str(), sizeof(buffer_rotation_z) - 1);
		buffer_rotation_z[sizeof(buffer_rotation_z) - 1] = '\0';
		strncpy(buffer_scale_x, function_obj[2][0].c_str(), sizeof(buffer_scale_x) - 1);
		buffer_scale_x[sizeof(buffer_scale_x) - 1] = '\0';
		strncpy(buffer_scale_y, function_obj[2][1].c_str(), sizeof(buffer_scale_y) - 1);
		buffer_scale_y[sizeof(buffer_scale_y) - 1] = '\0';
		strncpy(buffer_scale_z, function_obj[2][2].c_str(), sizeof(buffer_scale_z) - 1);
		buffer_scale_z[sizeof(buffer_scale_z) - 1] = '\0';

		ImGui::OpenPopup("Edit update function");
	}

	ImGui::SameLine();

	if (ImGui::Button(u8"\uf1f8 REMOVE OBJECT", ImVec2(ImGui::GetWindowWidth() / 2 - 12, 50.f)))
	{
		remove_editor_object();
		check_selected = true;
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Edit update function", NULL))
	{
		ImGui::BeginChild("ScrollArea", ImVec2(0, 400), false, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::SeparatorText("CHOOSE EDIT PARAMETR");

		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ImGui::GetContentRegionAvail().x);
		ImGui::Text(
			u8"Это меню определения функции обновления объектов на сцене. Ниже представлены поля, куда можно "
			u8"задавать свою логику изменения значений переменных в главном цикле сцены."
		);
		ImGui::Text(
			u8"Для создания функции используются переменные ниже и основные математические методы: +, -, *, /, ^, "
			u8"sqrt, cos, sin."
		);
		ImGui::PopTextWrapPos();

		ImGui::Spacing();

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

		ImGui::Text(u8"TIME - текущее время в сцене (основа любого передвижения в движке)");
		ImGui::SameLine();
		if (ImGui::Button("Copy##t")) {
			ImGui::SetClipboardText("TIME");
		}
		ImGui::Spacing();

		float available_width = ImGui::GetContentRegionAvail().x;

		switch (item_selected_idx)
		{
		case 0:	//position
			ImGui::Text(u8"POS_X - позиция по x");
			ImGui::SameLine();
			if (ImGui::Button("Copy##x")) {
				ImGui::SetClipboardText("POS_X");
			}
			ImGui::SetNextItemWidth(available_width);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
			ImGui::InputText("##pos_x", buffer_position_x, 1024);
			ImGui::PopStyleColor();

			ImGui::Spacing();

			ImGui::Text(u8"POS_Y - позиция по y");
			ImGui::SameLine();
			if (ImGui::Button("Copy##y")) {
				ImGui::SetClipboardText("POS_Y");
			}
			ImGui::SetNextItemWidth(available_width);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
			ImGui::InputText("##pos_y", buffer_position_y, 1024);
			ImGui::PopStyleColor();

			ImGui::Spacing();

			ImGui::Text(u8"POS_Z - позиция по z");
			ImGui::SameLine();
			if (ImGui::Button("Copy##z")) {
				ImGui::SetClipboardText("POS_Z");
			}
			ImGui::SetNextItemWidth(available_width);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
			ImGui::InputText("##pos_z", buffer_position_z, 1024);
			ImGui::PopStyleColor();
			break;
		case 1:	//rotation
			ImGui::Text(u8"ROT_X - поворот по x");
			ImGui::SameLine();
			if (ImGui::Button("Copy##x")) {
				ImGui::SetClipboardText("ROT_X");
			}
			ImGui::SetNextItemWidth(available_width);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
			ImGui::InputText("##rot_x", buffer_rotation_x, 1024);
			ImGui::PopStyleColor();

			ImGui::Spacing();

			ImGui::Text(u8"ROT_Y - поворот по y");
			ImGui::SameLine();
			if (ImGui::Button("Copy##y")) {
				ImGui::SetClipboardText("POS_X");
			}
			ImGui::SetNextItemWidth(available_width);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
			ImGui::InputText("##rot_y", buffer_rotation_y, 1024);
			ImGui::PopStyleColor();

			ImGui::Spacing();

			ImGui::Text(u8"ROT_Z - поворот по z");
			ImGui::SameLine();
			if (ImGui::Button("Copy##z")) {
				ImGui::SetClipboardText("ROT_Z");
			}
			ImGui::SetNextItemWidth(available_width);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
			ImGui::InputText("##rot_z", buffer_rotation_z, 1024);
			ImGui::PopStyleColor();
			break;
		case 2:	//scale
			ImGui::Text(u8"SC_X - скалирование по x");
			ImGui::SameLine();
			if (ImGui::Button("Copy##x")) {
				ImGui::SetClipboardText("SC_X");
			}
			ImGui::SetNextItemWidth(available_width);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
			ImGui::InputText("##sc_x", buffer_scale_x, 1024);
			ImGui::PopStyleColor();

			ImGui::Spacing();

			ImGui::Text(u8"SC_Y - скалирование по y");
			ImGui::SameLine();
			if (ImGui::Button("Copy##y")) {
				ImGui::SetClipboardText("SC_Y");
			}
			ImGui::SetNextItemWidth(available_width);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
			ImGui::InputText("##sc_y", buffer_scale_y, 1024);
			ImGui::PopStyleColor();

			ImGui::Spacing();

			ImGui::Text(u8"SC_Z - скалирование по z");
			ImGui::SameLine();
			if (ImGui::Button("Copy##z")) {
				ImGui::SetClipboardText("SC_Z");
			}
			ImGui::SetNextItemWidth(available_width);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
			ImGui::InputText("##sc_z", buffer_scale_z, 1024);
			ImGui::PopStyleColor();
		}

		ImGui::Spacing();

		ImGui::EndChild();

		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
		if (ImGui::Button("Close", ImVec2(50.f, 30.f)))
			ImGui::CloseCurrentPopup();
		ImGui::SameLine();
		if (ImGui::Button("Save", ImVec2(50.f, 30.f)))
		{
			function_editor_str[0][0] = buffer_position_x;
			function_editor_str[0][1] = buffer_position_y;
			function_editor_str[0][2] = buffer_position_z;

			function_editor_str[1][0] = buffer_rotation_x;
			function_editor_str[1][1] = buffer_rotation_y;
			function_editor_str[1][2] = buffer_rotation_z;

			function_editor_str[2][0] = buffer_scale_x;
			function_editor_str[2][1] = buffer_scale_y;
			function_editor_str[2][2] = buffer_scale_z;

			save_new_func();

			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset", ImVec2(50.f, 30.f)))
		{
			strncpy(buffer_position_x, "", sizeof(buffer_position_x) - 1);
			buffer_position_x[sizeof(buffer_position_x) - 1] = '\0';
			strncpy(buffer_position_y, "", sizeof(buffer_position_y) - 1);
			buffer_position_y[sizeof(buffer_position_y) - 1] = '\0';
			strncpy(buffer_position_z, "", sizeof(buffer_position_z) - 1);
			buffer_position_z[sizeof(buffer_position_z) - 1] = '\0';
			strncpy(buffer_rotation_x, "", sizeof(buffer_rotation_x) - 1);
			buffer_rotation_x[sizeof(buffer_rotation_x) - 1] = '\0';
			strncpy(buffer_rotation_y, "", sizeof(buffer_rotation_y) - 1);
			buffer_rotation_y[sizeof(buffer_rotation_y) - 1] = '\0';
			strncpy(buffer_rotation_z, "", sizeof(buffer_rotation_z) - 1);
			buffer_rotation_z[sizeof(buffer_rotation_z) - 1] = '\0';
			strncpy(buffer_scale_x, "", sizeof(buffer_scale_x) - 1);
			buffer_scale_x[sizeof(buffer_scale_x) - 1] = '\0';
			strncpy(buffer_scale_y, "", sizeof(buffer_scale_y) - 1);
			buffer_scale_y[sizeof(buffer_scale_y) - 1] = '\0';
			strncpy(buffer_scale_z, "", sizeof(buffer_scale_z) - 1);
			buffer_scale_z[sizeof(buffer_scale_z) - 1] = '\0';

			function_editor_str[0][0] = buffer_position_x;
			function_editor_str[0][1] = buffer_position_y;
			function_editor_str[0][2] = buffer_position_z;

			function_editor_str[1][0] = buffer_rotation_x;
			function_editor_str[1][1] = buffer_rotation_y;
			function_editor_str[1][2] = buffer_rotation_z;

			function_editor_str[2][0] = buffer_scale_x;
			function_editor_str[2][1] = buffer_scale_y;
			function_editor_str[2][2] = buffer_scale_z;

			reset_new_func();
		}
		ImGui::EndPopup();
	}
	}

	if (!check_selected) {
		ImGui::EndDisabled();
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

	ImGui::SetCursorPos(ImVec2(button_scene_pos.x - 52, button_scene_pos.y + 45 + 45 + 14));
	if (ImGui::ImageButton(std::to_string(texture_show_grid->get_id()).c_str(), (intptr_t)texture_show_grid->get_id(), ImVec2(45, 45)))
	{
		check_grid = !check_grid;
		set_check_lights(check_grid);
	}

	ImGui::SetCursorPos(ImVec2(button_scene_pos.x - 52, button_scene_pos.y + 45 + 45 + 45 + 14 + 14));
	if (ImGui::ImageButton(std::to_string(texture_camera_perspective->get_id()).c_str(), (intptr_t)texture_camera_perspective->get_id(), ImVec2(45, 45)))
	{
		perspective_camera = !perspective_camera;
		camera.set_projection_mode(perspective_camera ? NoEngine::Camera::ProjectionMode::Perspective : NoEngine::Camera::ProjectionMode::Orthographic);
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

		if (ImGui::ImageButton(std::to_string(texture_pointlight->get_id()).c_str(), (intptr_t)texture_pointlight->get_id(), ImVec2(100.f, 100.f)))
		{
			add_editor_object(NE_POINT_LIGHT);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndChild();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
		if (ImGui::Button("Close", ImVec2(50.f, 30.f)))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}

	//run-pause
	ImGui::SetCursorPos(ImVec2((button_scene_pos.x - 38 * 2) * 0.5, button_scene_pos.y + 45));
	std::string icon;
	if (!run && !pause)
		icon = u8"\uf04b";
	else if (!run && pause)
		icon = u8"\uf04b";
	else if (run && !pause)
		icon = u8"\uf04c";

	if (ImGui::Button(icon.c_str(), ImVec2(50, 50)))
	{
		if (!run && !pause) {
			current_state_scene = NE_RUN;
			run = true;
			stop = false;
		}
		else if (run && !pause) {
			current_state_scene = NE_PAUSE;
			run = false;
			pause = true;
			stop = false;
		}
		else if (!run && pause) {
			current_state_scene = NE_RUN;
			run = true;
			pause = false;
			stop = false;
		}
	}
	//stop
	ImGui::SetCursorPos(ImVec2((button_scene_pos.x + 38) * 0.5, button_scene_pos.y + 45));
	if (ImGui::Button(u8"\uf04d", ImVec2(50, 50)))
	{
		if (!stop)
		{
			current_state_scene = NE_STOP;
			stop = true;
			run = false;
			pause = false;
		}
	}

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();
}

void NoEngineEditor::on_editor_init()
{
	texture_add = NoEngine::ResourceManager::load_texture("add_obj_texture", "res/textures/ui/add obj.png", false);
	texture_show_grid = NoEngine::ResourceManager::load_texture("texture_show_grid", "res/textures/ui/show grid.png", false);
	texture_camera_perspective = NoEngine::ResourceManager::load_texture("camera_perspective_texture", "res/textures/ui/perspective_wh.png", false);
	texture_msaa_check = NoEngine::ResourceManager::load_texture("texture_msaa_check", "res/textures/ui/video card wh.png", false);
	texture_plane = NoEngine::ResourceManager::load_texture("texture_plane", "res/textures/ui/plane.png", false);
	texture_cube = NoEngine::ResourceManager::load_texture("texture_cube", "res/textures/ui/cube.png", false);
	texture_sphere = NoEngine::ResourceManager::load_texture("texture_sphere", "res/textures/ui/sphere.png", false);
	texture_pointlight = NoEngine::ResourceManager::load_texture("texture_pointlight", "res/textures/ui/light bulb wh.png", false);
}
