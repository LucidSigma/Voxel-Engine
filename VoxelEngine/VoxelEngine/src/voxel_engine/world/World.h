#pragma once
#ifndef WORLD_H
#define WORLD_H

#include "../../engine/interfaces/INoncopyable.h"
#include "../../engine/interfaces/INonmovable.h"

#include <glm/glm.hpp>

#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "../../engine/audio/AudioSource.h"
#include "../../engine/audio/Sound.h"
#include "../../engine/camera/Camera3D.h"
#include "../../engine/graphics/TileMap.h"
#include "Chunk.h"

class Game;

namespace std
{
	template <typename T>
	struct hash<glm::tvec3<T>>
	{
		inline size_t operator() (const glm::tvec3<T>& vector) const noexcept { return (size_t(vector.x) << 1) + (size_t(vector.y) << 3) + (size_t(vector.z) << 5); }
	};
}

class World
	: private INoncopyable, private INonmovable
{
private:
	static constexpr int s_RenderDistance = 16;
	static constexpr unsigned int s_ChunkHeight = 4u;

	const Game& m_game;

	const TileMap* m_tileMap = nullptr;
	const Camera3D& m_camera;

	std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>> m_chunks;
	std::unordered_set<glm::ivec3> m_treeLocations;

	Block m_selectedBlock = Block::Planks;

	AudioSource m_blockSoundSource;
	const Sound& m_blockBreakSound;
	const Sound& m_blockPlaceSound;
	const Sound& m_blockSwitchSound;

public:
	static glm::ivec3 WorldCoordinatesToChunkCoordinates(const int x, const int y, const int z);

	World(const TileMap& tileMap, const Camera3D& camera, const Sound& breakSound, const Sound& placeSound, const Sound& switchSound, const Game& game);
	~World() noexcept = default;

	void ProcessInput(const KeyboardState& keyboardState, const MouseState& mouseState, const Sint32 scrollState, const GameControllerState& gameControllerState, const glm::mat4& perspectiveMatrix);
	void Render() const;

	std::optional<Chunk*> GetChunkAt(const int x, const int y, const int z) const;

	void AddTreeLocation(const int x, const int y, const int z);

	inline Block GetSelectedBlock() const noexcept { return m_selectedBlock; }

private:
	bool PlaceBlockAt(int x, int y, int z, const Block block);
	bool DestroyBlockAt(int x, int y, int z);
	void UpdateNearbyChunks(const glm::ivec3& position);

	void CreateTree(const int x, const int y, const int z);
};

#endif