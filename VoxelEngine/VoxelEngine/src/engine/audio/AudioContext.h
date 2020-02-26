#pragma once
#ifndef AUDIO_CONTEXT_H
#define AUDIO_CONTEXT_H

#include "../interfaces/INoncopyable.h"
#include "../interfaces/INonmovable.h"

#include <glm/glm.hpp>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

class AudioContext
	: private INoncopyable, private INonmovable
{
private: 
	ALCdevice* m_audioDevice = nullptr;
	ALCcontext* m_audioContext = nullptr;

public:
	static void SetDistanceModel(const ALenum distanceModel);

	static void SetListenerPosition(const glm::vec3& position);
	static void SetListenerVelocity(const glm::vec3& velocity);
	static void SetListenerOrientation(const glm::vec3& lookingDirection, const glm::vec3& up);

	AudioContext();
	~AudioContext() noexcept;

private:
	void CreateDevice();
	void CreateContext();
};

#endif