#pragma once

#define M_PI 3.14159265358979323846

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <array>
#include <cmath>

#include <exprtk/include/exprtk.hpp>

#include "OpenGL/VertexArray.hpp"
#include "OpenGL/VertexBuffer.hpp"
#include "OpenGL/IndexBuffer.hpp"
#include "OpenGL/ShaderProgram.hpp"
#include "OpenGL/Renderer_OpenGL.hpp"
#include "../NoEngineCore/includes/NoEngineCore/Camera.hpp"
#include "../NoEngineCore/includes/NoEngineCore/ResourceManager.hpp"
#include "../NoEngineCore/includes/NoEngineCore/Log.hpp"

#include <iostream>

class Actor
{
public:
    Actor(glm::mat4 view_matrix, glm::mat4 projection_matrix, glm::vec3 camera_position,
        std::shared_ptr<NoEngine::ShaderProgram> shader,
        std::shared_ptr<NoEngine::ShaderProgram> outline_shader,
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& rotation = glm::vec3(0.0f),
        const glm::vec3& scale = glm::vec3(1.0f),
        const std::string name = "")
        : m_view_matrix(view_matrix), m_projection_matrix(projection_matrix), m_camera_position(camera_position), p_shader(shader),
        p_outline_shader(outline_shader), m_position(position), m_rotation(rotation),
        m_scale(scale), m_name(name), m_default_position(position), m_default_rotation(rotation), m_default_scale(scale)
    {
    }
    virtual ~Actor() {}

    virtual void update(float deltaTime, std::array<std::array<std::string, 3>, 3> function) {}
    virtual void draw(std::string param = "default") {}
    virtual bool intersect(const glm::vec3& ray_origin, const glm::vec3& ray_direction, float& distance) const { return false; }

    void update_function(float deltaTime, std::array<std::array<std::string, 3>, 3> function)
    {
        if (function[0][0] != "" || function[0][1] != "" || function[0][2] != ""
            || function[1][0] != "" || function[1][1] != "" || function[1][2] != ""
            || function[2][0] != "" || function[2][1] != "" || function[2][2] != "")
        {
            float pos_x, pos_y, pos_z;
            if (function[0][0] == "")
                pos_x = get_default_position().x;
            else
                pos_x = parser_string(function[0][0], deltaTime);
            if (function[0][1] == "")
                pos_y = get_default_position().y;
            else
                pos_y = parser_string(function[0][1], deltaTime);
            if (function[0][2] == "")
                pos_z = get_default_position().z;
            else
                pos_z = parser_string(function[0][2], deltaTime);
            set_position(glm::vec3(pos_x, pos_y, pos_z));

            float rot_x, rot_y, rot_z;
            if (function[1][0] == "")
                rot_x = get_default_rotation().x;
            else
                rot_x = parser_string(function[1][0], deltaTime);
            if (function[1][1] == "")
                rot_y = get_default_rotation().y;
            else
                rot_y = parser_string(function[1][1], deltaTime);
            if (function[1][2] == "")
                rot_z = get_default_rotation().z;
            else
                rot_z = parser_string(function[1][2], deltaTime);
            set_rotation(glm::vec3(rot_x, rot_y, rot_z));

            float sc_x, sc_y, sc_z;
            if (function[2][0] == "")
                sc_x = get_default_scale().x;
            else
                sc_x = parser_string(function[2][0], deltaTime);
            if (function[2][1] == "")
                sc_y = get_default_scale().y;
            else
                sc_y = parser_string(function[2][1], deltaTime);
            if (function[2][2] == "")
                sc_z = get_default_scale().z;
            else
                sc_z = parser_string(function[2][2], deltaTime);

            set_scale(glm::vec3(sc_x, sc_y, sc_z));
        }
    }

    void set_position(const glm::vec3& position)
    {
        m_position = position;
    }
    const glm::vec3& get_position() const { return m_position; }

    void set_default_position(const glm::vec3& default_position) { m_default_position = default_position; }
    const glm::vec3& get_default_position() const { return m_default_position; }

    void set_rotation(const glm::vec3& rotation) { m_rotation = rotation; }
    const glm::vec3& get_rotation() const { return m_rotation; }

    void set_default_rotation(const glm::vec3& default_rotation) { m_default_rotation = default_rotation; }
    const glm::vec3& get_default_rotation() const { return m_default_rotation; }

    void set_scale(const glm::vec3& scale) { m_scale = scale; }
    const glm::vec3& get_scale() const { return m_scale; }

    void set_default_scale(const glm::vec3& default_scale) { m_default_scale = default_scale; }
    const glm::vec3& get_default_scale() const { return m_default_scale; }

