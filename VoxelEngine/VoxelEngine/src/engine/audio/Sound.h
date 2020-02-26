#pragma once
#ifndef SOUND_H
#define SOUND_H

#include "../interfaces/INoncopyable.h"

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include <string>

class Sound
	: private INoncopyable	
{
private:
	ALuint m_id = 0;

public:
	Sound(const std::string& filepath);

	Sound(Sound&& other) noexcept;
	Sound& operator =(Sound&& other) noexcept;

	~Sound() noexcept;

	inline ALuint GetID() const noexcept { return m_id; }

private:
	void SetAudioBufferData(const std::string& filepath) const;

	void Destroy() noexcept;
	ALuint ResetBuffer(const ALuint newBufferID) noexcept;
};

#endif