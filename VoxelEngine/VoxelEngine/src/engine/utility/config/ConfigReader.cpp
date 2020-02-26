#include "ConfigReader.h"

#include <sstream>

#include "../../error/GameException.h"

const std::unordered_map<std::string, ConfigReader::ParserFunction> ConfigReader::s_configMappings
{
	{ "title", &ParseTitle },
	{ "window-width", &ParseWindowWidth },
	{ "window-height", &ParseWindowHeight },
	{ "gl-version-major", &ParseGLVersionMajor },
	{ "gl-version-minor", &ParseGLVersionMinor },
	{ "enable-vsync", &ParseEnableVsync },
	{ "multisample-count", &ParseMultisampleCount }
};

ConfigReader::ConfigReader()
{
	const auto result = m_configFile.LoadFile(s_configFilePath.c_str());

	if (!CheckXMLResult(result))
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "The config XML file is corrupted or missing.";

		throw GameException(GameException::Config::Missing, errorMessageStream.str());
	}
}

Config ConfigReader::ReadConfigFile()
{
	tinyxml2::XMLNode* const configRoot = m_configFile.FirstChildElement(s_rootNodeName.c_str());

	if (configRoot == nullptr)
	{
		ThrowCorruptFileException();
	}

	Config config;

	for (const auto& [nodeName, parseFunction] : s_configMappings)
	{
		tinyxml2::XMLElement* configElement = configRoot->FirstChildElement(nodeName.c_str());

		if (configElement == nullptr)
		{
			ThrowCorruptFileException();
		}

		if (const bool parseResult = (this->*parseFunction)(config, configElement);
			!parseResult)
		{
			ThrowCorruptFileException();
		}
	}

	return config;
}

bool ConfigReader::CheckXMLResult(const tinyxml2::XMLError result) noexcept
{
	return result == tinyxml2::XML_SUCCESS;
}

void ConfigReader::ThrowCorruptFileException()
{
	std::ostringstream errorMessageStream;
	errorMessageStream << "The config XML file is corrupted.";

	throw GameException(GameException::Config::Corrupt, errorMessageStream.str());
}

bool ConfigReader::ParseTitle(Config& config, const tinyxml2::XMLElement* const configElement)
{
	config.title = configElement->GetText();

	return true;
}

bool ConfigReader::ParseWindowWidth(Config& config, const tinyxml2::XMLElement* const configElement)
{
	const auto result = configElement->QueryUnsignedText(&config.windowWidth);

	return CheckXMLResult(result);
}

bool ConfigReader::ParseWindowHeight(Config& config, const tinyxml2::XMLElement* const configElement)
{
	const auto result = configElement->QueryUnsignedText(&config.windowHeight);

	return CheckXMLResult(result);
}

bool ConfigReader::ParseGLVersionMajor(Config& config, const tinyxml2::XMLElement* const configElement)
{
	const auto result = configElement->QueryUnsignedText(&config.glVersionMajor);

	return CheckXMLResult(result);
}

bool ConfigReader::ParseGLVersionMinor(Config& config, const tinyxml2::XMLElement* const configElement)
{
	const auto result = configElement->QueryUnsignedText(&config.glVersionMinor);

	return CheckXMLResult(result);
}

bool ConfigReader::ParseEnableVsync(Config& config, const tinyxml2::XMLElement* const configElement)
{
	const auto result = configElement->QueryBoolText(&config.enableVsync);

	return CheckXMLResult(result);
}

bool ConfigReader::ParseMultisampleCount(Config& config, const tinyxml2::XMLElement* const configElement)
{
	const auto result = configElement->QueryUnsignedText(&config.multisampleCount);

	return CheckXMLResult(result);
}