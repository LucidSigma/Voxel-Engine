#include "Texture.h"

#include <stb_image/stb_image.h>

#include <initializer_list>
#include <sstream>
#include <utility>

#include "../error/GameException.h"

Texture::Texture(const std::string& filepath)
{
	glGenTextures(1, &m_id);

	Generate(filepath);
}

Texture::Texture(const glm::vec2& size)
{
	glGenTextures(1, &m_id);

	m_size = size;

	SetupParameters(GL_RGB8, GL_RGB, nullptr);
}

Texture::Texture(Texture&& other) noexcept
	: m_id(ResetTexture(other.m_id)), m_size(other.m_size)
{
	other.m_id = 0;
	other.m_size = glm::uvec2{ 0, 0 };
}

Texture& Texture::operator=(Texture&& other) noexcept
{
	if (m_id != 0)
	{
		Destroy();
	}

	m_id = std::exchange(other.m_id, 0);
	m_size = std::exchange(other.m_size, glm::uvec2{ 0, 0 });

	return *this;
}

Texture::~Texture() noexcept
{
	Destroy();
}

void Texture::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::Generate(const std::string& filepath)
{
	int width = 0;
	int height = 0;
	int componentCount = 0;

	std::byte* data = reinterpret_cast<std::byte*>(stbi_load(filepath.c_str(), &width, &height, &componentCount, STBI_default));

	if (data == nullptr)
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "Failed to load texture file: " << filepath << ".";

		throw GameException(GameException::FileNotFound::Texture, errorMessageStream.str());
	}

	m_size.x = width;
	m_size.y = height;
	const auto [internalFormat, format] = s_componentMap.at(componentCount);

	SetupParameters(internalFormat, format, data);

	stbi_image_free(data);
}

void Texture::SetupParameters(const GLint internalFormat, const GLenum format, const std::byte* data)
{
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_size.x, m_size.y, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	const std::initializer_list<std::pair<GLenum, GLint>> textureParameters{
		{ GL_TEXTURE_WRAP_S, GL_REPEAT },
		{ GL_TEXTURE_WRAP_T, GL_REPEAT },
		{ GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST },
		{ GL_TEXTURE_MAG_FILTER, GL_NEAREST }
	};
	
	for (const auto [parameter, value] : textureParameters)
	{
		glTexParameteri(GL_TEXTURE_2D, parameter, value);
	}

	float maxAnisotropy = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropy);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Destroy() noexcept
{
	glDeleteTextures(1, &m_id);
	m_id = 0;
}

GLuint Texture::ResetTexture(const GLuint newTextureID) noexcept
{
	if (m_id != 0)
	{
		Destroy();
	}

	return newTextureID;
}
