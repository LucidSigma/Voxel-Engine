#pragma once
#ifndef CHUNK_H
#define CHUNK_H

#include "../../engine/graphics/Drawable.h"

#include <glm/glm.hpp>

#include <cstddef>
#include <limits>
#include <string>
#include <vector>

#include "../../engine/interfaces/INonmovable.h"

#include "../../engine/graphics/TileMap.h"
#include "../../engine/graphics/Vertex.h"
#include "../../engine/utility/Random.h"
#include "Block.h"

class World;

class Chunk
	: public Drawable, private INonmovable
{
private:
	static constexpr glm::tvec3<size_t> s_ChunkSize{ 16u, 16u, 16u };
	inline static const unsigned int s_seed = Random::GenerateInt(0u, static_cast<unsigned int>(std::numeric_limits<unsigned short>::max()));

	glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
	std::vector<Block> m_blocks{ s_ChunkSize.x * s_ChunkSize.y * s_ChunkSize.z, Block::Air };

	World& m_world;
	const TileMap* m_tileMap = nullptr;

	size_t m_faceCount = 0;
	size_t m_elementCount = 0;
	
	std::vector<Vertex3D> m_vertices;
	std::vector<unsigned short> m_elements;

	bool m_isReadyToRender = false;

public:
	static constexpr glm::tvec3<size_t> GetSize() noexcept { return s_ChunkSize; }

	Chunk(const glm::vec3& position, World& world, const TileMap& tileMap);
	virtual ~Chunk() noexcept override = default;

	void Create();
	void Render() const;

	inline Block GetBlockAt(const size_t x, const size_t y, const size_t z) const noexcept { return m_blocks[x * s_ChunkSize.y * s_ChunkSize.z + y * s_ChunkSize.z + z]; }
	inline void SetBlockAt(const size_t x, const size_t y, const size_t z, const Block block) noexcept { m_blocks[x * s_ChunkSize.y * s_ChunkSize.z + y * s_ChunkSize.z + z] = block; }

private:
	void GenerateBlocks();
	void GenerateMesh();
	void GenerateVertexObjects() const;

	std::vector<unsigned char> GetBlockFaces();
	void GenerateBufferData(const std::vector<unsigned char>& blockFaces);
	void AddElements(unsigned int& elementIndex, const unsigned int vertexIndex);

	std::string GetBlockTextureName(const Block block, const Direction direction) const;
};

#endif