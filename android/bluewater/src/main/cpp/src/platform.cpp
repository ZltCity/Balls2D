#include <b2/bytebuffer.hpp>
#include <b2/exception.hpp>
#include <b2/logger.hpp>

#include "platform.hpp"

namespace b2
{

void entry(std::shared_ptr<Platform>);

namespace android
{

AndroidPlatform::AndroidPlatform(android_app *androidApp) : androidApp(androidApp)
{
	androidApp->userData = this;
	androidApp->onAppCmd = cmdHandler;
	androidApp->onInputEvent = inputHandler;
}

std::list<Event> AndroidPlatform::pollEvents()
{
	android_poll_source *source;
	int32_t events = 0;

	if (ALooper_pollAll(0, nullptr, &events, reinterpret_cast<void **>(&source)) >= 0)
	{
		if (source != nullptr)
			source->process(androidApp, source);

		if (androidApp->destroyRequested != 0)
			pushEvent({Event::QuitRequest, true});
	}

	auto commonEvents = moveEventList(), sensorEvents = sensorManager.pollEvents();

	if (!sensorEvents.empty())
		commonEvents.insert(commonEvents.end(), sensorEvents.begin(), sensorEvents.end());

	return commonEvents;
}

void AndroidPlatform::updateDisplay()
{
	if (renderContext == nullptr)
		return;

	renderContext->swapBuffers();
}

void AndroidPlatform::pushEvent(Event event)
{
	std::lock_guard<std::mutex> lock(eventListLock);

	eventList.push_back(std::move(event));
}

std::list<Event> AndroidPlatform::moveEventList()
{
	std::lock_guard<std::mutex> lock(eventListLock);

	if (eventList.empty())
		return {};

	auto result = std::move(eventList);

	eventList = std::list<Event>();

	return result;
}

void AndroidPlatform::cmdHandler(android_app *androidApp, int32_t cmd)
{
	AndroidPlatform *self = reinterpret_cast<AndroidPlatform *>(androidApp->userData);

	switch (cmd)
	{
		case APP_CMD_START:
		case APP_CMD_RESUME: break;
		case APP_CMD_STOP:
		case APP_CMD_PAUSE: break;
		case APP_CMD_DESTROY: break;
		case APP_CMD_INIT_WINDOW:
		{
			self->renderContext = std::make_unique<RenderContext>(androidApp->window);

			break;
		}
		case APP_CMD_TERM_WINDOW:
		{
			self->renderContext.reset();

			break;
		}
	}
}

int32_t AndroidPlatform::inputHandler(android_app *androidApp, AInputEvent *event)
{
	auto motionPoints = [event]() -> std::vector<glm::vec2> {
		std::vector<glm::vec2> points;

		for (int32_t i = 0; i < AMotionEvent_getPointerCount(event); ++i)
			points.emplace_back(AMotionEvent_getX(event, i), AMotionEvent_getY(event, i));

		return points;
	};

	AndroidPlatform *self = reinterpret_cast<AndroidPlatform *>(androidApp->userData);
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
							self->pushEvent({Touch(Touch::Move, motionPoints())});

							break;
						}
						case AMOTION_EVENT_ACTION_UP:
						{
							self->pushEvent({Touch(Touch::Up, motionPoints())});

							break;
						}
						case AMOTION_EVENT_ACTION_DOWN:
						{
							self->pushEvent({Touch(Touch::Down, motionPoints())});

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

} // namespace android

} // namespace b2

void android_main(android_app *androidApp)
try
{
	b2::entry(std::make_shared<b2::android::AndroidPlatform>(androidApp));
}
catch (const std::exception &ex)
{
	throw;
}
