#include "PhysicsSystem.hpp"

namespace NoEngine {

	glm::vec2 PhysicsSystem::m_window_size;

	glm::vec3 PhysicsSystem::screen_to_ray_origin(const glm::vec2& mouse_pos, glm::mat4 view_projection_matrix)
	{
		float normal_x = (2.0f * mouse_pos.x) / m_window_size.x - 1.0f;
		float normal_y = 1.0f - (2.0f * mouse_pos.y) / m_window_size.y;

		glm::vec4 near_point(normal_x, normal_y, -1.0f, 1.0f);

		glm::mat4 view_projection_inverse = glm::inverse(view_projection_matrix);
		glm::vec4 world_point = view_projection_inverse * near_point;

		return glm::vec3(world_point) / world_point.w;
	}

	glm::vec3 PhysicsSystem::get_ray_direction(const glm::vec2& mouse_pos, glm::mat4 view_projection_matrix)
	{
		glm::vec3 near_point = screen_to_ray_origin(mouse_pos, view_projection_matrix);
		glm::vec3 far_point = screen_to_ray_origin(glm::vec2(m_window_size.x, m_window_size.y), view_projection_matrix);

		return glm::normalize(far_point - near_point);
	}

	void PhysicsSystem::set_window_size(glm::vec2 window_size)
	{
		m_window_size = window_size;
	}

}