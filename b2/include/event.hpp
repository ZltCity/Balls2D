#pragma once

#include <list>
#include <variant>
#include <vector>

#include <glm/glm.hpp>

namespace b2
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
	std::variant<bool, glm::ivec2, glm::vec3, Touch> payload;
};

template<typename T>
Event::Event(Type type, T payload) : type(type), payload(std::move(payload))
{}

} // namespace b2
