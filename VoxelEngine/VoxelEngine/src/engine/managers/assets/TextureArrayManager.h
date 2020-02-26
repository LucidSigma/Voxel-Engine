#pragma once
#ifndef TEXTURE_ARRAY_MANAGER_H
#define TEXTURE_ARRAY_MANAGER_H

#include "AssetManager.h"

#include "../../graphics/TextureArray.h"

class TextureArrayManager
	: public AssetManager<TextureArray>
{
public:
	TextureArrayManager() = default;
	virtual ~TextureArrayManager() noexcept override = default;

	void AddTextureArray(const std::string& name, const std::string& filepath, const glm::uvec2& subTextureSize);
};

#endif