#pragma once

#include <SDL.h>

#include <b2/application.hpp>

namespace b2::desktop
{

class DesktopApplication : public b2::Application
{
public:
	DesktopApplication();
	DesktopApplication(const DesktopApplication &) = delete;
	~DesktopApplication();

	DesktopApplication &operator=(const DesktopApplication &) = delete;

	[[nodiscard]] std::vector<Event> pollEvents() const final;

	[[nodiscard]] Bytebuffer readFile(const std::string &filepath) const final;

	[[nodiscard]] glm::uvec2 getWindowSize() const final;

	void swapBuffers() final;

private:
	std::shared_ptr<SDL_Window> window;
	std::shared_ptr<void> glContext;
	mutable bool firstRun;
};

} // namespace b2::desktop
