#include "Game.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <glad/glad.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <SDL2/SDL.h>
#include <stb_image/stb_image.h>
#include <stb_image/stb_image_write.h>

#include <chrono>
#include <cstddef>
#include <initializer_list>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "error/GameException.h"
#include "utility/config/ConfigReader.h"

#ifdef _WIN32
extern "C"
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	_declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformace = 0x00000001;
}
#endif

Game::Game()
{
	Initialise();
}

Game::~Game() noexcept
{
	m_gameController = nullptr;
	m_window = nullptr;
	m_audioContext = nullptr;

	SDL_Quit();
}

void Game::Play()
{
	m_isRunning = !m_scenes.empty();
	SDL_Event gameEvent{ };

	while (m_isRunning)
	{
		PollEvents(gameEvent);
		ProcessInput();
		Update();
		Render();

		UpdateSceneQueue();
	}
}

void Game::FinishCurrentScene()
{
	m_isCurrentSceneFinished = true;
}

void Game::Initialise()
{
	const Config config = ConfigReader().ReadConfigFile();

	InitialiseSDL(config);
	InitialiseWindow(config);
	InitialiseGLAD();
	InitialiseOpenGL();
	InitialiseAudio();
	InitialiseGameController();

	stbi_set_flip_vertically_on_load(true);
	stbi_flip_vertically_on_write(true);
}

void Game::InitialiseSDL(const Config& config) const
{
	constexpr int SDLSuccess = 0;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != SDLSuccess)
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "Failed to initialise SDL: " << SDL_GetError();

		throw GameException(GameException::Initialise::SDL, errorMessageStream.str());
	}

	const std::initializer_list<std::pair<SDL_GLattr, int>> glAttributes{
		{ SDL_GL_DOUBLEBUFFER, SDL_TRUE },
		{ SDL_GL_CONTEXT_MAJOR_VERSION, config.glVersionMajor },
		{ SDL_GL_CONTEXT_MINOR_VERSION, config.glVersionMinor },
		{ SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE },
	#ifdef __APPLE__
		{ SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG },
	#endif
		{ SDL_GL_MULTISAMPLESAMPLES, config.multisampleCount },
		{ SDL_GL_STENCIL_SIZE, 8 }
	};

	for (const auto [attribute, value] : glAttributes)
	{
		SDL_GL_SetAttribute(attribute, value);
	}
}

void Game::InitialiseWindow(const Config& config)
{
	const SDL_Rect windowRect{
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		static_cast<int>(config.windowWidth),
		static_cast<int>(config.windowHeight)
	};

	m_window = std::make_unique<GLWindow>(config.title.data(), windowRect);
	m_window->SetIcon("assets/icon/icon.png");
	m_window->MakeCurrentContext();

	constexpr int SDLSuccess = 0;

	if (config.enableVsync)
	{
		if (SDL_GL_SetSwapInterval(1) != SDLSuccess)
		{
			std::ostringstream errorMessageStream;
			errorMessageStream << "Failed to setup Vsync: " << SDL_GetError();

			throw GameException(GameException::Initialise::SDL, errorMessageStream.str());
		}
	}

	if (SDL_CaptureMouse(SDL_TRUE) != SDLSuccess)
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "Failed to capture mouse cursor: " << SDL_GetError();
	
		throw GameException(GameException::Initialise::SDL, errorMessageStream.str());
	}

	if (SDL_SetRelativeMouseMode(SDL_TRUE) != SDLSuccess)
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "Failed to switch to relative mouse mode: " << SDL_GetError();
	
		throw GameException(GameException::Initialise::SDL, errorMessageStream.str());
	}
	
	SDL_GetRelativeMouseState(nullptr, nullptr);
}

void Game::InitialiseGLAD() const
{
	if (!gladLoadGLLoader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "Failed to initialise GLAD.";

		throw GameException(GameException::Initialise::SDL, errorMessageStream.str());
	}
}

void Game::InitialiseOpenGL() const
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_MULTISAMPLE);

	glViewport(0, 0, m_window->GetSize().x, m_window->GetSize().y);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Game::InitialiseAudio()
{
	m_audioContext = std::make_unique<AudioContext>();

	AudioContext::SetDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);

	AudioContext::SetListenerPosition({ m_window->GetSize().x / 2.0f, m_window->GetSize().y / 2.0f, 0.0f });
	AudioContext::SetListenerVelocity(glm::vec3{ 0.0f, 0.0f, 0.0f });
	AudioContext::SetListenerOrientation(glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f });
}

