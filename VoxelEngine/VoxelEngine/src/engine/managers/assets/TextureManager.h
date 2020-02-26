#pragma once
#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "AssetManager.h"

#include "../../graphics/Texture.h"

class TextureManager
	: public AssetManager<Texture>
{
public:
	TextureManager() = default;
	virtual ~TextureManager() noexcept override = default;

	void AddTexture(const std::string& name, const std::string& filepath);
};

#endif