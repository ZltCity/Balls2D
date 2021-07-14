#include <android_native_app_glue.h>

#include <b2/exception.hpp>
#include <b2/gl.hpp>
#include <b2/logger.hpp>

#include "rendercontext.hpp"

namespace b2::android
{

RenderContext::RenderContext(ANativeWindow *window)
{
	eglVersion = {0, 0};

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	_assert(display != EGL_NO_DISPLAY, 0x8ea6aefc);
	_assert(eglInitialize(display, &eglVersion.major, &eglVersion.minor) != EGL_FALSE, 0x0ba4af75);

	eglDisplay = EglPtr(display, [](EGLDisplay display) { eglTerminate(display); });

	info("EGL version: %d.%d", eglVersion.major, eglVersion.minor);

	EGLConfig configPool[16];
	EGLint configsCount = 0, configAttrs[] = {EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
											  EGL_RED_SIZE,		8,
											  EGL_GREEN_SIZE,	8,
											  EGL_BLUE_SIZE,	8,
											  EGL_DEPTH_SIZE,	24,
											  EGL_NONE};

	_assert(
		eglChooseConfig(display, configAttrs, configPool, sizeof(configPool), &configsCount) != EGL_FALSE, 0xf7669c12);
	_assert(configsCount != 0, 0x238112b7);

	info("Suitable EGL framebuffer configurations count: %d", configsCount);

	EGLConfig &defaultConfig = configPool[0];
	EGLSurface surface = eglCreateWindowSurface(display, defaultConfig, window, nullptr);

	_assert(surface != EGL_NO_SURFACE, 0xc639ca32);

	eglSurface = EglPtr(surface, [display](EGLSurface surface) { eglDestroySurface(display, surface); });

	EGLint contextAttrs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
	EGLContext context = eglCreateContext(display, defaultConfig, nullptr, contextAttrs);

	_assert(context != EGL_NO_CONTEXT, 0xe2c81c3d);

	eglContext = EglPtr(context, [display](EGLContext context) { eglDestroyContext(display, context); });

	_assert(eglMakeCurrent(display, surface, surface, context) != EGL_FALSE, 0x5d236635);

	gl::_i(glViewport, 0, 0, ANativeWindow_getWidth(window), ANativeWindow_getHeight(window));
}

void RenderContext::swapBuffers()
{
	eglSwapBuffers(eglDisplay.get(), eglSurface.get());
}

} // namespace b2::android
