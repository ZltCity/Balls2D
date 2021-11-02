#include <fstream>

#include <b2/exception.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "desktopapp.hpp"

namespace b2::desktop
{

DesktopApplication::DesktopApplication() : firstRun(true)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	window.reset(
		SDL_CreateWindow("BlueWater 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 900, 900, SDL_WINDOW_OPENGL),
		[](SDL_Window *window) { SDL_DestroyWindow(window); });

	_assert(window != nullptr, 0xe18e3ae1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	glContext.reset(
		SDL_GL_CreateContext(this->window.get()), [](SDL_GLContext context) { SDL_GL_DeleteContext(context); });

	_assert(glContext != nullptr, 0xd6a07fa8);

	SDL_GL_MakeCurrent(this->window.get(), glContext.get());
}

DesktopApplication::~DesktopApplication()
{
	SDL_Quit();
}

Event DesktopApplication::pollEvent() const
{
	if (firstRun)
	{
		firstRun = false;
		return {Event::WindowCreated, true};
	}

	SDL_Event event;

	SDL_PollEvent(&event);

	if (event.type == SDL_WINDOWEVENT)
	{
		switch (event.window.event)
		{
			case SDL_WINDOWEVENT_CLOSE: return {Event::WindowDestroyed, true};
		}
	}
	else if (event.type == SDL_QUIT)
		return {Event::QuitRequest, true};

	return {Event::Empty, None {}};
}

Bytebuffer DesktopApplication::readFile(const std::string &filepath) const
{
	std::fstream stream(filepath, std::fstream::in | std::fstream::binary);

	if (!stream.is_open())
		throw std::runtime_error("Unable to open file.");

	stream.seekg(0, std::fstream::end);

	auto size = size_t(stream.tellg());
	auto buffer = Bytebuffer(size);

	stream.seekg(0, std::fstream::beg);
	stream.read(reinterpret_cast<char *>(buffer.data()), static_cast<std::streamsize>(size));

	return buffer;
}

glm::uvec2 DesktopApplication::getWindowSize() const
{
	if (window == nullptr)
		return {0, 0};

	int w, h;

	SDL_GetWindowSize(window.get(), &w, &h);

	return {w, h};
}

void DesktopApplication::swapBuffers()
{
	if (window == nullptr)
		return;

	SDL_GL_SwapWindow(window.get());
}

} // namespace b2::desktop
