#pragma once
#ifndef TEXT_H
#define TEXT_H

#include "../Drawable.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

#include "../shaders/ShaderProgram.h"
#include "Font.h"

class Text
	: public Drawable
{
private:
	const Font* m_font = nullptr;
	const ShaderProgram* m_shader = nullptr;

public:
	Text(const Font& font, const ShaderProgram& shader);

	Text(Text&& other) noexcept;
	Text& operator =(Text&& other) noexcept;

	virtual ~Text() noexcept override = default;

	void Render(const std::string& text, glm::vec2 position, const float scale, const glm::vec3& colour = glm::vec3{ 1.0f });

private:
	void InitialiseVertexObjects() const;
};

#endif