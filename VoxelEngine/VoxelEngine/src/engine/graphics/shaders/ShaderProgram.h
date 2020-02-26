#pragma once
#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "../../interfaces/INoncopyable.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstddef>
#include <initializer_list>
#include <memory>
#include <unordered_map>
#include <string>

#include "Shader.h"

class ShaderProgram
	: private INoncopyable
{
private:
	GLuint m_id = 0;

	mutable std::unordered_map<std::string, int> m_uniformCache;

public:
	ShaderProgram(const std::initializer_list<Shader>& shaders);

	ShaderProgram(ShaderProgram&& other) noexcept;
	ShaderProgram& operator =(ShaderProgram&& other) noexcept;

	~ShaderProgram() noexcept;

	void Use() const;

	template <typename T>
	void SetUniform(const std::string& name, const T& value) const
	{
		static_assert(false, "Invalid uniform type.");
	}

	template <>
	void SetUniform<bool>(const std::string& name, const bool& value) const
	{
		const GLint location = GetUniformLocation(name);
		glUniform1i(location, static_cast<int>(value));
	}

	template <>
	void SetUniform<int>(const std::string& name, const int& value) const
	{
		const GLint location = GetUniformLocation(name);
		glUniform1i(location, value);
	}

	template <>
	void SetUniform<float>(const std::string& name, const float& value) const
	{
		const GLint location = GetUniformLocation(name);
		glUniform1f(location, value);
	}

	template <>
	void SetUniform<double>(const std::string& name, const double& value) const
	{
		const GLint location = GetUniformLocation(name);
		glUniform1f(location, static_cast<float>(value));
	}

	template <>
	void SetUniform<glm::vec2>(const std::string& name, const glm::vec2& value) const
	{
		const GLint location = GetUniformLocation(name);
		glUniform2fv(location, 1, glm::value_ptr(value));
	}

	template <>
	void SetUniform<glm::vec3>(const std::string& name, const glm::vec3& value) const
	{
		const GLint location = GetUniformLocation(name);
		glUniform3fv(location, 1, glm::value_ptr(value));
	}

	template <>
	void SetUniform<glm::vec4>(const std::string& name, const glm::vec4& value) const
	{
		const GLint location = GetUniformLocation(name);
		glUniform4fv(location, 1, glm::value_ptr(value));
	}

	template <>
	void SetUniform<glm::mat4>(const std::string& name, const glm::mat4& value) const
	{
		const GLint location = GetUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	template <typename T>
	void SetUniformVector(const std::string& name, const T* values, const size_t count) const
	{
		static_assert(false, "Invalid uniform type.");
	}

	template <>
	void SetUniformVector<float>(const std::string& name, const float* values, const size_t count) const
	{
		const GLint location = GetUniformLocation(name);
		glUniform1fv(location, static_cast<GLsizei>(count), values);
	}

	template <>
	void SetUniformVector<glm::vec2>(const std::string& name, const glm::vec2* values, const size_t count) const
	{
		const GLint location = GetUniformLocation(name);
		glUniform2fv(location, static_cast<GLsizei>(count), reinterpret_cast<const float*>(values));
	}

	void SetTextureUniform(const std::string& name, const int index) const;

	inline GLuint GetID() const noexcept { return m_id; }

private:
	bool CheckLinkingStatus() const;
	bool CheckValidationStatus() const;
	bool CheckStatus(const GLenum status) const;

	void Destroy() noexcept;
	GLuint ResetProgram(const GLuint newProgramID) noexcept;

	int GetUniformLocation(const std::string& uniformName) const;
};

#endif