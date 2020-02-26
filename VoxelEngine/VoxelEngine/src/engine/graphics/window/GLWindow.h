#pragma once
#ifndef GL_WINDOW_H
#define GL_WINDOW_H

#include "Window.h"

class GLWindow
	: public Window
{
private:
	SDL_GLContext m_glContext = nullptr;

public:
	GLWindow() = default;
	GLWindow(const std::string_view& title, const SDL_Rect& rect, const unsigned int multisampleCount = 4, const Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	GLWindow(GLWindow&& other) noexcept;
	GLWindow& operator =(GLWindow&& other) noexcept;

	virtual ~GLWindow() noexcept override;

	void MakeCurrentContext() const;
	void SwapBuffers() const;

private:
	virtual void Destroy() noexcept override;
	void DestroyContext() noexcept;
};

#endif