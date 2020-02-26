#include "GLWindow.h"

#include <sstream>
#include <utility>

#include "../../error/GameException.h"

GLWindow::GLWindow(const std::string_view& title, const SDL_Rect& rect, const unsigned int multisampleCount, const Uint32 flags)
	: Window(title, rect, multisampleCount, flags)
{
	m_glContext = SDL_GL_CreateContext(GetPointer());

	if (m_glContext == nullptr)
	{
		std::ostringstream errorMessageStream;
		errorMessageStream << "Failed to create OpenGL context: " << SDL_GetError();

		throw GameException(GameException::Initialise::SDL, errorMessageStream.str());
	}
}

GLWindow::GLWindow(GLWindow&& other) noexcept
	: Window(std::move(other))
{
	DestroyContext();
	std::swap(m_glContext, other.m_glContext);
}

GLWindow& GLWindow::operator =(GLWindow&& other) noexcept
{
	Window::operator =(std::move(other));

	DestroyContext();
	m_glContext = std::exchange(other.m_glContext, nullptr);

	return *this;
}

GLWindow::~GLWindow() noexcept
{
	Destroy();
}

void GLWindow::MakeCurrentContext() const
{
	SDL_GL_MakeCurrent(GetPointer(), m_glContext);
}

void GLWindow::SwapBuffers() const
{
	SDL_GL_SwapWindow(GetPointer());
}

void GLWindow::Destroy() noexcept
{
	DestroyContext();
	Window::Destroy();
}

void GLWindow::DestroyContext() noexcept
{
	if (m_glContext != nullptr)
	{
		SDL_GL_DeleteContext(m_glContext);
		m_glContext = nullptr;
	}
}