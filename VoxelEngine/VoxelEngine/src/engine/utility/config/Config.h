#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config
{
	std::string title;

	unsigned int windowWidth = 0;
	unsigned int windowHeight = 0;

	unsigned int glVersionMajor = 3;
	unsigned int glVersionMinor = 3;

	bool enableVsync = false;

	unsigned int multisampleCount = 4;
};

#endif