#pragma once

#include <android_native_app_glue.h>

#include "rendercontext.hpp"
#include "sensormanager.hpp"

namespace b2::android
{

using namespace platform;

class AndroidEventLoop : public EventLoop
{
public:
	AndroidEventLoop(android_app *androidApp);
	AndroidEventLoop(const AndroidEventLoop &) = delete;

	AndroidEventLoop &operator=(const AndroidEventLoop &) = delete;

	void nextTick() final;
	void setEventHandler(EventHandler eventHandler) final;

private:
	void handleEvent(const Event &event) const;

	static void cmdHandler(android_app *androidApp, int32_t cmd);
	static int32_t inputHandler(android_app *androidApp, AInputEvent *event);

	android_app *androidApp;

	EventHandler eventHandler;
	SensorManager sensorManager;
};

} // namespace b2::android
