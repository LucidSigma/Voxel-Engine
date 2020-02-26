#pragma once
#ifndef TILE_MAP_H
#define TILE_MAP_H

#include "../interfaces/INoncopyable.h"

#include <glm/glm.hpp>

#include <cstddef>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "TextureArray.h"

class TileMap
	: private INoncopyable
{
private:
	const TextureArray* m_textureAtlas = nullptr;

	unsigned int m_tileColumns = 0;
	unsigned int m_tileRows = 0;

	std::unordered_map<std::string_view, size_t> m_tileNames;

public:
	TileMap(const TextureArray& textureAtlas, const glm::uvec2& tileSize, const std::vector<std::string_view>& tileNames);

	TileMap(TileMap&& other) noexcept;
	TileMap& operator =(TileMap&& other) noexcept;

	~TileMap() noexcept = default;

	void Bind() const;

	size_t GetTileIndex(const std::string_view& tileName) const;
};

#endif