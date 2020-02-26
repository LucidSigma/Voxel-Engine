#include "Sprite.h"

#include <utility>

#include "Vertex.h"

Sprite::Sprite(const ShaderProgram& shader, const Texture& texture)
	: m_shader(&shader), m_texture(&texture)
{
	InitialiseVertexObjects();
}

Sprite::Sprite(Sprite&& other) noexcept
	: Drawable(std::move(other)), m_shader(other.m_shader), m_texture(other.m_texture)
{
	other.m_shader = nullptr;
	other.m_texture = nullptr;
}

Sprite& Sprite::operator=(Sprite&& other) noexcept
{
	Drawable::operator =(std::move(other));

	m_shader = std::exchange(other.m_shader, nullptr);
	m_texture = std::exchange(other.m_texture, nullptr);

	return *this;
}

void Sprite::Render(const glm::vec2& position, const glm::vec2& size, const float rotation, const glm::vec3& colour) const
{
	m_shader->Use();

	const glm::mat4 model = CreateModelMatrix(position, size, rotation);
	m_shader->SetUniform<glm::mat4>("u_Model", model);
	m_shader->SetUniform<glm::vec3>("u_Colour", colour);

	glActiveTexture(GL_TEXTURE0);
	m_texture->Bind();

	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vertices::QuadIndices.size()), GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
}

void Sprite::InitialiseVertexObjects() const
{
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices::QuadVertices.size() * sizeof(Vertex2D), vertices::QuadVertices.data(), GL_STATIC_DRAW);

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

glm::mat4 Sprite::CreateModelMatrix(const glm::vec2& position, const glm::vec2& size, const float rotation) const
{
	glm::mat4 model{ 1.0f };
	model = glm::translate(model, glm::vec3{ position, 0.0f });

	model = glm::translate(model, glm::vec3{ 0.5f * size.x, 0.5f * size.y, 0.0f });
	model = glm::rotate(model, rotation, glm::vec3{ 0.0f, 0.0f, 1.0f });
	model = glm::translate(model, glm::vec3{ -0.5f * size.x, -0.5f * size.y, 0.0f });

	model = glm::scale(model, glm::vec3{ size, 1.0f });

	return model;
}