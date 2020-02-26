#include "ShaderManager.h"

void ShaderManager::AddShader(const std::string& name, const std::initializer_list<Shader>& shaders)
{
	Add(name, shaders);
}