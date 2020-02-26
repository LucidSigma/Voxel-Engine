#pragma once
#ifndef FONT_H
#define FONT_H

#include "../../interfaces/INoncopyable.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

class Font
	: private INoncopyable
{
public:
	struct Glyph
	{
		GLuint textureID;

		glm::ivec2 size;
		glm::ivec2 bearing;

		unsigned int advance;
	};

private:
	std::unordered_map<char, Glyph> m_glyphs;

public:
	Font(const std::string& fontFilepath, const unsigned int fontSize);

	Font(Font&& other) noexcept;
	Font& operator =(Font&& other) noexcept;

	~Font() noexcept = default;

	inline const std::unordered_map<char, Glyph>& GetGlyphs() const noexcept { return m_glyphs; }

private:
	Glyph CreateGlyph(const FT_Face& face) const;
};

#endif