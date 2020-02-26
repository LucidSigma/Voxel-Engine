#include "Window.h"

#include <stb_image/stb_image.h>

#include <sstream>
#include <utility>

#include "../../error/GameException.h"

void Window::WindowDestroyer::operator ()(SDL_Window* window) const noexcept
{
	SDL_DestroyWindow(window);
}

Window::Window(const std::string_view& title, const SDL_Rect& rect, const unsigned int multisampleCount, const Uint32 flags)
	: m_windowHandle(SDL_CreateWindow(title.data(), rect.x, rect.y, rect.w, rect.h, flags)), m_size(rect.w, rect.h), m_multisampleCount(multisampleCount)
{
	if (m_windowHandle == nullptr)
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "Failed to create window: " << SDL_GetError();

		throw GameException(GameException::Initialise::SDL, errorMessageStream.str());
	}
}

Window::Window(Window&& other) noexcept
	: m_windowHandle(nullptr), m_size(other.m_size), m_multisampleCount(other.m_multisampleCount)
{
	std::swap(m_windowHandle, other.m_windowHandle);

	other.m_size = glm::uvec2{ 0, 0 };
	other.m_multisampleCount = 0;
}

Window& Window::operator =(Window&& other) noexcept
{
	m_windowHandle = std::exchange(other.m_windowHandle, nullptr);

	m_size = std::exchange(other.m_size, glm::uvec2{ 0, 0 });
	m_multisampleCount = std::exchange(other.m_multisampleCount, 0);

	return *this;
}

Window::~Window() noexcept
{
	Destroy();
}

void Window::ProcessResize(const glm::uvec2& newSize)
{
	if (!m_hasUpdatedFullscreen)
	{
		m_size = newSize;
	}
	else
	{
		m_hasUpdatedFullscreen = false;
	}
}

void Window::ToggleFullscreen()
{
	if (m_isFullscreen)
	{
		SDL_SetWindowSize(GetPointer(), m_sizeBeforeFullscreen->x, m_sizeBeforeFullscreen->y);
		SDL_SetWindowFullscreen(GetPointer(), 0);
		SDL_SetWindowBordered(GetPointer(), SDL_TRUE);

		m_size = m_sizeBeforeFullscreen.value();
		m_sizeBeforeFullscreen = std::nullopt;
		m_hasUpdatedFullscreen = true;
	}
	else
	{
		SDL_DisplayMode displayMode{ };
		SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(GetPointer()), &displayMode);

		m_sizeBeforeFullscreen = m_size;
		m_size = glm::uvec2{ displayMode.w, displayMode.h };

		SDL_SetWindowSize(GetPointer(), m_size.x, m_size.y);
		SDL_SetWindowFullscreen(GetPointer(), SDL_WINDOW_FULLSCREEN);
		SDL_SetWindowBordered(GetPointer(), SDL_FALSE);

		m_hasUpdatedFullscreen = true;
	}

	m_isFullscreen = !m_isFullscreen;
}

void Window::SetIcon(const std::string& iconFilepath)
{
	int width = 0;
	int height = 0;
	std::byte* iconData = reinterpret_cast<std::byte*>(stbi_load(iconFilepath.c_str(), &width, &height, nullptr, STBI_rgb_alpha));

	if (iconData == nullptr)
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "Failed to load icon file.";

		throw GameException(GameException::FileNotFound::Icon, errorMessageStream.str());
	}

	Uint32 redMask = 0x00'00'00'FF;
	Uint32 greenMask = 0x00'00'FF'00;
	Uint32 blueMask = 0x00'FF'00'00;
	Uint32 alphaMask = 0xFF'00'00'00;

	if constexpr (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	{
		redMask = 0xFF'00'00'00;
		greenMask = 0x00'FF'00'00;
		blueMask = 0x00'00'FF'00;
		alphaMask = 0x00'00'00'FF;
	}

	SDL_Surface* iconSurface = SDL_CreateRGBSurfaceFrom(iconData, width, height, 32, width * 4, redMask, greenMask, blueMask, alphaMask);
	SDL_SetWindowIcon(GetPointer(), iconSurface);
	SDL_FreeSurface(iconSurface);

	stbi_image_free(iconData);
}

void Window::Destroy() noexcept
{
	if (m_windowHandle != nullptr)
	{
		m_windowHandle = nullptr;
	}
}