void Game::InitialiseGameController()
{
	if (GameController::GameControllerCount() >= 1)
	{
		m_gameController = std::make_unique<GameController>();
	}
}

void Game::PollEvents(SDL_Event& gameEvent)
{
	m_inputManager.SetScrollAmount(0);

	while (SDL_PollEvent(&gameEvent))
	{
		switch (gameEvent.type)
		{
		case SDL_KEYDOWN:
			switch (gameEvent.key.keysym.sym)
			{
			case SDLK_F2:
				TakeScreenshot();

				break;

			case SDLK_F11:
				m_window->ToggleFullscreen();

				break;

			default:
				break;
			}
			
			break;

		case SDL_MOUSEWHEEL:
			m_inputManager.SetScrollAmount(gameEvent.wheel.y);

			break;

		case SDL_WINDOWEVENT:
			switch (gameEvent.window.event)
			{
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				ProcessWindowResize(glm::uvec2{ gameEvent.window.data1, gameEvent.window.data2 });

				break;

			case SDL_WINDOWEVENT_FOCUS_GAINED:
				if (!m_hasFocus)
				{
					m_hasFocus = true;
				}

				SDL_GetRelativeMouseState(nullptr, nullptr);

				break;

			case SDL_WINDOWEVENT_MINIMIZED:
			case SDL_WINDOWEVENT_FOCUS_LOST:
				if (m_hasFocus)
				{
					m_hasFocus = false;
				}

				break;

			default:
				break;
			}

			break;

		case SDL_CONTROLLERDEVICEADDED:
			if (gameEvent.jdevice.which == 0)
			{
				m_gameController = std::make_unique<GameController>();
			}

			break;

		case SDL_CONTROLLERDEVICEREMOVED:
			m_gameController = nullptr;

			break;

		case SDL_QUIT:
			m_isRunning = false;

			break;

		default:
			break;
		}

		m_scenes.front()->PollEvents(gameEvent);
	}
}

void Game::ProcessInput()
{
	if (m_hasFocus)
	{
		const KeyboardState keyboardState = m_inputManager.GetKeyboardState();
		const MouseState mouseState = m_inputManager.GetMouseState();
		const Sint32 scrollOffset = m_inputManager.GetScrollState();
		const GameControllerState gameControllerState = m_inputManager.GetGameControllerState(m_gameController);

		m_scenes.front()->ProcessInput(keyboardState, mouseState, scrollOffset, gameControllerState);
	}
}

void Game::Update()
{
	const float deltaTime = CalculateDeltaTime();

	if (m_hasFocus)
	{
		m_scenes.front()->Update(deltaTime);
	}
}

void Game::Render() const
{
	if (m_hasFocus)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_scenes.front()->Render();

		m_window->SwapBuffers();
	}
}

void Game::UpdateSceneQueue()
{
	if (m_isCurrentSceneFinished)
	{
		m_scenes.front()->OnUnload();
		m_scenes.pop();

		m_isCurrentSceneFinished = false;
	}

	if (m_scenes.empty())
	{
		m_isRunning = false;
	}
}

float Game::CalculateDeltaTime()
{
	constexpr float MillisecondsPerSecond = 1000.0f;

	const float deltaTime = static_cast<float>(SDL_GetTicks() - m_ticksCount) / MillisecondsPerSecond;
	m_ticksCount = SDL_GetTicks();

	return deltaTime;
}

void Game::TakeScreenshot() const
{
	constexpr size_t PixelChannelCount = 3u;
	std::vector<std::byte> imageData(PixelChannelCount * m_window->GetSize().x * m_window->GetSize().y);

	glReadPixels(0, 0, m_window->GetSize().x, m_window->GetSize().y, GL_RGB, GL_UNSIGNED_BYTE, imageData.data());

	const long long currentTime = std::chrono::system_clock::now().time_since_epoch().count();
	const std::string filename = "screenshots/screenshot_" + std::to_string(currentTime) + ".png";

	stbi_write_png(filename.c_str(), m_window->GetSize().x, m_window->GetSize().y, PixelChannelCount, imageData.data(), m_window->GetSize().x * PixelChannelCount);
}

void Game::ProcessWindowResize(const glm::uvec2& newSize)
{
	m_window->ProcessResize(newSize);
	glViewport(0, 0, m_window->GetSize().x, m_window->GetSize().y);
}