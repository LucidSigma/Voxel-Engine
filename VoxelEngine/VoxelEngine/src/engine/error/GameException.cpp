#include "GameException.h"

#include <magic_enum/magic_enum.hpp>
#include <SDL2/SDL.h>

#include "../utility/Logger.h"
#include "../utility/StringUtility.h"

GameException::GameException(const std::variant<Initialise, FileNotFound, Shader, Config>& errorType, const std::string& message)
	: m_message(message), m_type(errorType)
{ }

void GameException::Log() const
{
	Logger::Log(ToUpperCase(GetErrorMessage()) + ": " + m_message);
}

void GameException::DisplayMessageBox() const
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, GetErrorMessage().c_str(), m_message.c_str(), nullptr);
}

std::string GameException::GetErrorMessage() const
{
	std::string errorMessageType;

	std::visit(Overload{
		[&errorMessageType](const Initialise& initialiseError) { errorMessageType = std::string(magic_enum::enum_name(initialiseError)) + " Initialise Error"; },
		[&errorMessageType](const FileNotFound& fileNotFoundError) { errorMessageType = std::string(magic_enum::enum_name(fileNotFoundError)) + " File Error"; },
		[&errorMessageType](const Shader& shaderError) { errorMessageType = "Shader " + std::string(magic_enum::enum_name(shaderError)) + " Error"; },
		[&errorMessageType](const Config& config) { errorMessageType = "Config " + std::string(magic_enum::enum_name(config)) + " Error"; }
	}, m_type);

	return errorMessageType;
}