#pragma once
#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

template <typename T>
class AssetManager
{
private:
	std::unordered_map<std::string, std::unique_ptr<T>> m_assets;

public:
	inline const T& Get(const std::string& name) const { return *m_assets.at(name); }
	inline const T& operator [](const std::string& name) const { return Get(name); }

protected:
	AssetManager() = default;
	virtual ~AssetManager() noexcept = default;

	template <typename... Args>
	void Add(const std::string& name, Args&&... args)
	{
		m_assets[name] = std::make_unique<T>(std::forward<Args>(args)...);
	}
};

#endif