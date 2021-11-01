#pragma once

#include <memory>

#include <SDL.h>
#include <platform/rendercontext.hpp>

namespace b2::desktop
{

class DesktopRenderContext : public platform::RenderContext
{
public:
	explicit DesktopRenderContext(std::shared_ptr<SDL_Window> window);
	~DesktopRenderContext();
	DesktopRenderContext(const DesktopRenderContext &) = delete;

	DesktopRenderContext &operator=(const DesktopRenderContext &) = delete;

	void swapBuffers() final;

	[[nodiscard]] glm::ivec2 getSurfaceSize() const final;

private:
	std::shared_ptr<SDL_Window> window;
	std::shared_ptr<void> glContext;
};

} // namespace b2-core::b2-app
