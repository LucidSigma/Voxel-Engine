#include "Sound.h"

#include <SDL2/SDL.h>

#include <sstream>
#include <utility>

#include "../error/GameException.h"

Sound::Sound(const std::string& filepath)
{
	alGenBuffers(1, &m_id);
	SetAudioBufferData(filepath);
}

Sound::Sound(Sound&& other) noexcept
	: m_id(ResetBuffer(other.m_id))
{
	other.m_id = 0;
}

Sound& Sound::operator=(Sound&& other) noexcept
{
	if (m_id != 0)
	{
		Destroy();
	}

	m_id = std::exchange(other.m_id, 0);

	return *this;
}

Sound::~Sound() noexcept
{
	Destroy();
}

void Sound::SetAudioBufferData(const std::string& filepath) const
{
	SDL_AudioSpec audioSpec{ };
	unsigned int audioLength = 0;
	Uint8* audioData = nullptr;
	
	if (SDL_LoadWAV(filepath.c_str(), &audioSpec, &audioData, &audioLength) == nullptr)
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "Failed to load audio file: " << filepath << ".";

		throw GameException(GameException::FileNotFound::Audio, errorMessageStream.str());
	}

	const ALenum format = audioSpec.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
	alBufferData(m_id, format, audioData, audioLength, audioSpec.freq);
	SDL_FreeWAV(audioData);
}

void Sound::Destroy() noexcept
{
	alDeleteBuffers(1, &m_id);
	m_id = 0;
}

ALuint Sound::ResetBuffer(const ALuint newBufferID) noexcept
{
	if (m_id != 0)
	{
		Destroy();
	}

	return newBufferID;
}