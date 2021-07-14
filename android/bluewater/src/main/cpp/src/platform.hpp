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

	std::list<Event> pollEvents();
	void updateDisplay();

private:
	void pushEvent(Event event);
	std::list<Event> moveEventList();

	static void cmdHandler(android_app *androidApp, int32_t cmd);
	static int32_t inputHandler(android_app *androidApp, AInputEvent *event);

	android_app *androidApp;

	std::list<Event> eventList;
	std::mutex eventListLock;

	std::unique_ptr<RenderContext> renderContext;
	SensorManager sensorManager;
};

} // namespace b2::android
