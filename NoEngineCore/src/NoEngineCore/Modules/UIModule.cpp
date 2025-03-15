#include "UIModule.hpp"
#include "../NoEngineCore/includes/NoEngineCore/ResourceManager.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

#include "iostream"

namespace NoEngine {
	void UIModule::on_window_create(GLFWwindow* pWindow)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_ViewportsEnable;

		//ImFont* font = io.Fonts->AddFontFromFileTTF((NoEngine::ResourceManager::get_path() + "res/fonts/Nexa/Nexa Bold.otf").c_str(), 16.f);

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguisb.ttf", 20.0f);
		if (!font) {
			font = io.Fonts->AddFontDefault();
		}
		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 16.0f;
		static const ImWchar icon_ranges[] = { 0xf000, 0xf8ff, 0 };
		ImFont* fontAwesome = io.Fonts->AddFontFromFileTTF((ResourceManager::get_path() + "res/fonts/fontawesome/otfs/fa-solid-900.otf").c_str(), 16.f, &config, icon_ranges);
		if (fontAwesome == nullptr) {
			std::cerr << "Ошибка: FontAwesome не загружен!" << std::endl;
		}

		io.FontDefault = font;
		io.Fonts->Build();

		style.Colors[ImGuiCol_TitleBgActive] = style.Colors[ImGuiCol_TitleBg];
		style.Colors[ImGuiCol_Header] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05, 0.05, 0.05, 1.0f);

		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.27f, 0.27f, 0.27f, 1.0f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.0f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

		style.FrameRounding = 3.f;

		ImGui_ImplOpenGL3_Init();
		ImGui_ImplGlfw_InitForOpenGL(pWindow, true);
	}

	void UIModule::on_window_close()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void UIModule::on_ui_draw_begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void UIModule::on_ui_draw_end()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

}