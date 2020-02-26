#pragma once
#ifndef CUBEMAP_MANAGER_H
#define CUBEMAP_MANAGER_H

#include "AssetManager.h"

#include "../../graphics/Cubemap.h"

class CubemapManager
	: public AssetManager<Cubemap>
{
public:
	CubemapManager() = default;
	virtual ~CubemapManager() noexcept override = default;

	void AddCubemap(const std::string& name, const std::string& filepath);
};

#endif