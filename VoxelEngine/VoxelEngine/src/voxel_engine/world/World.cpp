#include "World.h"

#include <magic_enum/magic_enum.hpp>

#include <iterator>
#include <utility>

#include "../../engine/Game.h"

glm::ivec3 World::WorldCoordinatesToChunkCoordinates(const int x, const int y, const int z)
{
	return glm::ivec3{
		glm::floor(x / static_cast<float>(Chunk::GetSize().x)) * Chunk::GetSize().x,
		glm::floor(y / static_cast<float>(Chunk::GetSize().y)) * Chunk::GetSize().y,
		glm::floor(z / static_cast<float>(Chunk::GetSize().z)) * Chunk::GetSize().z
	};
}

World::World(const TileMap& tileMap, const Camera3D& camera, const Sound& breakSound, const Sound& placeSound, const Sound& switchSound, const Game& game)
	: m_tileMap(&tileMap), m_camera(camera), m_blockBreakSound(breakSound), m_blockPlaceSound(placeSound), m_blockSwitchSound(switchSound), m_game(game)
{
	const glm::ivec3 cameraPosition = m_camera.GetPosition();

	for (int x = 0; x <= cameraPosition.x + s_RenderDistance; ++x)
	{
		for (unsigned int y = 0; y < s_ChunkHeight; ++y)
		{
			for (int z = 0; z <= cameraPosition.z + s_RenderDistance; ++z)
			{
				const glm::vec3 currentPosition{ x * static_cast<float>(Chunk::GetSize().x), y * static_cast<float>(Chunk::GetSize().y), z * static_cast<float>(Chunk::GetSize().z) };

				m_chunks.insert({ WorldCoordinatesToChunkCoordinates(static_cast<int>(currentPosition.x), static_cast<int>(currentPosition.y), static_cast<int>(currentPosition.z)), std::make_unique<Chunk>(currentPosition, *this, tileMap) });
			}
		}
	}

	for (const auto& treeLocation : m_treeLocations)
	{
		CreateTree(treeLocation.x, treeLocation.y, treeLocation.z);
	}

	for (auto& [position, chunk] : m_chunks)
	{
		chunk->Create();
	}
}

