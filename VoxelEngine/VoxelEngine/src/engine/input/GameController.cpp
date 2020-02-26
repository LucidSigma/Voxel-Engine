#include "GameController.h"

#include <sstream>
#include <utility>

#include "../error/GameException.h"

void GameController::GameControllerDestroyer::operator()(SDL_GameController* gameController) const noexcept
{
	SDL_GameControllerClose(gameController);
}

unsigned int GameController::GameControllerCount()
{
	return static_cast<unsigned int>(SDL_NumJoysticks());
}

GameController::GameController()
	: m_gameControllerHandle(SDL_GameControllerOpen(0))
{
	if (m_gameControllerHandle == nullptr)
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "Failed to open game controller: " << SDL_GetError();

		throw GameException(GameException::Initialise::SDL, errorMessageStream.str());
	}
}

GameController::GameController(GameController&& other) noexcept
	: m_gameControllerHandle(nullptr)
{
	std::swap(m_gameControllerHandle, other.m_gameControllerHandle);
}

GameController& GameController::operator=(GameController&& other) noexcept
{
	m_gameControllerHandle = std::exchange(other.m_gameControllerHandle, nullptr);

	return *this;
}

GameController::~GameController() noexcept
{
	Destroy();
}

void GameController::Destroy() noexcept
{
	if (m_gameControllerHandle != nullptr)
	{
		m_gameControllerHandle = nullptr;
	}
}