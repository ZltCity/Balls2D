#include <fstream>

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

	if (window == nullptr)
		throw std::runtime_error("Unable to create window.");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	glContext.reset(
		SDL_GL_CreateContext(this->window.get()), [](SDL_GLContext context) { SDL_GL_DeleteContext(context); });

	if (glContext == nullptr)
		throw std::runtime_error("Unable to create GL context.");

	SDL_GL_MakeCurrent(this->window.get(), glContext.get());
}

DesktopApplication::~DesktopApplication()
{
	SDL_Quit();
}

std::vector<Event> DesktopApplication::pollEvents() const
{
	std::vector<Event> events;

	if (firstRun)
	{
		firstRun = false;
		events.emplace_back(Event::WindowCreated, true);
	}

	SDL_Event event;

	while (SDL_PollEvent(&event) != 0)
	{
		if (event.type == SDL_WINDOWEVENT)
		{
			switch (event.window.event)
			{
				case SDL_WINDOWEVENT_CLOSE:
				{
					events.emplace_back(Event::WindowDestroyed, true);
					break;
				};
			}
		}
		else if (event.type == SDL_QUIT)
			events.emplace_back(Event::QuitRequest, true);
	}

	return events;
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
