#include "NoEngineCore/Window.hpp"
#include "NoEngineCore/Log.hpp"
#include "NoEngineCore/Rendering/OpenGL/ShaderProgram.hpp"
#include "NoEngineCore/Rendering/OpenGL/VertexBuffer.hpp"
#include "NoEngineCore/Rendering/OpenGL/VertexArray.hpp"
#include "NoEngineCore/Rendering/OpenGL/IndexBuffer.hpp"
#include "NoEngineCore/Camera.hpp"
#include "NoEngineCore/Modules/UIModule.hpp"

#include "NoEngineCore/Rendering/OpenGL/Renderer_OpenGL.hpp"

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>

namespace NoEngine {

    GLfloat positions_colors2[] = {
        -0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f
    };

    GLuint indeces[] = {
        0, 1, 2, 3, 2, 1
    };

    const char* vertex_shader =
        "#version 460\n"
        "layout(location = 0) in vec3 vertex_position;"
        "layout(location = 1) in vec3 vertex_color;"
        "uniform mat4 model_matrix;"
        "uniform mat4 view_projection_matrix;"
        "out vec3 color;"
        "void main() {"
        "   color = vertex_color;"
        "   gl_Position = view_projection_matrix * model_matrix * vec4(vertex_position, 1.0);"
        "}";

    const char* fragment_shader =
        "#version 460\n"
        "in vec3 color;"
        "out vec4 frag_color;"
        "void main() {"
        "   frag_color = vec4(color, 1.0);"
        "}";

    std::unique_ptr<ShaderProgram> p_shader_program;
    std::unique_ptr<VertexBuffer> p_positions_colors_vbo;
    std::unique_ptr<IndexBuffer> p_index_buffer;
    std::unique_ptr<VertexArray> p_vao;
    float scale[3] = { 1.f, 1.f, 1.f };
    float rotate = 0.f;
    float translate[3] = {0.f, 0.f, 0.f};

    float camera_position[3] = { 0.f, 0.f, 1.f };
    float camera_rotation[3] = { 0.f, 0.f, 0.f };
    bool perspective_camera = false;
    Camera camera;

	Window::Window(std::string title, const unsigned int width, const unsigned int height)
        : m_data({ std::move(title), width, height })
	{
		int resultCode = init();
	}

	Window::~Window()
	{
		shutdown();
	}

	int Window::init()
	{
        LOG_INFO("Creating window '{0}' width size {1}x{2}", m_data.title, m_data.width, m_data.height);

        glfwSetErrorCallback([](int error_code, const char* description)
            {
                LOG_CRITICAL("GLFW error: {0}", description);
            });

        if (!glfwInit())
        {
            LOG_CRITICAL("Can't initialize GLFW!");
            return -1;
        }

        m_pWindow = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);
        if (!m_pWindow)
        {
            LOG_CRITICAL("Can't create window '{0}' width size {1}x{2}", m_data.title, m_data.width, m_data.height);
            return -2;
        }

        if (!Renderer_OpenGL::init(m_pWindow))
        {
            LOG_CRITICAL("Failed to initialize OpenGL renderer");
            return -3;
        };

        glfwSetWindowUserPointer(m_pWindow, &m_data);

        glfwSetWindowSizeCallback(m_pWindow,
            [](GLFWwindow* pWindow, int width, int height)
            {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                data.width = width;
                data.height = height;
                EventWindowResize event(width, height);
                data.eventCallbackFn(event);
            }
        );

        glfwSetCursorPosCallback(m_pWindow,
            [](GLFWwindow* pWindow, double x, double y)
            {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                EventMouseMoved event(x, y);
                data.eventCallbackFn(event);
            }
        );

        glfwSetWindowCloseCallback(m_pWindow,
            [](GLFWwindow* pWindow)
            {
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(pWindow));
                EventWindowClose event;
                data.eventCallbackFn(event);
            }
        );

        glfwSetFramebufferSizeCallback(m_pWindow,
            [](GLFWwindow* pWindow, int width, int height)
            {
                Renderer_OpenGL::set_viewport(width, height);
            }
        );
        UIModule::on_window_create(m_pWindow);

        p_shader_program = std::make_unique<ShaderProgram>(vertex_shader, fragment_shader);
        if (!p_shader_program->isCompiled())
        {
            return false;
        }
        
        BufferLayout buffer_layout_1vec3
        {
            ShaderDataType::Float3
        };

        BufferLayout buffer_layout_2vec3
        {
            ShaderDataType::Float3,
            ShaderDataType::Float3
        };

        p_vao = std::make_unique<VertexArray>();
        p_positions_colors_vbo = std::make_unique<VertexBuffer>(positions_colors2, sizeof(positions_colors2), buffer_layout_2vec3);
        p_index_buffer = std::make_unique<IndexBuffer>(indeces, sizeof(indeces) / sizeof(GLuint));

        p_vao->add_vertex_buffer(*p_positions_colors_vbo);
        p_vao->set_index_buffer(*p_index_buffer);

        return 0;
	}

	void Window::shutdown()
	{
        UIModule::on_window_close();
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
	}

    void Window::on_update()
    {
        Renderer_OpenGL::set_clear_color(m_background_color[0], m_background_color[1], m_background_color[2], m_background_color[3]);
        Renderer_OpenGL::clear();

        p_shader_program->bind();

        glm::mat4 scale_matrix(scale[0], 0, 0, 0,
                               0, scale[1], 0, 0,
                               0, 0, scale[2], 0,
                               0, 0, 0, 1);

        float rotate_in_radians = glm::radians(rotate);
        glm::mat4 rotate_matrix(cos(rotate_in_radians), sin(rotate_in_radians), 0, 0,
                                -sin(rotate_in_radians), cos(rotate_in_radians), 0, 0,
                                0, 0, 1, 0,
                                0, 0, 0, 1);

        glm::mat4 translate_matrix( 1, 0, 0, 0,
                                    0, 1, 0, 0,
                                    0, 0, 1, 0,
                                    translate[0], translate[1], translate[2], 1);

        glm::mat4 model_matrix = translate_matrix * rotate_matrix * scale_matrix;
        p_shader_program->setMatrix4("model_matrix", model_matrix);

        camera.set_position_rotation(glm::vec3(camera_position[0], camera_position[1], camera_position[2]), 
                                     glm::vec3(camera_rotation[0], camera_rotation[1], camera_rotation[2]));
        camera.set_projection_mode(perspective_camera ? Camera::ProjectionMode::Perspective : Camera::ProjectionMode::Orthographic);
        p_shader_program->setMatrix4("view_projection_matrix", camera.get_projection_matrix() * camera.get_view_matrix());

        Renderer_OpenGL::draw(*p_vao);

        UIModule::on_ui_draw_begin();
        bool show = true;
        UIModule::ShowExampleAppDockSpace(&show);
        ImGui::ShowDemoWindow();
        ImGui::Begin("Background Color Window");
        ImGui::ColorEdit4("Background Color", m_background_color);
        ImGui::SliderFloat3("Scale", scale, 0.f, 2.f);
        ImGui::SliderFloat("Rotate", &rotate, 0.f, 360.f);
        ImGui::SliderFloat3("Translate", translate, -1.f, 1.f);

        ImGui::SliderFloat3("Camera position", camera_position, -10.f, 10.f);
        ImGui::SliderFloat3("Camera rotation", camera_rotation, 0.f, 360.f);
        ImGui::Checkbox("Perspective camera", &perspective_camera);
        ImGui::End();
        UIModule::on_ui_draw_end();

        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();

    }
}