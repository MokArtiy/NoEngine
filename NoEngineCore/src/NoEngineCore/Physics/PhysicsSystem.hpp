#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace NoEngine {
	
	class PhysicsSystem
	{
	public:
		static glm::vec3 screen_to_ray_origin(const glm::vec2& mouse_pos, glm::mat4 view_projection_matrix);
		static glm::vec3 get_ray_direction(const glm::vec2& mouse_pos, glm::mat4 view_projection_matrix);
		static void set_window_size(glm::vec2 window_size);

	private:
		static glm::vec2 m_window_size;
	};
}