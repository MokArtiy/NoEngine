#pragma once

#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace NoEngine {

	class Camera
	{
	public:
		enum class ProjectionMode
		{
			Perspective,
			Orthographic
		};
		enum class CameraDirectaion
		{
			Forward,
			Backward,
			Left,
			Right,
			Up,
			Down
		};

		Camera(const glm::vec3& position = { 0, 0, 0 },
			   const glm::vec3& rotation = { 0, 0, 0 },
			   const ProjectionMode projection_mode = ProjectionMode::Perspective);

		void set_position(const glm::vec3& position);
		void set_rotation(const glm::vec3& rotation);
		void set_position_rotation(const glm::vec3& position, const glm::vec3& rotation);
		void set_projection_mode(const ProjectionMode projection_mode);
		void set_far_clip_plane(const float far);
		void set_near_clip_plane(const float near);
		void set_viewport_size(const float width, const float height);
		void set_field_of_view(const float fov);
		void set_movement_speed(const float movement_speed);

		const glm::mat4 get_view_matrix();
		const glm::mat4 get_projection_matrix() const { return m_projection_matrix; }
		const glm::mat4 get_vp_matrix() const { return m_projection_matrix * m_view_matrix; }
		const glm::vec3 get_direction() const { return m_direction; }
		const float get_far_clip_plane() const { return m_far_clip_plane; }
		const float get_near_clip_plane() const { return m_near_clip_plane; }
		const glm::vec2 get_viewport_size() const { return glm::vec2(m_viewport_width, m_viewport_height); }
		const float get_field_of_view() const { return m_field_of_view; }
		const float get_movement_speed() const { return m_movement_speed; }

		void move_forward(const float delta);
		void move_right(const float delta);
		void move_up(const float delta);
		void rotation(const glm::vec3& rotation_delta);

		const glm::vec3& get_position() const { return m_position; }
		const glm::vec3& get_rotation() const { return m_rotation; }

		// movement_delta.x - forward, movement_delta.x - right/left, movement_delta.z - up/down
		// rotation_delta.x - roll, rotation_delta.y - pitch, rotation_delta.z - yaw
		void add_movement_and_rotation(const glm::vec3& movement_delta, const glm::vec3& rotation_delta);
		void add_movement(const glm::vec3& movement_delta);

		void process_keyboard(CameraDirectaion direction, float delta_time);

	private:
		void update_view_matrix();
		void update_projection_matrix();

		glm::vec3 m_position;
		glm::vec3 m_rotation; // X - Roll, Y - Pitch, Z - Yaw
		ProjectionMode m_projection_mode;

		glm::vec3 m_direction;
		glm::vec3 m_right;
		glm::vec3 m_up;

		float m_far_clip_plane{ 100.f };
		float m_near_clip_plane{ 0.1f };
		float m_viewport_width{ 1920.f};
		float m_viewport_height{ 1080.f};
		float m_field_of_view{ 75.f };
		float m_movement_speed{ 5.f };

		static constexpr glm::vec3 s_world_up{ 0.f, 0.f, 1.f };
		static constexpr glm::vec3 s_world_right{ 0.f, -1.f, 0.f };
		static constexpr glm::vec3 s_world_forward{ 1.f, 0.f, 0.f };

		glm::mat4 m_view_matrix;
		glm::mat4 m_projection_matrix;
		bool m_update_view_matrix = false;
	};
}