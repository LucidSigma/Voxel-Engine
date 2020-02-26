#include "Chunk.h"

#include <glm/gtc/noise.hpp>

#include <array>
#include <iterator>

#include "World.h"

Chunk::Chunk(const glm::vec3& position, World& world, const TileMap& tileMap)
	: Drawable(), m_position(position), m_world(world), m_tileMap(&tileMap)
{
	GenerateBlocks();
}

void Chunk::Create()
{
	m_isReadyToRender = false;

	GenerateMesh();
	GenerateVertexObjects();

	m_vertices.clear();
	m_elements.clear();
	m_isReadyToRender = true;
}

void Chunk::Render() const
{
	if (m_isReadyToRender)
	{
		glActiveTexture(GL_TEXTURE0);
		m_tileMap->Bind();

		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_elementCount), GL_UNSIGNED_SHORT, nullptr);
		glBindVertexArray(0);
	}
}

void Chunk::GenerateBlocks()
{
	size_t currentIndex = 0;

	for (size_t x = 0; x < s_ChunkSize.x; ++x)
	{
		for (size_t y = 0; y < s_ChunkSize.y; ++y)
		{
			for (size_t z = 0; z < s_ChunkSize.z; ++z)
			{
				unsigned int terrainHeight = static_cast<unsigned int>(glm::ceil(glm::simplex(glm::vec2{ (x + m_position.x + s_seed) / 128.0f, (z + m_position.z + s_seed) / 64.0f }) * 4.0f));
				terrainHeight += static_cast<unsigned int>(glm::ceil(glm::simplex(glm::vec2{ (x + m_position.x - s_seed) / 64.0f, (z + m_position.z - s_seed) / 128.0f }) * 8.0f + 32.0f));

				const unsigned int currentLayer = static_cast<unsigned int>(y + m_position.y);

				if (currentLayer == 0)
				{
					m_blocks[currentIndex] = Block::Bedrock;
				}
				else if (currentLayer == terrainHeight)
				{
					m_blocks[currentIndex] = Block::Grass;
				}
				else if (glm::abs(terrainHeight - currentLayer) <= 3)
				{
					m_blocks[currentIndex] = Block::Dirt;
				}
				else if (currentLayer < terrainHeight)
				{
					m_blocks[currentIndex] = Block::Stone;
				}
				else if (currentLayer == terrainHeight + 1)
				{
					const unsigned int generateFoliageNumber = Random::GenerateInt(0, 2500);

					if (generateFoliageNumber == 0)
					{
						m_blocks[currentIndex] = Block::Pumpkin;
					}
					else if (generateFoliageNumber < 10)
					{
						m_world.AddTreeLocation(static_cast<int>(x + m_position.x), static_cast<int>(y + m_position.y), static_cast<int>(z + m_position.z));
					}
				}

				++currentIndex;
			}
		}
	}
}

void Chunk::GenerateMesh()
{
	const std::vector<unsigned char> blockFaces = GetBlockFaces();
	GenerateBufferData(blockFaces);
}

