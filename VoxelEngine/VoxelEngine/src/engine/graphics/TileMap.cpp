#include "TileMap.h"

TileMap::TileMap(const TextureArray& textureAtlas, const glm::uvec2& tileSize, const std::vector<std::string_view>& tileNames)
	: m_textureAtlas(&textureAtlas), m_tileColumns(m_textureAtlas->GetSize().x / m_textureAtlas->GetSubTextureSize().x), m_tileRows(m_textureAtlas->GetSize().y / m_textureAtlas->GetSubTextureSize().y)
{
	for (size_t i = 0; i < tileNames.size(); ++i)
	{
		m_tileNames.insert({ tileNames[i], i });
	}
}

TileMap::TileMap(TileMap&& other) noexcept
	: m_textureAtlas(other.m_textureAtlas), m_tileColumns(other.m_tileColumns), m_tileRows(other.m_tileRows), m_tileNames(std::move(other.m_tileNames))
{
	other.m_textureAtlas = nullptr;

	other.m_tileColumns = 0;
	other.m_tileRows = 0;

	other.m_tileNames.clear();
}

TileMap& TileMap::operator =(TileMap&& other) noexcept
{
	m_textureAtlas = std::exchange(other.m_textureAtlas, nullptr);

	m_tileColumns = std::exchange(other.m_tileColumns, 0);
	m_tileRows = std::exchange(other.m_tileRows, 0);

	m_tileNames = std::move(other.m_tileNames);
	other.m_tileNames.clear();

	return *this;
}

void TileMap::Bind() const
{
	m_textureAtlas->Bind();
}

size_t TileMap::GetTileIndex(const std::string_view& tileName) const
{
	return m_tileNames.at(tileName);
}