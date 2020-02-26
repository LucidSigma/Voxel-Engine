#include "Drawable.h"

#include <utility>

Drawable::Drawable()
{
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);
}

Drawable::Drawable(Drawable&& other) noexcept
	: m_vao(ResetVertexArray(other.m_vao)), m_vbo(ResetVertexBuffer(other.m_vbo)), m_ebo(ResetElementBuffer(other.m_ebo))
{
	other.m_vao = 0;
	other.m_vbo = 0;
	other.m_ebo = 0;
}

Drawable& Drawable::operator=(Drawable&& other) noexcept
{
	Destroy();

	m_vao = std::exchange(other.m_vao, 0);
	m_vbo = std::exchange(other.m_vbo, 0);
	m_ebo = std::exchange(other.m_ebo, 0);

	return *this;
}

Drawable::~Drawable() noexcept
{
	DestroyVertexArray();
	DestroyVertexBuffer();
	DestroyElementBuffer();
}

void Drawable::Destroy() noexcept
{
	DestroyVertexArray();
	DestroyVertexBuffer();
	DestroyElementBuffer();
}

void Drawable::DestroyVertexArray() noexcept
{
	if (m_vao != 0)
	{
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}
}

void Drawable::DestroyVertexBuffer() noexcept
{
	if (m_vbo != 0)
	{
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}
}

void Drawable::DestroyElementBuffer() noexcept
{
	if (m_ebo != 0)
	{
		glDeleteBuffers(1, &m_ebo);
		m_ebo = 0;
	}
}

GLuint Drawable::ResetVertexArray(const GLuint newVertexArrayID) noexcept
{
	DestroyVertexArray();

	return newVertexArrayID;
}

GLuint Drawable::ResetVertexBuffer(const GLuint newVertexBufferID) noexcept
{
	DestroyVertexBuffer();

	return newVertexBufferID;
}

GLuint Drawable::ResetElementBuffer(const GLuint newElementBufferID) noexcept
{
	DestroyElementBuffer();

	return newElementBufferID;
}