void Chunk::GenerateVertexObjects() const
{
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex3D), m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_elements.size() * sizeof(unsigned short), m_elements.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(Position);
	glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<const GLvoid*>(offsetof(Vertex3D, position)));

	glEnableVertexAttribArray(TextureCoordinates);
	glVertexAttribPointer(TextureCoordinates, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<const GLvoid*>(offsetof(Vertex3D, textureCoordinates)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

std::vector<unsigned char> Chunk::GetBlockFaces()
{
	m_faceCount = 0;
	std::vector<unsigned char> blockFaces(s_ChunkSize.x * s_ChunkSize.y * s_ChunkSize.z, 0);
	size_t currentIndex = 0;

	const std::array<std::optional<Chunk*>, 6u> neighbourChunks{
		m_world.GetChunkAt(static_cast<int>(m_position.x - s_ChunkSize.x), static_cast<int>(m_position.y), static_cast<int>(m_position.z)),
		m_world.GetChunkAt(static_cast<int>(m_position.x + s_ChunkSize.x), static_cast<int>(m_position.y), static_cast<int>(m_position.z)),
		m_world.GetChunkAt(static_cast<int>(m_position.x), static_cast<int>(m_position.y - s_ChunkSize.y), static_cast<int>(m_position.z)),
		m_world.GetChunkAt(static_cast<int>(m_position.x), static_cast<int>(m_position.y + s_ChunkSize.y), static_cast<int>(m_position.z)),
		m_world.GetChunkAt(static_cast<int>(m_position.x), static_cast<int>(m_position.y), static_cast<int>(m_position.z - s_ChunkSize.z)),
		m_world.GetChunkAt(static_cast<int>(m_position.x), static_cast<int>(m_position.y), static_cast<int>(m_position.z + s_ChunkSize.z))
	};

	for (size_t x = 0; x < s_ChunkSize.x; ++x)
	{
		for (size_t y = 0; y < s_ChunkSize.y; ++y)
		{
			for (size_t z = 0; z < s_ChunkSize.z; ++z)
			{
				if (m_blocks[currentIndex] == Block::Air)
				{
					++currentIndex;

					continue;
				}

				if ((x == 0 && (!neighbourChunks[0].has_value() || (neighbourChunks[0].has_value() && IsTransparent(neighbourChunks[0].value()->GetBlockAt(s_ChunkSize.x - 1, y, z)))) || (x != 0 && IsTransparent(GetBlockAt(x - 1, y, z)))))
				{
					blockFaces[currentIndex] |= Direction::West;
					++m_faceCount;
				}

				if ((x == s_ChunkSize.x - 1 && (!neighbourChunks[1].has_value() || (neighbourChunks[1].has_value() && IsTransparent(neighbourChunks[1].value()->GetBlockAt(0, y, z)))) || (x != s_ChunkSize.x - 1 && IsTransparent(GetBlockAt(x + 1, y, z)))))
				{
					blockFaces[currentIndex] |= Direction::East;
					++m_faceCount;
				}

				if ((y == 0 && (!neighbourChunks[2].has_value() || (neighbourChunks[2].has_value() && IsTransparent(neighbourChunks[2].value()->GetBlockAt(x, s_ChunkSize.y - 1, z))))) || (y != 0 && IsTransparent(GetBlockAt(x, y - 1, z))))
				{
					blockFaces[currentIndex] |= Direction::Nadir;
					++m_faceCount;
				}

				if ((y == s_ChunkSize.y - 1 && (!neighbourChunks[3].has_value() || (neighbourChunks[3].has_value() && IsTransparent(neighbourChunks[3].value()->GetBlockAt(x, 0, z))))) || (y != s_ChunkSize.y - 1 && IsTransparent(GetBlockAt(x, y + 1, z))))
				{
					blockFaces[currentIndex] |= Direction::Zenith;
					++m_faceCount;
				}

				if ((z == 0 && (!neighbourChunks[4].has_value() || (neighbourChunks[4].has_value() && IsTransparent(neighbourChunks[4].value()->GetBlockAt(x, y, s_ChunkSize.z - 1)))) || (z != 0 && IsTransparent(GetBlockAt(x, y, z - 1)))))
				{
					blockFaces[currentIndex] |= Direction::North;
					++m_faceCount;
				}

				if ((z == s_ChunkSize.z - 1 && (!neighbourChunks[5].has_value() || (neighbourChunks[5].has_value() && IsTransparent(neighbourChunks[5].value()->GetBlockAt(x, y, 0)))) || (z != s_ChunkSize.z - 1 && IsTransparent(GetBlockAt(x, y, z + 1)))))
				{
					blockFaces[currentIndex] |= Direction::South;
					++m_faceCount;
				}

				++currentIndex;
			}
		}
	}

	return blockFaces;
}

void Chunk::GenerateBufferData(const std::vector<unsigned char>& blockFaces)
{
	m_vertices.resize(m_faceCount * 4);
	m_elements.resize(m_faceCount * 6);

	unsigned int vertexIndex = 0;
	unsigned int elementIndex = 0;
	size_t currentIndex = 0;

	for (size_t x = 0; x < s_ChunkSize.x; ++x)
	{
		for (size_t y = 0; y < s_ChunkSize.y; ++y)
		{
			for (size_t z = 0; z < s_ChunkSize.z; ++z)
			{
				if (blockFaces[currentIndex] == 0)
				{
					++currentIndex;

					continue;
				}

				if (blockFaces[currentIndex] & Direction::West)
				{
					const float tileIndex = static_cast<float>(m_tileMap->GetTileIndex(GetBlockTextureName(GetBlockAt(x, y, z), Direction::West)));

					m_vertices[static_cast<size_t>(vertexIndex) + 0] = Vertex3D{ glm::vec3{ x + m_position.x, y + m_position.y, z + m_position.z }, glm::vec3{ 0.0f, 0.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 1] = Vertex3D{ glm::vec3{ x + m_position.x, y + m_position.y + 1, z + m_position.z }, glm::vec3{ 0.0f, 1.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 2] = Vertex3D{ glm::vec3{ x + m_position.x, y + m_position.y, z + m_position.z + 1 }, glm::vec3{ 1.0f, 0.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 3] = Vertex3D{ glm::vec3{ x + m_position.x, y + m_position.y + 1, z + m_position.z + 1 }, glm::vec3{ 1.0f, 1.0f, tileIndex } };

					AddElements(elementIndex, vertexIndex);
					vertexIndex += 4;
				}

				if (blockFaces[currentIndex] & Direction::East)
				{
					const float tileIndex = static_cast<float>(m_tileMap->GetTileIndex(GetBlockTextureName(GetBlockAt(x, y, z), Direction::East)));

					m_vertices[static_cast<size_t>(vertexIndex) + 0] = Vertex3D{ glm::vec3{ x + m_position.x + 1, y + m_position.y, z + m_position.z }, glm::vec3{ 1.0f, 0.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 1] = Vertex3D{ glm::vec3{ x + m_position.x + 1, y + m_position.y, z + m_position.z + 1 }, glm::vec3{ 0.0f, 0.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 2] = Vertex3D{ glm::vec3{ x + m_position.x + 1, y + m_position.y + 1, z + m_position.z }, glm::vec3{ 1.0f, 1.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 3] = Vertex3D{ glm::vec3{ x + m_position.x + 1, y + m_position.y + 1, z + m_position.z + 1 }, glm::vec3{ 0.0f, 1.0f, tileIndex } };

					AddElements(elementIndex, vertexIndex);
					vertexIndex += 4;
				}

				if (blockFaces[currentIndex] & Direction::Nadir)
				{
					const float tileIndex = static_cast<float>(m_tileMap->GetTileIndex(GetBlockTextureName(GetBlockAt(x, y, z), Direction::Nadir)));

					m_vertices[static_cast<size_t>(vertexIndex) + 0] = Vertex3D{ glm::vec3{ x + m_position.x, y + m_position.y, z + m_position.z }, glm::vec3{ 0.0f, 0.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 1] = Vertex3D{ glm::vec3{ x + m_position.x, y + m_position.y, z + m_position.z + 1 }, glm::vec3{ 0.0f, 1.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 2] = Vertex3D{ glm::vec3{ x + m_position.x + 1, y + m_position.y, z + m_position.z }, glm::vec3{ 1.0f, 0.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 3] = Vertex3D{ glm::vec3{ x + m_position.x + 1, y + m_position.y, z + m_position.z + 1 }, glm::vec3{ 1.0f, 1.0f, tileIndex } };

					AddElements(elementIndex, vertexIndex);
					vertexIndex += 4;
				}

				if (blockFaces[currentIndex] & Direction::Zenith)
				{
					const float tileIndex = static_cast<float>(m_tileMap->GetTileIndex(GetBlockTextureName(GetBlockAt(x, y, z), Direction::Zenith)));

					m_vertices[static_cast<size_t>(vertexIndex) + 0] = Vertex3D{ glm::vec3{ x + m_position.x, y + m_position.y + 1, z + m_position.z }, glm::vec3{ 0.0f, 0.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 1] = Vertex3D{ glm::vec3{ x + m_position.x + 1, y + m_position.y + 1, z + m_position.z }, glm::vec3{ 1.0f, 0.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 2] = Vertex3D{ glm::vec3{ x + m_position.x, y + m_position.y + 1, z + m_position.z + 1 }, glm::vec3{ 0.0f, 1.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 3] = Vertex3D{ glm::vec3{ x + m_position.x + 1, y + m_position.y + 1, z + m_position.z + 1 }, glm::vec3{ 1.0f, 1.0f, tileIndex } };

					AddElements(elementIndex, vertexIndex);
					vertexIndex += 4;
				}

				if (blockFaces[currentIndex] & Direction::North)
				{
					const float tileIndex = static_cast<float>(m_tileMap->GetTileIndex(GetBlockTextureName(GetBlockAt(x, y, z), Direction::North)));
				
					m_vertices[static_cast<size_t>(vertexIndex) + 0] = Vertex3D{ glm::vec3{ x + m_position.x, y + m_position.y, z + m_position.z }, glm::vec3{ 1.0f, 0.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 1] = Vertex3D{ glm::vec3{ x + m_position.x + 1, y + m_position.y, z + m_position.z }, glm::vec3{ 0.0f, 0.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 2] = Vertex3D{ glm::vec3{ x + m_position.x, y + m_position.y + 1, z + m_position.z }, glm::vec3{ 1.0f, 1.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 3] = Vertex3D{ glm::vec3{ x + m_position.x + 1, y + m_position.y + 1, z + m_position.z }, glm::vec3{ 0.0f, 1.0f, tileIndex } };
				
					AddElements(elementIndex, vertexIndex);
					vertexIndex += 4;
				}

				if (blockFaces[currentIndex] & Direction::South)
				{
					const float tileIndex = static_cast<float>(m_tileMap->GetTileIndex(GetBlockTextureName(GetBlockAt(x, y, z), Direction::South)));
					
					m_vertices[static_cast<size_t>(vertexIndex) + 0] = Vertex3D{ glm::vec3{ x + m_position.x, y + m_position.y, z + m_position.z + 1 }, glm::vec3{ 0.0f, 0.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 1] = Vertex3D{ glm::vec3{ x + m_position.x, y + m_position.y + 1, z + m_position.z + 1 }, glm::vec3{ 0.0f, 1.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 2] = Vertex3D{ glm::vec3{ x + m_position.x + 1, y + m_position.y, z + m_position.z + 1 }, glm::vec3{ 1.0f, 0.0f, tileIndex } };
					m_vertices[static_cast<size_t>(vertexIndex) + 3] = Vertex3D{ glm::vec3{ x + m_position.x + 1, y + m_position.y + 1, z + m_position.z + 1 }, glm::vec3{ 1.0f, 1.0f, tileIndex } };

					AddElements(elementIndex, vertexIndex);
					vertexIndex += 4;
				}

				++currentIndex;
			}
		}
	}

	m_elementCount = m_elements.size();
}

void Chunk::AddElements(unsigned int& elementIndex, const unsigned int vertexIndex)
{
	m_elements[elementIndex++] = vertexIndex + 0;
	m_elements[elementIndex++] = vertexIndex + 2;
	m_elements[elementIndex++] = vertexIndex + 1;

	m_elements[elementIndex++] = vertexIndex + 2;
	m_elements[elementIndex++] = vertexIndex + 3;
	m_elements[elementIndex++] = vertexIndex + 1;
}

std::string Chunk::GetBlockTextureName(const Block block, const Direction direction) const
{
	switch (block)
	{
	case Block::Stone:
		return "stone";

	case Block::Dirt:
		return "dirt";

	case Block::Grass:
		switch (direction)
		{
		case Direction::Zenith:
			return "grass_top";

		case Direction::Nadir:
			return "dirt";

		default:
			return "grass_side";
		}

	case Block::Bedrock:
		return "bedrock";

	case Block::Log:
		switch (direction)
		{
		case Direction::Zenith:
		case Direction::Nadir:
			return "log_top";

		default:
			return "log_side";
		}

	case Block::Leaves:
		return "leaves";

	case Block::Planks:
		return "planks";

	case Block::Cobblestone:
		return "cobblestone";

	case Block::Glass:
		return "glass";

	case Block::Pumpkin:
		switch (direction)
		{
		case Direction::Zenith:
			return "pumpkin_top";

		case Direction::Nadir:
			return "pumpkin_bottom";

		default:
			return "pumpkin_side";
		}
	
	default:
		return "";
	}
}