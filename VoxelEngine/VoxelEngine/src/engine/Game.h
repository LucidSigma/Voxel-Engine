#pragma once
#ifndef GAME_H
#define GAME_H

#include "interfaces/INoncopyable.h"
#include "interfaces/INonmovable.h"

#include <glm/glm.hpp>

#include <memory>
#include <queue>
#include <type_traits>
#include <utility>

#include "audio/AudioContext.h"
#include "graphics/window/GLWindow.h"
#include "input/GameController.h"
#include "managers/input/InputManager.h"
#include "scenes/GameScene.h"
#include "utility/config/Config.h"

class Game final
	: private INoncopyable, private INonmovable
{
private:
	std::unique_ptr<GLWindow> m_window = nullptr;
	bool m_hasFocus = true;

	bool m_isRunning = true;
	Uint32 m_ticksCount = 0;
	
	std::queue<std::unique_ptr<GameScene>> m_scenes;
	bool m_isCurrentSceneFinished = false;

	std::unique_ptr<AudioContext> m_audioContext;

	InputManager m_inputManager;
	std::unique_ptr<GameController> m_gameController = nullptr;
	
public:
	Game();
	~Game() noexcept;

	void Play();

	template <typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<GameScene, T> && std::is_constructible_v<T, Args...>>>
	void PushScene(Args&&... args)
	{
		m_scenes.push(std::make_unique<T>(std::forward<Args>(args)...));
		m_scenes.front()->OnLoad();
	}

	void FinishCurrentScene();

	inline const glm::uvec2& GetWindowSize() const noexcept { return m_window->GetSize(); }

private:
	void Initialise();
	void InitialiseSDL(const Config& config) const;
	void InitialiseWindow(const Config& config);
	void InitialiseGLAD() const;
	void InitialiseOpenGL() const;
	void InitialiseAudio();
	void InitialiseGameController();

	void PollEvents(SDL_Event& gameEvent);
	void ProcessInput();
	void Update();
	void Render() const;

	void UpdateSceneQueue();

	float CalculateDeltaTime();

	void TakeScreenshot() const;
	void ProcessWindowResize(const glm::uvec2& newSize);
};

#endif