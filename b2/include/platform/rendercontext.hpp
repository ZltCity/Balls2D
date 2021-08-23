#pragma once

#include <glm/vec2.hpp>

namespace b2::platform
{

class RenderContext
{
public:
	RenderContext() = default;
	RenderContext(const RenderContext &) = delete;

	virtual ~RenderContext() = 0;

	RenderContext &operator=(const RenderContext &) = delete;

	virtual void swapBuffers() = 0;

	[[nodiscard]] virtual glm::ivec2 getSurfaceSize() const = 0;
};

} // namespace b2::platform
