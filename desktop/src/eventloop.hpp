#pragma once

#include <SDL2/SDL.h>

#include <b2/platform/eventloop.hpp>
#include <b2/timer.hpp>

namespace b2::desktop
{

class DesktopEventLoop : public platform::EventLoop
{
public:
	explicit DesktopEventLoop(std::unique_ptr<SDL_Window, void (*)(SDL_Window *)> window);
	~DesktopEventLoop();
	DesktopEventLoop(const DesktopEventLoop &) = delete;

	DesktopEventLoop &operator=(const DesktopEventLoop &) = delete;

	void nextTick() final;
	void setEventHandler(EventHandler eventHandler) final;

private:
	void handleEvent(const platform::Event &event) const;

	EventHandler eventHandler;
	std::shared_ptr<SDL_Window> window;
	float angle;
	Timer timer;
};

} // namespace b2::desktop
