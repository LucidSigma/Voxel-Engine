#include "VoxelEngine.h"

#include <magic_enum/magic_enum.hpp>

#include "../engine/Game.h"
#include "../engine/audio/AudioContext.h"

VoxelEngine::VoxelEngine(Game& game)
	: GameScene(game)
{ }

void VoxelEngine::OnLoad()
{
	m_orthogonalMatrix = glm::ortho(0.0f, static_cast<float>(m_game.GetWindowSize().x), static_cast<float>(m_game.GetWindowSize().y), 0.0f);

	m_shaders.AddShader("sprite", { Shader(Shader::Type::Vertex, "shaders/sprite.vert"), Shader(Shader::Type::Fragment, "shaders/sprite.frag") });
	m_shaders["sprite"].Use();
	m_shaders["sprite"].SetUniform<glm::mat4>("u_Projection", m_orthogonalMatrix);
	m_shaders["sprite"].SetTextureUniform("u_SpriteTexture", 0);

	m_shaders.AddShader("chunk", { Shader(Shader::Type::Vertex, "shaders/chunk.vert"), Shader(Shader::Type::Fragment, "shaders/chunk.frag") });
	m_shaders["chunk"].Use();
	m_shaders["chunk"].SetTextureUniform("u_BlockTexture", 0);

	m_shaders.AddShader("skybox", { Shader(Shader::Type::Vertex, "shaders/skybox.vert"), Shader(Shader::Type::Fragment, "shaders/skybox.frag") });
	m_shaders["skybox"].Use();
	m_shaders["skybox"].SetTextureUniform("u_SkyboxCubemap", 0);

	m_shaders.AddShader("text", { Shader(Shader::Type::Vertex, "shaders/text.vert"), Shader(Shader::Type::Fragment, "shaders/text.frag") });
	m_shaders["text"].Use();
	m_shaders["text"].SetUniform<glm::mat4>("u_Projection", m_orthogonalMatrix);
	m_shaders["text"].SetTextureUniform("u_Text", 0);

	m_textures.AddTexture("crosshair", "assets/textures/crosshair.png");
	m_textureArrays.AddTextureArray("blocks", "assets/textures/blocks.png", glm::uvec2{ 16, 16 });
	m_cubemaps.AddCubemap("skybox", "assets/textures/cubemaps/skybox");
	m_sounds.AddSound("break", "assets/sounds/break.wav");
	m_sounds.AddSound("place", "assets/sounds/place.wav");
	m_sounds.AddSound("switch", "assets/sounds/switch.wav");
	m_fonts.AddFont("font", "assets/fonts/AldotheApache.ttf", 48);

	m_blockTileMap = std::make_unique<TileMap>(m_textureArrays["blocks"], glm::uvec2{ 16, 16 }, std::vector<std::string_view>{ "stone", "dirt", "grass_top", "grass_side", "bedrock", "log_top", "log_side", "leaves", "planks", "cobblestone", "glass", "pumpkin_bottom", "pumpkin_top", "pumpkin_side" });
	m_world = std::make_unique<World>(*m_blockTileMap, m_camera, m_sounds["break"], m_sounds["place"], m_sounds["switch"], m_game);

	m_skybox = std::make_unique<Skybox>(m_shaders["skybox"], m_cubemaps["skybox"]);
	m_crosshair = std::make_unique<Sprite>(m_shaders["sprite"], m_textures["crosshair"]);

	m_selectedBlockText = std::make_unique<Text>(m_fonts["font"], m_shaders["text"]);
}

void VoxelEngine::OnUnload() noexcept
{
	
}

void VoxelEngine::PollEvents(const SDL_Event& gameEvent)
{
	switch (gameEvent.type)
	{
	case SDL_KEYDOWN:
		switch (gameEvent.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			m_game.FinishCurrentScene();

			break;

		case SDLK_SPACE:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			break;

		default:
			break;
		}

		break;

	case SDL_KEYUP:
		switch (gameEvent.key.keysym.sym)
		{
		case SDLK_SPACE:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			break;

		default:
			break;
		}

		break;

	case SDL_CONTROLLERBUTTONDOWN:
		switch (gameEvent.cbutton.button)
		{
		case SDL_CONTROLLER_BUTTON_START:
			m_game.FinishCurrentScene();

			break;

		case SDL_CONTROLLER_BUTTON_LEFTSTICK:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		default:
			break;
		}

		break;

	case SDL_CONTROLLERBUTTONUP:
		switch (gameEvent.cbutton.button)
		{
		case SDL_CONTROLLER_BUTTON_LEFTSTICK:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		default:
			break;
		}

		break;

	case SDL_WINDOWEVENT:
		switch (gameEvent.window.event)
		{
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			m_perspectiveMatrix = glm::perspective(glm::radians(m_camera.GetZoom()), static_cast<float>(m_game.GetWindowSize().x) / static_cast<float>(m_game.GetWindowSize().y), s_NearPlane, s_FarPlane);
			m_orthogonalMatrix = glm::ortho(0.0f, static_cast<float>(m_game.GetWindowSize().x), static_cast<float>(m_game.GetWindowSize().y), 0.0f);

			m_shaders["sprite"].Use();
			m_shaders["sprite"].SetUniform<glm::mat4>("u_Projection", m_orthogonalMatrix);

			break;

		default:
			break;
		}

		break;

		break;

	default:
		break;
	}
}

void VoxelEngine::ProcessInput(const KeyboardState& keyboardState, const MouseState& mouseState, const Sint32 scrollState, const GameControllerState& gameControllerState)
{
	m_camera.ProcessInput(keyboardState, mouseState, scrollState, gameControllerState);
	m_world->ProcessInput(keyboardState, mouseState, scrollState, gameControllerState, m_perspectiveMatrix);
}

void VoxelEngine::Update(const float deltaTime)
{
	m_camera.Update(deltaTime);
	m_perspectiveMatrix = glm::perspective(glm::radians(m_camera.GetZoom()), static_cast<float>(m_game.GetWindowSize().x) / static_cast<float>(m_game.GetWindowSize().y), s_NearPlane, s_FarPlane);

	AudioContext::SetListenerPosition(m_camera.GetPosition());
	AudioContext::SetListenerOrientation(m_camera.GetFront(), glm::vec3{ 0.0f, 1.0f, 0.0f });
}

void VoxelEngine::Render() const
{
	m_shaders["chunk"].Use();
	m_shaders["chunk"].SetUniform<glm::mat4>("u_MVP", m_perspectiveMatrix * m_camera.GetViewMatrix());
	m_world->Render();

	m_skybox->Render(glm::mat4(glm::mat3(m_camera.GetViewMatrix())), m_perspectiveMatrix);

	constexpr float CrosshairSize = 24.0f;
	m_crosshair->Render(glm::vec2{ m_game.GetWindowSize().x / 2.0f - (CrosshairSize / 2.0f), m_game.GetWindowSize().y / 2.0f - (CrosshairSize / 2.0f) }, glm::vec2{ CrosshairSize, CrosshairSize });

	m_selectedBlockText->Render("Selected block: " + std::string(magic_enum::enum_name(m_world->GetSelectedBlock())), glm::vec2{ 5.0f, 5.0f }, 1.0f, glm::vec3{ 0.1f, 0.1f, 0.1f });
}