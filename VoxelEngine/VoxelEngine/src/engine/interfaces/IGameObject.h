#pragma once
#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <SDL2/SDL.h>

#include <memory>

#include "../input/GameController.h"
#include "../input/Input.h"

class IGameObject
{
public:
	IGameObject() = default;
	virtual ~IGameObject() noexcept = default;

	virtual void ProcessInput(const KeyboardState& keyboardState, const MouseState& mouseState, const Sint32 scrollState, const GameControllerState& gameControllerState) = 0;
	virtual void Update(const float deltaTime) = 0;
	virtual void Render() const = 0;
};

#endif