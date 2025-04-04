#pragma once

#include <glm/mat4x4.hpp>

namespace NoEngine {

	enum class TypeShader
	{
		Object,
		PointLight,
		Grid,
		Outline
	};

	class ShaderProgram
	{
	public:
		ShaderProgram(const char* vertex_shader_src, const char* fragment_shader_src, TypeShader type = TypeShader::Object);
		ShaderProgram(ShaderProgram&&) noexcept;
		ShaderProgram& operator=(ShaderProgram&&) noexcept;
		~ShaderProgram();

		ShaderProgram() = delete;
		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;

		void bind() const;
		static void unbind();
		bool isCompiled() const { return m_isCompiled; }

		void set_float(const char* name, const float value) const;
		void set_matrix4(const char* name, const glm::mat4& matrix) const;
		void set_matrix3(const char* name, const glm::mat3& matrix) const;
		void set_int(const char* name, const int value) const;
		void set_vec3(const char* name, const glm::vec3& value) const;
		void set_vec3(const char* name, const float x, const float y, const float z) const;
		void set_vec3(const char* name, const float value[3]) const;
		void set_bool(const char* name, const bool value) const;

		TypeShader get_type() const { return m_type; }

	private:
		bool m_isCompiled = false;
		unsigned int m_id = 0;
		TypeShader m_type;
	};
}