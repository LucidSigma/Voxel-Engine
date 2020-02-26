#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include "../../interfaces/INoncopyable.h"

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

class Window
	: private INoncopyable
{
private:
	struct WindowDestroyer
	{
		void operator ()(SDL_Window* window) const noexcept;
	};

	std::unique_ptr<SDL_Window, WindowDestroyer> m_windowHandle = nullptr;

	glm::uvec2 m_size{ 0, 0 };
	std::optional<glm::uvec2> m_sizeBeforeFullscreen = std::nullopt;
	unsigned int m_multisampleCount = 4;

	bool m_isFullscreen = false;
	bool m_hasUpdatedFullscreen = false;

public:
	Window() = default;
	Window(const std::string_view& title, const SDL_Rect& rect, const unsigned int multisampleCount = 4, const Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);

	Window(Window&& other) noexcept;
	Window& operator =(Window&& other) noexcept;

	virtual ~Window() noexcept;

	void ProcessResize(const glm::uvec2& newSize);
	void ToggleFullscreen();

	void SetIcon(const std::string& iconFilepath);

	inline const glm::uvec2& GetSize() const noexcept { return m_size; }
	inline unsigned int GetMultisampleCount() const noexcept { return m_multisampleCount; }

	inline SDL_Window* GetPointer() const noexcept { return m_windowHandle.get(); }
	[[nodiscard]] inline SDL_Surface* GetSurface() const noexcept { return SDL_GetWindowSurface(GetPointer()); }

protected:
	virtual void Destroy() noexcept;
};

#endif