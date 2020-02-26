#pragma once
#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "../interfaces/INoncopyable.h"

#include <SDL2/SDL.h>

#include <memory>

#include "Input.h"

class GameController
	: private INoncopyable
{
private:
	struct GameControllerDestroyer
	{
		void operator ()(SDL_GameController* gameController) const noexcept;
	};

	std::unique_ptr<SDL_GameController, GameControllerDestroyer> m_gameControllerHandle = nullptr;

public:
	static unsigned int GameControllerCount();

	GameController();

	GameController(GameController&& other) noexcept;
	GameController& operator =(GameController&& other) noexcept;

	~GameController() noexcept;

	inline SDL_GameController* GetPointer() const noexcept { return m_gameControllerHandle.get(); }

private:
	void Destroy() noexcept;
};

#endif