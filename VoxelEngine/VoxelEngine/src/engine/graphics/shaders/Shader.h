#pragma once
#ifndef SHADER_H
#define SHADER_H

#include "../../interfaces/INoncopyable.h"

#include <glad/glad.h>

#include <string>

class Shader
	: private INoncopyable
{
public:
	enum class Type
		: GLenum
	{
		Vertex = GL_VERTEX_SHADER,
		TessellationControl = GL_TESS_CONTROL_SHADER,
		TessellationEvaluation = GL_TESS_EVALUATION_SHADER,
		Geometry = GL_GEOMETRY_SHADER,
		Fragment = GL_FRAGMENT_SHADER,
		Compute = GL_COMPUTE_SHADER
	};

private:
	GLuint m_id = 0;

public:
	Shader(const Type type, const std::string& filepath, const bool isSpirV = false);

	Shader(Shader&& other) noexcept;
	Shader& operator =(Shader&& other) noexcept;

	~Shader() noexcept;

	inline GLuint GetID() const noexcept { return m_id; }

private:
	std::string ReadShaderFile(const std::string& filepath, const bool isSpirV);

	void CompileShader(const char* shaderCodeData);
	void CompileSpirVShader(const std::string& shaderCode);

	bool CheckCompilationErrors() const;

	void Destroy() noexcept;
	GLuint ResetShader(const GLuint newShaderID) noexcept;
};

#endif