#include "Text.h"

#include <array>
#include <utility>

#include "../Vertex.h"

#include <iostream>

Text::Text(const Font& font, const ShaderProgram& shader)
	: m_font(&font), m_shader(&shader)
{
	InitialiseVertexObjects();
}

Text::Text(Text&& other) noexcept
	: Drawable(std::move(other)), m_font(other.m_font), m_shader(other.m_shader)
{
	other.m_font = nullptr;
	other.m_shader = nullptr;
}

Text& Text::operator=(Text&& other) noexcept
{
	Drawable::operator =(std::move(other));

	m_font = std::exchange(other.m_font, nullptr);
	m_shader = std::exchange(other.m_shader, nullptr);

	return *this;
}

void Text::Render(const std::string& text, glm::vec2 position, const float scale, const glm::vec3& colour)
{
	m_shader->Use();
	m_shader->SetUniform<glm::vec3>("u_TextColour", colour);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m_vao);

	for (const char character : text)
	{
		const Font::Glyph& currentGlyph = m_font->GetGlyphs().at(character);

		constexpr char TallestCharacter = 'H';
		const float xPosition = position.x + currentGlyph.bearing.x * scale;
		const float yPosition = position.y + (m_font->GetGlyphs().at(TallestCharacter).bearing.y - currentGlyph.bearing.y) * scale;

		const float width = currentGlyph.size.x * scale;
		const float height = currentGlyph.size.y * scale;

		const std::array<Vertex2D, 4u> vertices{
			Vertex2D{ { xPosition, yPosition + height }, { 0.0f, 1.0f } },
			Vertex2D{ { xPosition + width, yPosition }, { 1.0f, 0.0f } },
			Vertex2D{ { xPosition, yPosition }, { 0.0f, 0.0f } },
			Vertex2D{ { xPosition + width, yPosition + height }, { 1.0f, 1.0f } }
		};

		glBindTexture(GL_TEXTURE_2D, currentGlyph.textureID);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

		position.x += (currentGlyph.advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Text::InitialiseVertexObjects() const
{
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex2D), nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices::QuadIndices.size() * sizeof(unsigned short), vertices::QuadIndices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(Position);
	glVertexAttribPointer(Position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<const GLvoid*>(offsetof(Vertex2D, position)));

	glEnableVertexAttribArray(TextureCoordinates);
	glVertexAttribPointer(TextureCoordinates, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<const GLvoid*>(offsetof(Vertex2D, textureCoordinates)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}