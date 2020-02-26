#pragma once
#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include "AssetManager.h"

#include "../../graphics/text/Font.h"

class FontManager
	: public AssetManager<Font>
{
public:
	FontManager() = default;
	virtual ~FontManager() noexcept override = default;

	void AddFont(const std::string& name, const std::string& filepath, const unsigned int size);
};

#endif