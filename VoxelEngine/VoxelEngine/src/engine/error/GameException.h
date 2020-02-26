#pragma once
#ifndef GAME_EXCEPTION_H
#define GAME_EXCEPTION_H

#include <exception>
#include <string>
#include <variant>

class GameException
	: public std::exception
{
public:
	enum class Initialise
	{
		SDL,
		GLAD,
		OpenGL,
		OpenAL,
		FreeType
	};

	enum class FileNotFound
	{
		Icon,
		Texture,
		Audio,
		Shader,
		Font
	};

	enum class Shader
	{
		Compile,
		Link,
		Validate
	};

	enum class Config
	{
		Missing,
		Corrupt
	};

private:
	template <typename... Ts>
	struct Overload
		: public Ts...
	{
		using Ts::operator ()...;
	};

	template <typename... Ts>
	Overload(Ts...) -> Overload<Ts...>;

	std::string m_message;
	std::variant<Initialise, FileNotFound, Shader, Config> m_type;

public:
	GameException(const std::variant<Initialise, FileNotFound, Shader, Config>& errorType, const std::string& message);
	~GameException() noexcept = default;

	void Log() const;
	void DisplayMessageBox() const;

private:
	std::string GetErrorMessage() const;
};

#endif