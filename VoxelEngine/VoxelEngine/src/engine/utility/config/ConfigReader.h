#pragma once
#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include <tinyxml2/tinyxml2.h>

#include <unordered_map>
#include <string>

#include "Config.h"

class ConfigReader
{
private:
	using ParserFunction = bool (ConfigReader::*)(Config&, const tinyxml2::XMLElement* const);

	inline static const std::string s_configFilePath = "config.xml";
	inline static const std::string s_rootNodeName = "config";

	static const std::unordered_map<std::string, ParserFunction> s_configMappings;

	tinyxml2::XMLDocument m_configFile;

public:
	ConfigReader();

	Config ReadConfigFile();

private:
	bool CheckXMLResult(const tinyxml2::XMLError result) noexcept;
	void ThrowCorruptFileException();

	bool ParseTitle(Config& config, const tinyxml2::XMLElement* const configElement);

	bool ParseWindowWidth(Config& config, const tinyxml2::XMLElement* const configElement);
	bool ParseWindowHeight(Config& config, const tinyxml2::XMLElement* const configElement);

	bool ParseGLVersionMajor(Config& config, const tinyxml2::XMLElement* const configElement);
	bool ParseGLVersionMinor(Config& config, const tinyxml2::XMLElement* const configElement);

	bool ParseEnableVsync(Config& config, const tinyxml2::XMLElement* const configElement);

	bool ParseMultisampleCount(Config& config, const tinyxml2::XMLElement* const configElement);
};

#endif