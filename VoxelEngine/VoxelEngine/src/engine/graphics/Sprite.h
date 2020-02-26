#pragma once
#ifndef SPRITE_H
#define SPRITE_H

#include "Drawable.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <array>

#include "shaders/ShaderProgram.h"
#include "Texture.h"

class Sprite
	: public Drawable
{
private:
	const ShaderProgram* m_shader = nullptr;
	const Texture* m_texture = nullptr;

public:
	Sprite(const ShaderProgram& shader, const Texture& texture);
	
	Sprite(Sprite&& other) noexcept;
	Sprite& operator =(Sprite&& other) noexcept;

	virtual ~Sprite() noexcept override = default;

	void Render(const glm::vec2& position, const glm::vec2& size, const float rotation = 0.0f, const glm::vec3& colour = glm::vec3{ 1.0f }) const;

private:
	void InitialiseVertexObjects() const;

	glm::mat4 CreateModelMatrix(const glm::vec2& position, const glm::vec2& size, const float rotation) const;
};

#endif