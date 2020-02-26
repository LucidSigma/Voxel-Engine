#include "StringUtility.h"

#include <algorithm>
#include <locale>
#include <iterator>

std::string ToLowerCase(std::string string)
{
	std::transform(std::begin(string), std::end(string), std::begin(string), [](const unsigned char letter)
	{
		return std::tolower(letter, std::locale());
	});

	return string;
}

std::string ToUpperCase(std::string string)
{
	std::transform(std::begin(string), std::end(string), std::begin(string), [](const unsigned char letter)
	{
		return std::toupper(letter, std::locale());
	});

	return string;
}