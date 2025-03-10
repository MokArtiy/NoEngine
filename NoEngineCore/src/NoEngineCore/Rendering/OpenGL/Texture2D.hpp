#pragma once

namespace NoEngine {

	class Texture2D {
	public:
		Texture2D(const unsigned char* data, const unsigned int width, const unsigned int height, const unsigned int channels);
		~Texture2D();

		Texture2D(const Texture2D&) = delete;
		Texture2D& operator=(const Texture2D&) = delete;
		Texture2D& operator=(Texture2D&& texture) noexcept;
		Texture2D(Texture2D&& texture) noexcept;

		void bind(const unsigned int unit) const;
		unsigned int get_id() const { return m_id; }

	private:
		unsigned int m_id = 0;
		unsigned int m_mode = 0;
		unsigned int m_storage_mode = 0;
		unsigned int m_width = 0;
		unsigned int m_height = 0;
	};
}