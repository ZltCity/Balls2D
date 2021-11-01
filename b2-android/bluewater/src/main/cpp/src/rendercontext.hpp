#pragma once

#include <functional>
#include <memory>

#include <EGL/egl.h>
#include <android_native_app_glue.h>

#include <platform/rendercontext.hpp>

namespace b2::android
{

using namespace platform;

class AndroidRenderContext : public RenderContext
{
public:
	AndroidRenderContext(ANativeWindow *window);
	AndroidRenderContext(const AndroidRenderContext &) = delete;

	AndroidRenderContext &operator=(const AndroidRenderContext &) = delete;

	void swapBuffers() final;

	glm::ivec2 getSurfaceSize() const final;

private:
	struct
	{
		EGLint major, minor;
	} eglVersion;

	using EglPtr = std::unique_ptr<void, std::function<void(void *)>>;

	ANativeWindow *window;
	EglPtr eglDisplay, eglSurface, eglContext;
};

} // namespace b2-core::android
