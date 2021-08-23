#include "eventloop.hpp"

namespace b2::android
{

AndroidEventLoop::AndroidEventLoop(android_app *androidApp) : androidApp(androidApp)
{
	androidApp->userData = this;
	androidApp->onAppCmd = cmdHandler;
	androidApp->onInputEvent = inputHandler;
}

void AndroidEventLoop::nextTick()
{
	android_poll_source *source;
	int32_t events = 0;

	if (ALooper_pollAll(0, nullptr, &events, reinterpret_cast<void **>(&source)) >= 0)
	{
		if (source != nullptr)
			source->process(androidApp, source);

		if (androidApp->destroyRequested != 0)
			handleEvent({Event::QuitRequest, true});
	}

	for (const Event &event : sensorManager.pollEvents())
		handleEvent(event);

	//
	//	if (renderContext != nullptr)
	//		renderContext->swapBuffers();
}

void AndroidEventLoop::setEventHandler(EventHandler eventHandler)
{
	this->eventHandler = std::move(eventHandler);
}

void AndroidEventLoop::handleEvent(const Event &event) const
{
	if (!eventHandler)
		return;

	eventHandler(event);
}

void AndroidEventLoop::cmdHandler(android_app *androidApp, int32_t cmd)
{
	AndroidEventLoop *self = reinterpret_cast<AndroidEventLoop *>(androidApp->userData);

	switch (cmd)
	{
		case APP_CMD_START:
		case APP_CMD_RESUME: break;
		case APP_CMD_STOP:
		case APP_CMD_PAUSE: break;
		case APP_CMD_DESTROY: break;
		case APP_CMD_INIT_WINDOW:
		{
			self->handleEvent({Event::WindowCreated, std::make_shared<AndroidRenderContext>(androidApp->window)});

			break;
		}
		case APP_CMD_TERM_WINDOW:
		{
			self->handleEvent({Event::WindowDestroyed, true});

			break;
		}
	}
}

int32_t AndroidEventLoop::inputHandler(android_app *androidApp, AInputEvent *event)
{
	auto motionPoints = [event]() -> std::vector<glm::vec2> {
		std::vector<glm::vec2> points;

		for (int32_t i = 0; i < AMotionEvent_getPointerCount(event); ++i)
			points.emplace_back(AMotionEvent_getX(event, i), AMotionEvent_getY(event, i));

		return points;
	};

	AndroidEventLoop *self = reinterpret_cast<AndroidEventLoop *>(androidApp->userData);
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
						case AMOTION_EVENT_ACTION_MOVE:
						{
							self->handleEvent(Event(Touch(Touch::Move, motionPoints())));

							break;
						}
						case AMOTION_EVENT_ACTION_UP:
						{
							self->handleEvent(Event(Touch(Touch::Up, motionPoints())));

							break;
						}
						case AMOTION_EVENT_ACTION_DOWN:
						{
							self->handleEvent(Event(Touch(Touch::Down, motionPoints())));

							break;
						}
					}

					break;
			}

			break;
		}
	}

	return 0;
}

} // namespace b2::android
