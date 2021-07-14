#pragma once

#include <functional>
#include <memory>

#include <EGL/egl.h>

namespace b2::android
{

class RenderContext
{
public:
	RenderContext(ANativeWindow *window);
	RenderContext(const RenderContext &) = delete;

	RenderContext &operator=(const RenderContext &) = delete;

	void swapBuffers();

private:
	struct
	{
		EGLint major, minor;
	} eglVersion;

	using EglPtr = std::unique_ptr<void, std::function<void(void *)>>;

	EglPtr eglDisplay, eglSurface, eglContext;
};

} // namespace b2::android
