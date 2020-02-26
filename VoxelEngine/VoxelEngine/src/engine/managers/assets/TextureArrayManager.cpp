#include "TextureArrayManager.h"

void TextureArrayManager::AddTextureArray(const std::string& name, const std::string& filepath, const glm::uvec2& subTextureSize)
{
	Add(name, filepath, subTextureSize);
}