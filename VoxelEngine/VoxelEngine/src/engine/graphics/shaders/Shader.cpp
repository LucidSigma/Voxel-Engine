#include "Shader.h"

#include <fstream>
#include <sstream>
#include <utility>
#include <vector>

#include "../../error/GameException.h"
#include "../../utility/Logger.h"

Shader::Shader(const Type type, const std::string& filepath, const bool isSpirV)
	: m_id(glCreateShader(static_cast<GLenum>(type)))
{
	const std::string& shaderCode = ReadShaderFile(filepath, isSpirV);

	if (isSpirV)
	{
		CompileSpirVShader(shaderCode);
	}
	else
	{
		const char* shaderCodeData = shaderCode.c_str();
		CompileShader(shaderCodeData);
	}

	if (!CheckCompilationErrors())
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "Failed to compile shader: " << filepath << ".";

		throw GameException(GameException::Shader::Compile, errorMessageStream.str());
	}
}

Shader::Shader(Shader&& other) noexcept
	: m_id(ResetShader(other.m_id))
{
	other.m_id = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	if (m_id != 0)
	{
		Destroy();
	}

	m_id = std::exchange(other.m_id, 0);

	return *this;
}

Shader::~Shader() noexcept
{
	Destroy();
}

std::string Shader::ReadShaderFile(const std::string& filepath, const bool isSpirV)
{
	const int fileFlags = isSpirV ? std::ios_base::in | std::ios_base::binary : std::ios_base::in;
	std::ifstream shaderFile(filepath, fileFlags);

	if (!shaderFile.is_open())
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "Failed to open shader file: " << filepath << ".";

		throw GameException(GameException::FileNotFound::Shader, errorMessageStream.str());
	}

	std::ostringstream fileReader;
	fileReader << shaderFile.rdbuf();
	shaderFile.close();

	return fileReader.str();
}

void Shader::CompileShader(const char* shaderCodeData)
{
	glShaderSource(m_id, 1, &shaderCodeData, nullptr);
	glCompileShader(m_id);
}

void Shader::CompileSpirVShader(const std::string& shaderCode)
{
	glShaderBinary(1, &m_id, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderCode.data(), static_cast<GLsizei>(shaderCode.length()));
	glSpecializeShader(m_id, "main", 0, nullptr, nullptr);
}

bool Shader::CheckCompilationErrors() const
{
	GLint isSuccessful = 0;
	glGetShaderiv(m_id, GL_COMPILE_STATUS, &isSuccessful);

	if (isSuccessful == GL_FALSE)
	{
		GLint infoLogLength = 0;
		glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);

		std::vector<char> infoLog(infoLogLength);
		glGetShaderInfoLog(m_id, infoLogLength, nullptr, infoLog.data());

		std::ostringstream warningMessageStream;
		warningMessageStream << "SHADER COMPILATION LOG:\n" << infoLog.data();

		Logger::Log(warningMessageStream.str());

		return false;
	}

	return true;
}

void Shader::Destroy() noexcept
{
	glDeleteShader(m_id);
	m_id = 0;
}

GLuint Shader::ResetShader(const GLuint newShaderID) noexcept
{
	if (m_id != 0)
	{
		Destroy();
	}

	return newShaderID;
}