    void set_name(const std::string& name) { m_name = name; }
    const std::string& get_name() const { return m_name; }

    std::shared_ptr<NoEngine::ShaderProgram> get_shader() { return p_shader; }
    void set_shader(std::shared_ptr<NoEngine::ShaderProgram> shader) { p_shader = shader; }

    void set_material_shader()
    {
        if (m_use_texture)
        {
            p_shader->bind();
            p_shader->set_int("material.diffuse", 0);
            p_shader->set_int("material.specular", 1);
        }
        else
        {
            p_shader->bind();
            p_shader->set_vec3("material.ambient_color", glm::vec3(0.7, 0.7, 0.7));
            p_shader->set_vec3("material.diffuse_color", glm::vec3(1.0f, 1.0f, 1.0f));
            p_shader->set_vec3("material.specular_color", glm::vec3(0.7, 0.7, 0.7));
            p_shader->set_float("material.shininess", 32.f);
        }
    }

    void set_new_function(const std::string new_function[3][3])
    {
        m_update_function[0][0] = new_function[0][0];
        m_update_function[0][1] = new_function[0][1];
        m_update_function[0][2] = new_function[0][2];

        m_update_function[1][0] = new_function[1][0];
        m_update_function[1][1] = new_function[1][1];
        m_update_function[1][2] = new_function[1][2];

        m_update_function[2][0] = new_function[2][0];
        m_update_function[2][1] = new_function[2][1];
        m_update_function[2][2] = new_function[2][2];
    }
    const std::array<std::array<std::string, 3>, 3> get_update_func() const { return m_update_function; }

    void set_selected(const bool selected) { m_selected = selected; }
    const bool get_selected() const { return m_selected; }

    void set_check_update_func(const bool check_update_func) { m_new_update_func = check_update_func; }
    const bool get_check_update_func() const { return m_new_update_func; }

    double parser_string(std::string function_string, float deltatime)
    {
        if (function_string == "") {
            return 0;
        }

        typedef exprtk::symbol_table<double> symbol_table_t;
        typedef exprtk::expression<double> expression_t;
        typedef exprtk::parser<double> parser_t;

        double POS_X = static_cast<double>(get_position().x);
        double POS_Y = static_cast<double>(get_position().y);
        double POS_Z = static_cast<double>(get_position().z);
        double ROT_X = static_cast<double>(get_rotation().x);
        double ROT_Y = static_cast<double>(get_rotation().y);
        double ROT_Z = static_cast<double>(get_rotation().z);
        double SC_X = static_cast<double>(get_scale().x);
        double SC_Y = static_cast<double>(get_scale().y);
        double SC_Z = static_cast<double>(get_scale().z);
        double TIME = static_cast<double>(deltatime);

        symbol_table_t symbol_table;
        symbol_table.add_variable("POS_X", POS_X);
        symbol_table.add_variable("POS_Y", POS_Y);
        symbol_table.add_variable("POS_Z", POS_Z);
        symbol_table.add_variable("ROT_X", ROT_X);
        symbol_table.add_variable("ROT_Y", ROT_Y);
        symbol_table.add_variable("ROT_Z", ROT_Z);
        symbol_table.add_variable("SC_X", SC_X);
        symbol_table.add_variable("SC_Y", SC_Y);
        symbol_table.add_variable("SC_Z", SC_Z);
        symbol_table.add_variable("TIME", TIME);

        expression_t expr;
        expr.register_symbol_table(symbol_table);

        parser_t parser;
        if (!parser.compile(function_string, expr)) {
            LOG_ERROR("Error compile Update-Function: {0}", function_string);
        }

        /*std::cout << expr.value() << "\n";*/

        return expr.value();
    }

    glm::mat4 update_model_matrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_position);
        model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, m_scale);
        return model;
    }

    glm::mat4 get_model_matrix() const {
        return update_model_matrix();
    }

protected:
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    std::string m_name;
    std::array<std::array<std::string, 3>, 3> m_update_function = { "", "", "", "", "", "", "", "", "" };
    bool m_selected = true;
    bool m_new_update_func = false;
    bool m_use_texture = false;

    glm::vec3 m_default_position;
    glm::vec3 m_default_rotation;
    glm::vec3 m_default_scale;

    std::shared_ptr<NoEngine::ShaderProgram> p_outline_shader;
    std::shared_ptr<NoEngine::ShaderProgram> p_shader;

    glm::mat4 m_view_matrix;
    glm::mat4 m_projection_matrix;
    glm::vec3 m_camera_position;
};