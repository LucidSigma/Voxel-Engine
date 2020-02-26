#pragma once
#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "../interfaces/INoncopyable.h"

#include <glad/glad.h>

class Drawable
	: private INoncopyable
{
protected:
	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLuint m_ebo = 0;

public:
	Drawable();

	Drawable(Drawable&& other) noexcept;
	Drawable& operator =(Drawable&& other) noexcept;

	virtual ~Drawable() noexcept;

private:
	void Destroy() noexcept;
	void DestroyVertexArray() noexcept;
	void DestroyVertexBuffer() noexcept;
	void DestroyElementBuffer() noexcept;

	GLuint ResetVertexArray(const GLuint newVertexArrayID) noexcept;
	GLuint ResetVertexBuffer(const GLuint newVertexBufferID) noexcept;
	GLuint ResetElementBuffer(const GLuint newElementBufferID) noexcept;
};

#endif