#pragma once
#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "AssetManager.h"

#include <initializer_list>

#include "../../graphics/shaders/ShaderProgram.h"

class ShaderManager
	: public AssetManager<ShaderProgram>
{
public:
	ShaderManager() = default;
	virtual ~ShaderManager() noexcept override = default;

	void AddShader(const std::string& name, const std::initializer_list<Shader>& shaders);
};

#endif