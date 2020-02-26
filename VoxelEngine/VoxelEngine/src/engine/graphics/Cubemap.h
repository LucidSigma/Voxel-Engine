#pragma once
#ifndef CUBEMAP_H
#define CUBEMAP_H

#include "../interfaces/INoncopyable.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <string>

class Cubemap
{
private:
	inline static const std::unordered_map<int, std::pair<GLint, GLenum>> s_componentMap{
		{ 1, { GL_R8, GL_RED } },
		{ 2, { GL_RG8, GL_RG } },
		{ 3, { GL_RGB8, GL_RGB } },
		{ 4, { GL_RGBA8, GL_RGBA } }
	};

	GLuint m_id = 0;
	glm::uvec2 m_size{ 0, 0 };

public:
	Cubemap(const std::string& filepath);

	Cubemap(Cubemap&& other) noexcept;
	Cubemap& operator =(Cubemap&& other) noexcept;

	~Cubemap() noexcept;

	void Bind() const;

	inline GLuint GetID() const noexcept { return m_id; }

private:
	void Generate(const std::string& filepath);
	void SetupParameters(const GLint internalFormat, const GLenum format);

	void Destroy() noexcept;
	GLuint ResetCubemap(const GLuint newCubemapID) noexcept;
};

#endif