#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

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
    Actor(std::shared_ptr<NoEngine::ShaderProgram> outline_shader,
        const glm::vec3& position = glm::vec3(0.0f), 
          const glm::vec3& rotation = glm::vec3(0.0f),
          const glm::vec3& scale    = glm::vec3(1.0f),
          const std::string name = "")
        : p_outline_shader(outline_shader), m_position(position), m_rotation(rotation), m_scale(scale), m_name(name) { }
    virtual ~Actor() {}

    virtual void update(float deltaTime) {}
    virtual void draw(std::string param = "default") {}
    virtual bool intersect(const glm::vec3& ray_origin, const glm::vec3& ray_direction, float& distance) const { return false; }

    void set_position(const glm::vec3& position) { m_position = position; }
    const glm::vec3& get_position() const { return m_position; }

    void set_rotation(const glm::vec3& rotation) { m_rotation = rotation; }
    const glm::vec3& get_rotation() const { return m_rotation; }

    void set_scale(const glm::vec3& scale) { m_scale = scale; }
    const glm::vec3& get_scale() const { return m_scale; }

    void set_name(const std::string& name) { m_name = name; }
    const std::string& get_name() const { return m_name; }

    void set_selected(const bool selected) { m_selected = selected; }
    const bool get_selected() const { return m_selected; }

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
    bool m_selected = true;

    std::shared_ptr<NoEngine::ShaderProgram> p_outline_shader;
    //GLuint m_id;
};