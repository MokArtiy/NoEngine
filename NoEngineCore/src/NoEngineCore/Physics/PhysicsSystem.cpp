#include "PhysicsSystem.hpp"

namespace NoEngine {

	glm::vec2 PhysicsSystem::m_window_size;

	/*glm::vec3 PhysicsSystem::screen_to_ray_origin(const glm::vec2& mouse_pos, glm::mat4 view_projection_matrix)
	{
		float x = (2.0f * mouse_pos.x) / m_window_size.x - 1.0f;
		float y = 1.0f - (2.0f * mouse_pos.y) / m_window_size.y;
		glm::vec3 ray_nds = glm::vec3(x, y, 1.0f);

		glm::mat4 view_projection_inverse = glm::inverse(view_projection_matrix);
		glm::vec4 world_point = view_projection_inverse * ray_nds;

		return glm::vec3(world_point) / world_point.w;
	}*/

	glm::vec3 PhysicsSystem::get_ray_direction(const glm::vec2& mouse_pos, glm::mat4 view_matrix, glm::mat4 projection_matrix)
	{
		float x = (2.0f * mouse_pos.x) / m_window_size.x - 1.0f;
		float y = 1.0f - (2.0f * mouse_pos.y) / m_window_size.y;
		glm::vec3 ray_nds = glm::vec3(x, y, 1.0f);

		glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
		glm::vec4 ray_eye = glm::inverse(projection_matrix) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
		glm::vec3 ray_wor = glm::vec3(glm::inverse(view_matrix) * ray_eye);
		ray_wor = glm::normalize(ray_wor);

		return ray_wor;
	}

	void PhysicsSystem::set_window_size(glm::vec2 window_size)
	{
		m_window_size = window_size;
	}

}