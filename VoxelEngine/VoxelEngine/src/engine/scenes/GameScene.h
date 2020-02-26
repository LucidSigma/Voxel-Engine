#pragma once
#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "../interfaces/INoncopyable.h"
#include "../interfaces/INonmovable.h"

#include <SDL2/SDL.h>

#include "../input/Input.h"
#include "../input/GameController.h"

class Game;

class GameScene
	: private INoncopyable, private INonmovable
{
protected:
	Game& m_game;

public:
	explicit GameScene(Game& game);
	virtual ~GameScene() noexcept = default;

	virtual void OnLoad() = 0;
	virtual void OnUnload() noexcept = 0;

	virtual void PollEvents(const SDL_Event& gameEvent) = 0;
	virtual void ProcessInput(const KeyboardState& keyboardState, const MouseState& mouseState, const Sint32 scrollState, const GameControllerState& gameControllerState) = 0;
	virtual void Update(const float deltaTime) = 0;
	virtual void Render() const = 0;
};

#endif