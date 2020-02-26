#include "FontManager.h"

void FontManager::AddFont(const std::string& name, const std::string& filepath, const unsigned int size)
{
	Add(name, filepath, size);
}