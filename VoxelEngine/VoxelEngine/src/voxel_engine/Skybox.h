#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H

#include "../engine/graphics/Drawable.h"

#include "../engine/graphics/Cubemap.h"
#include "../engine/graphics/shaders/ShaderProgram.h"

class Skybox
	: public Drawable
{
private:
	const ShaderProgram* m_shader = nullptr;
	const Cubemap* m_cubemap = nullptr;

public:
	Skybox(const ShaderProgram& shader, const Cubemap& cubemap);
	
	Skybox(Skybox&& other) noexcept;
	Skybox& operator =(Skybox && other) noexcept;

	~Skybox() noexcept = default;

	void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const;

private:
	void InitialiseVertexObjects() const;
};

#endif