void World::ProcessInput(const KeyboardState& keyboardState, const MouseState& mouseState, const Sint32 scrollState, const GameControllerState& gameControllerState, const glm::mat4& perspectiveMatrix)
{
	constexpr Sint16 ControllerAxisDeadZone = 8'000;

	if (((mouseState.currentButtonStates & SDL_BUTTON(SDL_BUTTON_LEFT)) && !(mouseState.previousButtonStates & SDL_BUTTON(SDL_BUTTON_LEFT))) ||
		(gameControllerState.currentAxes.triggerLeft > ControllerAxisDeadZone && gameControllerState.previousAxes.triggerLeft <= ControllerAxisDeadZone))
	{
		float depth = 0.0f;
		glReadPixels(m_game.GetWindowSize().x / 2, m_game.GetWindowSize().y / 2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

		const glm::ivec3 clickPosition = glm::unProject(glm::vec3{ m_game.GetWindowSize().x / 2.0f, m_game.GetWindowSize().y / 2.0f, depth + 0.0001f }, m_camera.GetViewMatrix(), perspectiveMatrix, glm::vec4{ 0, 0, m_game.GetWindowSize() });
		const unsigned int distance = static_cast<unsigned int>(glm::abs(glm::distance(m_camera.GetPosition(), glm::vec3(clickPosition))));
		std::optional<Chunk*> chunk = GetChunkAt(clickPosition.x - clickPosition.x % Chunk::GetSize().x, clickPosition.y - clickPosition.y % Chunk::GetSize().y, clickPosition.z - clickPosition.z % Chunk::GetSize().z);

		if (distance < 32 && chunk.has_value() && DestroyBlockAt(clickPosition.x, clickPosition.y, clickPosition.z))
		{
			m_blockSoundSource.SetVolume(1.0f);
			m_blockSoundSource.SetPosition(clickPosition);
			m_blockSoundSource.Play(m_blockBreakSound);

			chunk.value()->Create();

			UpdateNearbyChunks(clickPosition);
		}
	}

	if (((mouseState.currentButtonStates & SDL_BUTTON(SDL_BUTTON_RIGHT)) && !(mouseState.previousButtonStates & SDL_BUTTON(SDL_BUTTON_RIGHT))) ||
		(gameControllerState.currentAxes.triggerRight > ControllerAxisDeadZone && gameControllerState.previousAxes.triggerRight <= ControllerAxisDeadZone))
	{
		float depth = 0.0f;
		glReadPixels(m_game.GetWindowSize().x / 2, m_game.GetWindowSize().y / 2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

		const glm::ivec3 clickPosition = glm::unProject(glm::vec3{ m_game.GetWindowSize().x / 2.0f, m_game.GetWindowSize().y / 2.0f, depth - 0.0001f }, m_camera.GetViewMatrix(), perspectiveMatrix, glm::vec4{ 0, 0, m_game.GetWindowSize() });
		const unsigned int distance = static_cast<unsigned int>(glm::abs(glm::distance(m_camera.GetPosition(), glm::vec3(clickPosition))));
		std::optional<Chunk*> chunk = GetChunkAt(clickPosition.x - clickPosition.x % Chunk::GetSize().x, clickPosition.y - clickPosition.y % Chunk::GetSize().y, clickPosition.z - clickPosition.z % Chunk::GetSize().z);

		if (distance < 32 && chunk.has_value() && PlaceBlockAt(clickPosition.x, clickPosition.y, clickPosition.z, m_selectedBlock))
		{
			m_blockSoundSource.SetVolume(1.0f);
			m_blockSoundSource.SetPosition(clickPosition);
			m_blockSoundSource.Play(m_blockPlaceSound);

			chunk.value()->Create();

			UpdateNearbyChunks(clickPosition);
		}
	}

	static constexpr unsigned short BlockCount = static_cast<unsigned short>(magic_enum::enum_count<Block>());

	if (scrollState > 0 || (gameControllerState.currentButtons.leftShoulder && !gameControllerState.previousButtons.leftShoulder))
	{
		unsigned short nextBlockID = static_cast<unsigned short>(m_selectedBlock);
		--nextBlockID;

		if (nextBlockID == 0)
		{
			nextBlockID = BlockCount - 1;
		}

		m_selectedBlock = static_cast<Block>(nextBlockID);

		m_blockSoundSource.SetPosition(m_camera.GetPosition());
		m_blockSoundSource.SetVolume(0.25f);
		m_blockSoundSource.Play(m_blockSwitchSound);
	}
	else if (scrollState < 0 || (gameControllerState.currentButtons.rightShoulder && !gameControllerState.previousButtons.rightShoulder))
	{
		unsigned short nextBlockID = static_cast<unsigned short>(m_selectedBlock);
		++nextBlockID;
		nextBlockID %= BlockCount;

		if (nextBlockID == 0)
		{
			nextBlockID = 1;
		}

		m_selectedBlock = static_cast<Block>(nextBlockID);

		m_blockSoundSource.SetPosition(m_camera.GetPosition());
		m_blockSoundSource.SetVolume(0.25f);
		m_blockSoundSource.Play(m_blockSwitchSound);
	}
}

void World::Render() const
{
	for (const auto& [position, chunk] : m_chunks)
	{
		chunk->Render();
	}
}

std::optional<Chunk*> World::GetChunkAt(const int x, const int y, const int z) const
{
	const glm::ivec3 position{ x, y, z };

	if (const auto location = m_chunks.find(position);
		location != std::cend(m_chunks))
	{
		return location->second.get();
	}
	else
	{
		return std::nullopt;
	}
}

void World::AddTreeLocation(const int x, const int y, const int z)
{
	m_treeLocations.insert(glm::ivec3{ x, y, z });
}

bool World::PlaceBlockAt(int x, int y, int z, const Block block)
{
	std::optional<Chunk*> chunk = GetChunkAt(x - x % Chunk::GetSize().x, y - y % Chunk::GetSize().y, z - z % Chunk::GetSize().z);

	x %= Chunk::GetSize().x;
	y %= Chunk::GetSize().y;
	z %= Chunk::GetSize().z;

	if (chunk.has_value() && chunk.value()->GetBlockAt(x, y, z) == Block::Air)
	{
		chunk.value()->SetBlockAt(x, y, z, block);

		return true;
	}

	return false;
}

bool World::DestroyBlockAt(int x, int y, int z)
{
	std::optional<Chunk*> chunk = GetChunkAt(x - x % Chunk::GetSize().x, y - y % Chunk::GetSize().y, z - z % Chunk::GetSize().z);

	x %= Chunk::GetSize().x;
	y %= Chunk::GetSize().y;
	z %= Chunk::GetSize().z;

	if (chunk.has_value() && chunk.value()->GetBlockAt(x, y, z) != Block::Air)
	{
		chunk.value()->SetBlockAt(x, y, z, Block::Air);

		return true;
	}

	return false;
}

void World::UpdateNearbyChunks(const glm::ivec3& position)
{
	if (position.x % Chunk::GetSize().x == 0)
	{
		std::optional<Chunk*> neighbourChunk = GetChunkAt(position.x - position.x % Chunk::GetSize().x - static_cast<int>(Chunk::GetSize().x), position.y - position.y % Chunk::GetSize().y, position.z - position.z % Chunk::GetSize().z);

		if (neighbourChunk.has_value())
		{
			neighbourChunk.value()->Create();
		}
	}
	else if (position.x % Chunk::GetSize().x == Chunk::GetSize().x - 1)
	{
		std::optional<Chunk*> neighbourChunk = GetChunkAt(position.x - position.x % Chunk::GetSize().x + static_cast<int>(Chunk::GetSize().x), position.y - position.y % Chunk::GetSize().y, position.z - position.z % Chunk::GetSize().z);

		if (neighbourChunk.has_value())
		{
			neighbourChunk.value()->Create();
		}
	}

	if (position.y % Chunk::GetSize().y == 0)
	{
		std::optional<Chunk*> neighbourChunk = GetChunkAt(position.x - position.x % Chunk::GetSize().x, position.y - position.y % Chunk::GetSize().y - static_cast<int>(Chunk::GetSize().y), position.z - position.z % Chunk::GetSize().z);

		if (neighbourChunk.has_value())
		{
			neighbourChunk.value()->Create();
		}
	}
	else if (position.y % Chunk::GetSize().y == Chunk::GetSize().y - 1)
	{
		std::optional<Chunk*> neighbourChunk = GetChunkAt(position.x - position.x % Chunk::GetSize().x, position.y - position.y % Chunk::GetSize().y + static_cast<int>(Chunk::GetSize().y), position.z - position.z % Chunk::GetSize().z);

		if (neighbourChunk.has_value())
		{
			neighbourChunk.value()->Create();
		}
	}

	if (position.z % Chunk::GetSize().z == 0)
	{
		std::optional<Chunk*> neighbourChunk = GetChunkAt(position.x - position.x % Chunk::GetSize().x, position.y - position.y % Chunk::GetSize().y, position.z - position.z % Chunk::GetSize().z - static_cast<int>(Chunk::GetSize().z));

		if (neighbourChunk.has_value())
		{
			neighbourChunk.value()->Create();
		}
	}
	else if (position.z % Chunk::GetSize().z == Chunk::GetSize().z - 1)
	{
		std::optional<Chunk*> neighbourChunk = GetChunkAt(position.x - position.x % Chunk::GetSize().x, position.y - position.y % Chunk::GetSize().y, position.z - position.z % Chunk::GetSize().z + static_cast<int>(Chunk::GetSize().z));

		if (neighbourChunk.has_value())
		{
			neighbourChunk.value()->Create();
		}
	}
}

void World::CreateTree(const int x, const int y, const int z)
{
	constexpr int TreeHeight = 6;

	for (int i = 0; i < TreeHeight; ++i)
	{
		PlaceBlockAt(x, y + i, z, Block::Log);
	}

	constexpr int MinLeafHeight = 4;
	constexpr int MaxLeafHeight = 8;
	
	int leafRadius = 3;

	for (int i = MinLeafHeight; i <= MaxLeafHeight; ++i)
	{
		for (int j = -leafRadius; j <= leafRadius; ++j)
		{
			for (int k = -leafRadius; k <= leafRadius; ++k)
			{
				if (j == x && k == z)
				{
					continue;
				}

				PlaceBlockAt(x + j, y + i, z + k, Block::Leaves);
			}
		}

		if (i == MinLeafHeight + 1 || i == MinLeafHeight + 3)
		{
			--leafRadius;
		}
	}
}