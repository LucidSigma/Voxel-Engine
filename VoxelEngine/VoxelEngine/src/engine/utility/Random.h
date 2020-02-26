#pragma once
#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <type_traits>

class Random
{
private:
	inline static std::random_device s_randomSeed;
	inline static std::mt19937 s_randomEngine{ s_randomSeed() };

public:
	Random() = delete;
	~Random() noexcept = delete;

	template <typename T = float, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	static T GenerateFloat(const T& min, const T& max)
	{
		const std::uniform_real_distribution<T> randomRange(min, max);

		return randomRange(s_randomEngine);
	}

	template <typename T = int, typename = std::enable_if_t<std::is_integral_v<T>>>
	static T GenerateInt(const T& min, const T& max)
	{
		const std::uniform_int_distribution<T> randomRange(min, max);

		return randomRange(s_randomEngine);
	}
};

#endif