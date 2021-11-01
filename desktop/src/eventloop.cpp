#include <glm/gtc/matrix_transform.hpp>

#include "eventloop.hpp"
#include "rendercontext.hpp"

#include <iostream>
namespace b2::desktop
{

DesktopEventLoop::DesktopEventLoop(std::unique_ptr<SDL_Window, void (*)(SDL_Window *)> window)
	: window(std::move(window)), angle(0.0f)
{
	std::cout << "DesktopEventLoop()" << std::endl;
}

DesktopEventLoop::~DesktopEventLoop()
{
	std::cout << "~DesktopEventLoop()" << std::endl;
}

void DesktopEventLoop::nextTick()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_WINDOWEVENT:
			{
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_SHOWN:
					{
						handleEvent({platform::Event::WindowCreated, std::make_shared<DesktopRenderContext>(window)});
						break;
					}
					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_CLOSE:
					{
						handleEvent({platform::Event::WindowDestroyed, true});
						break;
					}
				}
				break;
			}
			case SDL_QUIT:
			{
				handleEvent({platform::Event::QuitRequest, true});
				break;
			}
		}
	}

	auto gravity = glm::vec4 {0.0f, -9.81f, 0.0f, 0.0f};
	auto transform = glm::rotate(glm::mat4 {1}, angle, glm::vec3 {0.0f, 0.0f, 1.0f});

	gravity = gravity * transform;

	handleEvent({platform::Event::AccelerationEvent, glm::vec3(gravity)});

	angle += timer.getDeltaMs() * 0.001f;
}

void DesktopEventLoop::setEventHandler(EventHandler eventHandler)
{
	this->eventHandler = eventHandler;
}

void DesktopEventLoop::handleEvent(const platform::Event &event) const
{
	if (!eventHandler)
		return;

	eventHandler(event);
}

} // namespace b2::desktop
