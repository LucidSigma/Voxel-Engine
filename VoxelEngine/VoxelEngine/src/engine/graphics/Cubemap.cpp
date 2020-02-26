#include "Cubemap.h"

#include <stb_image/stb_image.h>

#include <array>
#include <cstddef>
#include <initializer_list>
#include <sstream>
#include <utility>

#include "../error/GameException.h"

Cubemap::Cubemap(const std::string& filepath)
{
	glGenTextures(1, &m_id);

	Generate(filepath);
	SetupParameters(GL_RGB8, GL_RGB);
}

Cubemap::Cubemap(Cubemap&& other) noexcept
	: m_id(ResetCubemap(other.m_id)), m_size(other.m_size)
{
	other.m_id = 0;
	other.m_size = glm::uvec2{ 0, 0 };
}

Cubemap& Cubemap::operator=(Cubemap&& other) noexcept
{
	if (m_id != 0)
	{
		Destroy();
	}

	m_id = std::exchange(other.m_id, 0);
	m_size = std::exchange(other.m_size, glm::uvec2{ 0, 0 });

	return *this;
}

Cubemap::~Cubemap() noexcept
{
	Destroy();
}

void Cubemap::Bind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}

void Cubemap::Generate(const std::string& filepath)
{
	Bind();

	static const std::array<std::string, 6u> faceNames{
		"right", "left",
		"bottom", "top",
		"back", "front"
	};

	int width = 0;
	int height = 0;
	int componentCount = 0;

	for (size_t i = 0; i < 6; ++i)
	{
		const std::string faceFilepath = filepath + "/" + faceNames[i] + ".png";
		std::byte* data = reinterpret_cast<std::byte*>(stbi_load(faceFilepath.c_str(), &width, &height, &componentCount, STBI_rgb));

		if (data == nullptr)
		{
			std::ostringstream errorMessageStream;
			errorMessageStream << "Failed to load cubemap texture file: " << filepath << ".";

			throw GameException(GameException::FileNotFound::Texture, errorMessageStream.str());
		}

		m_size.x = width;
		m_size.y = height;
		const auto [internalFormat, format] = s_componentMap.at(componentCount);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i), 0, internalFormat, m_size.x, m_size.y, 0, format, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
}

void Cubemap::SetupParameters(const GLint internalFormat, const GLenum format)
{
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	const std::initializer_list<std::pair<GLenum, GLint>> textureParameters{
		{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR },
		{ GL_TEXTURE_MAG_FILTER, GL_LINEAR }
	};

	for (const auto [parameter, value] : textureParameters)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, parameter, value);
	}

	float maxAnisotropy = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropy);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Cubemap::Destroy() noexcept
{
	glDeleteTextures(1, &m_id);
	m_id = 0;
}

GLuint Cubemap::ResetCubemap(const GLuint newCubemapID) noexcept
{
	if (m_id != 0)
	{
		Destroy();
	}

	return newCubemapID;
}