#include "Skybox.h"

#include <utility>

#include "../engine/graphics/Vertex.h"

Skybox::Skybox(const ShaderProgram& shader, const Cubemap& cubemap)
	: m_shader(&shader), m_cubemap(&cubemap)
{
	InitialiseVertexObjects();
}

Skybox::Skybox(Skybox&& other) noexcept
	: Drawable(std::move(other)), m_shader(other.m_shader), m_cubemap(other.m_cubemap)
{
	other.m_shader = nullptr;
	other.m_cubemap = nullptr;
}

Skybox& Skybox::operator=(Skybox&& other) noexcept
{
	Drawable::operator =(std::move(other));

	m_shader = std::exchange(other.m_shader, nullptr);
	m_cubemap = std::exchange(other.m_cubemap, nullptr);

	return *this;
}

void Skybox::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const
{
	m_shader->Use();
	m_shader->SetUniform("u_ViewProjection", projectionMatrix * viewMatrix);

	glActiveTexture(GL_TEXTURE0);
	m_cubemap->Bind();

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vertices::CubeIndices.size()), GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(0);
}

void Skybox::InitialiseVertexObjects() const
{
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices::CubeVertices.size() * sizeof(Vertex2D), vertices::CubeVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices::InvertedCubeIndices.size() * sizeof(unsigned short), vertices::InvertedCubeIndices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(Position);
	glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<const GLvoid*>(0));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}