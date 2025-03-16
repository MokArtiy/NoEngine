#pragma once

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

namespace NoEngine {

	class VertexArray {
	public:
		VertexArray();
		~VertexArray();

		VertexArray(const VertexArray&) = delete;
		VertexArray& operator=(const VertexArray&) = delete;
		VertexArray& operator=(VertexArray&& vertex_array) noexcept;
		VertexArray(VertexArray&& vertex_array) noexcept;

		void add_vertex_buffer(const VertexBuffer& vertex_buffer);
		void set_index_buffer(const IndexBuffer& index_buffer);
		void bind() const;
		static void unbind();
		size_t get_indices_count() const { return m_indices_count; }
		unsigned int get_id() const { return m_id; }

	private:
		unsigned int m_id = 0;
		unsigned int m_elements_count = 0;
		size_t m_indices_count = 0;
	};
}