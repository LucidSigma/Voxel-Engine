#include "TextureArray.h"

#include <stb_image/stb_image.h>

#include <initializer_list>
#include <sstream>
#include <utility>

#include "../error/GameException.h"

TextureArray::TextureArray(const std::string& filepath, const glm::uvec2& subTextureSize)
	: m_subTextureSize(subTextureSize)
{
	glGenTextures(1, &m_id);

	Generate(filepath);
}

TextureArray::TextureArray(TextureArray&& other) noexcept
	: m_id(ResetTextureArray(other.m_id)), m_size(other.m_size)
{
	other.m_id = 0;
	other.m_size = glm::uvec2{ 0, 0 };
}

TextureArray& TextureArray::operator=(TextureArray&& other) noexcept
{
	if (m_id != 0)
	{
		Destroy();
	}

	m_id = std::exchange(other.m_id, 0);
	m_size = std::exchange(other.m_size, glm::uvec2{ 0, 0 });

	return *this;
}

TextureArray::~TextureArray() noexcept
{
	Destroy();
}

void TextureArray::Bind() const
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
}

void TextureArray::Generate(const std::string& filepath)
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

void TextureArray::SetupParameters(const GLint internalFormat, const GLenum format, const std::byte* data)
{
	Bind();
	const unsigned int imageRows = m_size.y / m_subTextureSize.y;
	const unsigned int imageColumns = m_size.x / m_subTextureSize.x;

	glPixelStorei(GL_UNPACK_ROW_LENGTH, m_size.x);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internalFormat, m_subTextureSize.x, m_subTextureSize.y, imageRows * imageColumns);
	
	for (size_t y = 0; y < imageRows; ++y)
	{
		for (size_t x = 0; x < imageColumns; ++x)
		{
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, static_cast<GLint>(imageColumns * y + x), m_subTextureSize.x, m_subTextureSize.y, 1, format, GL_UNSIGNED_BYTE, data + (y * m_subTextureSize.y * m_size.x + x * m_subTextureSize.x) * imageRows * 4);
		}
	}

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	const std::initializer_list<std::pair<GLenum, GLint>> textureParameters{
		{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR },
		{ GL_TEXTURE_MAG_FILTER, GL_NEAREST }
	};

	for (const auto [parameter, value] : textureParameters)
	{
		glTexParameteri(GL_TEXTURE_2D_ARRAY, parameter, value);
	}

	float maxAnisotropy = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropy);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
}

void TextureArray::Destroy() noexcept
{
	glDeleteTextures(1, &m_id);
	m_id = 0;
}

GLuint TextureArray::ResetTextureArray(const GLuint newTextureArrayID) noexcept
{
	if (m_id != 0)
	{
		Destroy();
	}

	return newTextureArrayID;
}