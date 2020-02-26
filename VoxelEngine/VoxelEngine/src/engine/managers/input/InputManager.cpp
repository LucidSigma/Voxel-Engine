#include "InputManager.h"

#include <utility>

KeyboardState InputManager::GetKeyboardState()
{
	m_previousKeys = std::move(m_currentKeys);
	m_currentKeys = std::move(std::vector<Uint8>(SDL_GetKeyboardState(nullptr), SDL_GetKeyboardState(nullptr) + SDL_NUM_SCANCODES));

	return { m_currentKeys.data(), m_previousKeys.data() };
}

MouseState InputManager::GetMouseState()
{
	int x = 0;
	int y = 0;

	m_previousButtonStates = m_currentButtonStates;
	m_currentButtonStates = SDL_GetMouseState(&x, &y);

	int relativeX = 0;
	int relativeY = 0;
	SDL_GetRelativeMouseState(&relativeX, &relativeY);

	return MouseState{ m_currentButtonStates, m_previousButtonStates, static_cast<float>(x), static_cast<float>(y), static_cast<float>(relativeX), static_cast<float>(relativeY) };
}

Sint32 InputManager::GetScrollState() const
{
	return m_yScrollAmount;
}

GameControllerState InputManager::GetGameControllerState(const std::unique_ptr<GameController>& gameController)
{
	GameControllerState gameControllerState{ 0 };
	gameControllerState.previousButtons = m_currentButtons;
	gameControllerState.previousAxes = m_currentAxes;

	if (gameController != nullptr)
	{
		gameControllerState.currentButtons.dPad.up = SDL_GameControllerGetButton(gameController->GetPointer(), SDL_CONTROLLER_BUTTON_DPAD_UP) == 1;
		gameControllerState.currentButtons.dPad.down = SDL_GameControllerGetButton(gameController->GetPointer(), SDL_CONTROLLER_BUTTON_DPAD_DOWN) == 1;
		gameControllerState.currentButtons.dPad.left = SDL_GameControllerGetButton(gameController->GetPointer(), SDL_CONTROLLER_BUTTON_DPAD_LEFT) == 1;
		gameControllerState.currentButtons.dPad.right = SDL_GameControllerGetButton(gameController->GetPointer(), SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == 1;

		gameControllerState.currentButtons.a = SDL_GameControllerGetButton(gameController->GetPointer(), SDL_CONTROLLER_BUTTON_A) == 1;
		gameControllerState.currentButtons.b = SDL_GameControllerGetButton(gameController->GetPointer(), SDL_CONTROLLER_BUTTON_B) == 1;
		gameControllerState.currentButtons.x = SDL_GameControllerGetButton(gameController->GetPointer(), SDL_CONTROLLER_BUTTON_X) == 1;
		gameControllerState.currentButtons.y = SDL_GameControllerGetButton(gameController->GetPointer(), SDL_CONTROLLER_BUTTON_Y) == 1;

		gameControllerState.currentButtons.back = SDL_GameControllerGetButton(gameController->GetPointer(), SDL_CONTROLLER_BUTTON_BACK) == 1;
		gameControllerState.currentButtons.guide = SDL_GameControllerGetButton(gameController->GetPointer(), SDL_CONTROLLER_BUTTON_GUIDE) == 1;
		gameControllerState.currentButtons.start = SDL_GameControllerGetButton(gameController->GetPointer(), SDL_CONTROLLER_BUTTON_START) == 1;

		gameControllerState.currentButtons.leftStick = SDL_GameControllerGetButton(gameController->GetPointer(), SDL_CONTROLLER_BUTTON_LEFTSTICK) == 1;
		gameControllerState.currentButtons.rightStick = SDL_GameControllerGetButton(gameController->GetPointer(), SDL_CONTROLLER_BUTTON_RIGHTSTICK) == 1;
		
		gameControllerState.currentButtons.leftShoulder = SDL_GameControllerGetButton(gameController->GetPointer(), SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == 1;
		gameControllerState.currentButtons.rightShoulder = SDL_GameControllerGetButton(gameController->GetPointer(), SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == 1;

		gameControllerState.currentAxes.left.x = SDL_GameControllerGetAxis(gameController->GetPointer(), SDL_CONTROLLER_AXIS_LEFTX);
		gameControllerState.currentAxes.left.y = SDL_GameControllerGetAxis(gameController->GetPointer(), SDL_CONTROLLER_AXIS_LEFTY);
		gameControllerState.currentAxes.right.x = SDL_GameControllerGetAxis(gameController->GetPointer(), SDL_CONTROLLER_AXIS_RIGHTX);
		gameControllerState.currentAxes.right.y = SDL_GameControllerGetAxis(gameController->GetPointer(), SDL_CONTROLLER_AXIS_RIGHTY);

		gameControllerState.currentAxes.triggerLeft = SDL_GameControllerGetAxis(gameController->GetPointer(), SDL_CONTROLLER_AXIS_TRIGGERLEFT);
		gameControllerState.currentAxes.triggerRight= SDL_GameControllerGetAxis(gameController->GetPointer(), SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
	}

	m_currentButtons = gameControllerState.currentButtons;
	m_currentAxes = gameControllerState.currentAxes;

	return gameControllerState;
}
