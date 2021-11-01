#include "rendercontext.hpp"
#include <iostream>
namespace b2::desktop
{

DesktopRenderContext::DesktopRenderContext(std::shared_ptr<SDL_Window> window) : window(std::move(window))
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	glContext.reset(
		SDL_GL_CreateContext(this->window.get()), [](SDL_GLContext context) { SDL_GL_DeleteContext(context); });

	if (glContext == nullptr)
		throw std::runtime_error("Unable to create OpenGL context.");

	SDL_GL_MakeCurrent(this->window.get(), glContext.get());
}

DesktopRenderContext::~DesktopRenderContext()
{
}

void DesktopRenderContext::swapBuffers()
{
	if (window == nullptr)
		return;

	SDL_GL_SwapWindow(window.get());
}

glm::ivec2 DesktopRenderContext::getSurfaceSize() const
{
	if (window == nullptr)
		throw std::runtime_error("Unable to get window size.");

	int w, h;

	SDL_GetWindowSize(window.get(), &w, &h);

	return {w, h};
}

} // namespace b2-core::b2-app
