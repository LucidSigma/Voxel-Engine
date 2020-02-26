#pragma once
#ifndef VOXEL_ENGINE_H
#define VOXEL_ENGINE_H

#include "../engine/scenes/GameScene.h"

#include <glm/glm.hpp>

#include <memory>

#include "../engine/camera/Camera3D.h"
#include "../engine/graphics/Sprite.h"
#include "../engine/graphics/text/Text.h"
#include "../engine/graphics/TileMap.h"
#include "../engine/managers/assets/CubemapManager.h"
#include "../engine/managers/assets/FontManager.h"
#include "../engine/managers/assets/ShaderManager.h"
#include "../engine/managers/assets/SoundManager.h"
#include "../engine/managers/assets/TextureManager.h"
#include "../engine/managers/assets/TextureArrayManager.h"
#include "Skybox.h"
#include "world/World.h"

class VoxelEngine final
	: public GameScene
{
private:
	static constexpr float s_NearPlane = 0.1f;
	static constexpr float s_FarPlane = 1000.0f;

	ShaderManager m_shaders;
	TextureManager m_textures;
	TextureArrayManager m_textureArrays;
	CubemapManager m_cubemaps;
	SoundManager m_sounds;
	FontManager m_fonts;

	std::unique_ptr<TileMap> m_blockTileMap;
	std::unique_ptr<World> m_world;

	Camera3D m_camera{ glm::vec3{ 0.0f, 48.0f, 0.0f } };
	glm::mat4 m_perspectiveMatrix{ 1.0f };
	glm::mat4 m_orthogonalMatrix{ 1.0f };

	std::unique_ptr<Skybox> m_skybox;
	std::unique_ptr<Sprite> m_crosshair;

	std::unique_ptr<Text> m_selectedBlockText;

public:
	explicit VoxelEngine(Game& game);
	virtual ~VoxelEngine() noexcept override = default;

	virtual void OnLoad() override;
	virtual void OnUnload() noexcept override;

	virtual void PollEvents(const SDL_Event& gameEvent) override;
	virtual void ProcessInput(const KeyboardState& keyboardState, const MouseState& mouseState, const Sint32 scrollState, const GameControllerState& gameControllerState) override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
};

#endif