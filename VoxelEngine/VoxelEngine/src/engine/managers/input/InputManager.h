#pragma once
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <SDL2/SDL.h>

#include <vector>

#include "../../input/Input.h"
#include "../../input/GameController.h"

class InputManager
{
private:
	Sint32 m_yScrollAmount = 0;

	std::vector<Uint8> m_previousKeys{ std::vector<Uint8>(SDL_NUM_SCANCODES, SDL_FALSE) };
	std::vector<Uint8> m_currentKeys{ std::vector<Uint8>(SDL_NUM_SCANCODES, SDL_FALSE) };

	Uint32 m_previousButtonStates = 0;
	Uint32 m_currentButtonStates = 0;

	GameControllerState::Axes m_currentAxes{ };
	GameControllerState::Buttons m_currentButtons{ };
	
public:
	InputManager() = default;
	~InputManager() noexcept = default;

	KeyboardState GetKeyboardState();
	MouseState GetMouseState();
	Sint32 GetScrollState() const;
	GameControllerState GetGameControllerState(const std::unique_ptr<GameController>& gameController);

	inline void SetScrollAmount(const Sint32 amount) noexcept { m_yScrollAmount = amount; }
};

#endif