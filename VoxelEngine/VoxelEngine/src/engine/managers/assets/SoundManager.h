#pragma once
#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include "AssetManager.h"

#include "../../audio/Sound.h"

class SoundManager
	: public AssetManager<Sound>
{
public:
	SoundManager() = default;
	virtual ~SoundManager() noexcept override = default;

	void AddSound(const std::string& name, const std::string& filepath);
};

#endif