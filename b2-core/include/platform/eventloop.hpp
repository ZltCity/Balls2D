#pragma once

#include <functional>
#include <list>
#include <memory>
#include <variant>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "rendercontext.hpp"

namespace b2::platform
{

struct Touch
{
	enum Type
	{
		Move = 0,
		Up,
		Down
	};

	Touch(Type type, std::vector<glm::vec2> points);

	Type type;
	std::vector<glm::vec2> points;
};

struct Event
{
	enum Type
	{
		QuitRequest = 0,
		WindowCreated,
		WindowDestroyed,
		TouchEvent,
		AccelerationEvent
	};

	explicit Event(Touch touch);
	template<typename T>
	Event(Type type, T payload);

	Type type;
	std::variant<bool, glm::ivec2, glm::vec3, Touch, std::shared_ptr<RenderContext>> payload;
};

class EventLoop
{
public:
	using EventHandler = std::function<void(const Event &event)>;

	EventLoop() = default;
	EventLoop(const EventLoop &) = delete;

	virtual ~EventLoop() = 0;

	EventLoop &operator=(const EventLoop &) = delete;

	virtual void nextTick() = 0;
	virtual void setEventHandler(EventHandler eventHandler) = 0;
};

template<typename T>
Event::Event(Type type, T payload) : type(type), payload(std::move(payload))
{}

} // namespace b2-core::platform
