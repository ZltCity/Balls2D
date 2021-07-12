#include <EGL/egl.h>
#include <android/sensor.h>

#include <exception.hpp>
#include <game.hpp>
#include <gapi.hpp>

#include "application.hpp"

namespace b2::android
{

class RenderContext
{
public:
	RenderContext(ANativeWindow *window);

	struct
	{
		EGLint major, minor;
	} eglVersion;

	std::shared_ptr<void> eglDisplay, eglSurface, eglContext;
};

void initSensors();

void run(android_app *androidApp)
{
	struct State
	{
		std::shared_ptr<RenderContext> renderContext;
		std::shared_ptr<ASensorEventQueue> sensorEventQueue;
		std::shared_ptr<Game> game;
	} state;

	auto initSensors = [&state]() {
		ASensorManager *sensorManager = ASensorManager_getInstance();

		_assert(sensorManager != nullptr, 0x5fd881d9);

		ASensorList sensors;
		const ASensor *accelSensor = nullptr;

		for (int32_t i = 0; i < ASensorManager_getSensorList(sensorManager, &sensors); ++i)
		{
			int32_t sensorType = ASensor_getType(sensors[i]);

			switch (sensorType)
			{
				case ASENSOR_TYPE_ACCELEROMETER: accelSensor = sensors[i]; break;
			}
		}

		_assert(accelSensor != nullptr, 0x37753fba);

		ASensorEventQueue *eventQueue = ASensorManager_createEventQueue(
			sensorManager, ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS), 0, nullptr, nullptr);

		_assert(eventQueue != nullptr, 0xcc5b7924);

		state.sensorEventQueue.reset(eventQueue, [sensorManager](ASensorEventQueue *queue) {
			ASensorManager_destroyEventQueue(sensorManager, queue);
		});

		_assert(ASensorEventQueue_enableSensor(eventQueue, accelSensor) == 0, 0xae3cf11b);
	};
	auto processSensors = [&state]() {
		ASensorEvent events[4];
		size_t count = ASensorEventQueue_getEvents(state.sensorEventQueue.get(), events, sizeof(events));

		for (size_t i = 0; i < count; ++i)
		{
			const ASensorEvent &event = events[i];

			switch (event.type)
			{
				case ASENSOR_TYPE_ACCELEROMETER:
				{
					if (state.game != nullptr)
						state.game->onSensorsEvent(
							{-event.acceleration.x, -event.acceleration.y, event.acceleration.z});

					break;
				}
			}
		}
	};
	auto processEvents = [androidApp, &processSensors]() {
		android_poll_source *source;
		int32_t events = 0;

		while (ALooper_pollAll(0, nullptr, &events, reinterpret_cast<void **>(&source)) >= 0)
		{
			if (source != nullptr)
				source->process(androidApp, source);

			if (androidApp->destroyRequested != 0)
				return;

			processSensors();
		}
	};
	auto frame = [&state]() {
		if (state.game != nullptr)
			state.game->update();

		if (state.renderContext != nullptr)
			eglSwapBuffers(state.renderContext->eglDisplay.get(), state.renderContext->eglSurface.get());
	};

	androidApp->userData = &state;
	androidApp->onAppCmd = [](android_app *androidApp, int32_t cmd) {
		State *state = reinterpret_cast<State *>(androidApp->userData);

		switch (cmd)
		{
			case APP_CMD_START:
			case APP_CMD_RESUME: break;
			case APP_CMD_STOP:
			case APP_CMD_PAUSE: break;
			case APP_CMD_DESTROY: break;
			case APP_CMD_INIT_WINDOW:
			{
				state->renderContext.reset(new RenderContext(androidApp->window));
				state->game = std::make_shared<Game>(
					system::AssetManager(androidApp->activity->assetManager),
					glm::ivec2(
						ANativeWindow_getWidth(androidApp->window), ANativeWindow_getHeight(androidApp->window)));

				break;
			}
			case APP_CMD_TERM_WINDOW:
			{
				state->game.reset();
				state->renderContext.reset();

				break;
			}
		}
	};
	androidApp->onInputEvent = [](android_app *androidApp, AInputEvent *event) -> int32_t {
		[[maybe_unused]] auto motionPoints = [event]() -> std::vector<glm::vec2> {
			std::vector<glm::vec2> points;

			for (int32_t i = 0; i < AMotionEvent_getPointerCount(event); ++i)
				points.push_back(glm::vec2(AMotionEvent_getX(event, i), AMotionEvent_getY(event, i)));

			return points;
		};

		[[maybe_unused]] State *state = reinterpret_cast<State *>(androidApp->userData);
		int32_t eventType = AInputEvent_getType(event);

		switch (eventType)
		{
			case AINPUT_EVENT_TYPE_MOTION:
			{
				switch (AInputEvent_getSource(event))
				{
					case AINPUT_SOURCE_TOUCHSCREEN:
						int action = AKeyEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;

						switch (action)
						{
							case AMOTION_EVENT_ACTION_DOWN:
							{
								break;
							}
							case AMOTION_EVENT_ACTION_MOVE:
							{
								break;
							}
							case AMOTION_EVENT_ACTION_UP:
							{
								break;
							}
						}

						break;
				}

				break;
			}
		}

		return 0;
	};

	initSensors();

	while (true)
	{
		processEvents();
		frame();
	}
}

RenderContext::RenderContext(ANativeWindow *window)
{
	eglVersion = {0, 0};

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	_assert(display != EGL_NO_DISPLAY, 0x8ea6aefc);
	_assert(eglInitialize(display, &eglVersion.major, &eglVersion.minor) != EGL_FALSE, 0x0ba4af75);

	eglDisplay.reset(display, [](EGLDisplay display) { eglTerminate(display); });

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

	eglSurface.reset(surface, [display](EGLSurface surface) { eglDestroySurface(display, surface); });

	EGLint contextAttrs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
	EGLContext context = eglCreateContext(display, defaultConfig, nullptr, contextAttrs);

	_assert(context != EGL_NO_CONTEXT, 0xe2c81c3d);

	eglContext.reset(context, [display](EGLContext context) { eglDestroyContext(display, context); });

	_assert(eglMakeCurrent(display, surface, surface, context) != EGL_FALSE, 0x5d236635);

	render::_i(glViewport, 0, 0, ANativeWindow_getWidth(window), ANativeWindow_getHeight(window));
}

} // namespace b2::android
