#pragma once

#include <mutex>

#include <android_native_app_glue.h>

#include <b2/platform.hpp>

#include "rendercontext.hpp"
#include "sensormanager.hpp"

namespace b2::android
{

class AndroidPlatform : public Platform
{
public:
	AndroidPlatform(android_app *androidApp);
	AndroidPlatform(const AndroidPlatform &) = delete;

	AndroidPlatform &operator=(const AndroidPlatform &) = delete;

	void nextTick();
	Bytebuffer readFile(const std::string &filepath) const;

	void setEventHandler(EventHandler eventHandler);

private:
	void handleEvent(const Event &event) const;

	static void cmdHandler(android_app *androidApp, int32_t cmd);
	static int32_t inputHandler(android_app *androidApp, AInputEvent *event);

	android_app *androidApp;

	EventHandler eventHandler;

	std::unique_ptr<RenderContext> renderContext;
	SensorManager sensorManager;
};

} // namespace b2::android
