#include "AudioContext.h"

#include <array>
#include <sstream>

#include "../error/GameException.h"

void AudioContext::SetDistanceModel(const ALenum distanceModel)
{
	alDistanceModel(distanceModel);
}

void AudioContext::SetListenerPosition(const glm::vec3& position)
{
	alListener3f(AL_POSITION, position.x, position.y, position.z);
}

void AudioContext::SetListenerVelocity(const glm::vec3& velocity)
{
	alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

void AudioContext::SetListenerOrientation(const glm::vec3& lookingDirection, const glm::vec3& up)
{
	const std::array<float, 6u> listenerOrientation{
		lookingDirection.x, lookingDirection.y, lookingDirection.z,
		up.x, up.y, up.z
	};

	alListenerfv(AL_ORIENTATION, listenerOrientation.data());
}

AudioContext::AudioContext()
{
	CreateDevice();
	CreateContext();
}

AudioContext::~AudioContext() noexcept
{
	m_audioDevice = alcGetContextsDevice(m_audioContext);
	alcMakeContextCurrent(nullptr);

	alcDestroyContext(m_audioContext);
	alcCloseDevice(m_audioDevice);
}

void AudioContext::CreateDevice()
{
	m_audioDevice = alcOpenDevice(nullptr);

	if (m_audioDevice == nullptr)
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "Failed to initialise OpenAL device.";

		throw GameException(GameException::Initialise::OpenAL, errorMessageStream.str());
	}
}

void AudioContext::CreateContext()
{
	m_audioContext = alcCreateContext(m_audioDevice, nullptr);

	if (m_audioContext == nullptr)
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "Failed to create OpenAL context.";

		throw GameException(GameException::Initialise::OpenAL, errorMessageStream.str());
	}

	if (alcMakeContextCurrent(m_audioContext) == AL_FALSE)
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "Failed to make a current OpenAL context.";

		throw GameException(GameException::Initialise::OpenAL, errorMessageStream.str());
	}
}