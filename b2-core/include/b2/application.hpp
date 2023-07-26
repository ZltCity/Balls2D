#pragma once

#include <memory>
#include <variant>
#include <vector>

#include <glm/glm.hpp>

#include <b2/bytebuffer.hpp>

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

struct None
{};

struct Event
{
	enum Type
	{
		Empty,
		QuitRequest,
		WindowCreated,
		WindowDestroyed,
		TouchEvent,
		AccelerationEvent
	};

	Event();
	explicit Event(Touch touch);
	template<typename T>
	Event(Type type, T payload);

	Type type;
	std::variant<None, bool, glm::ivec2, glm::vec3, Touch> payload;
};

class Application
{
public:
	Application() = default;
	Application(const Application &) = delete;
	virtual ~Application() = 0;

	Application &operator=(const Application &) = delete;

	[[nodiscard]] virtual std::vector<Event> pollEvents() const = 0;

	[[nodiscard]] virtual glm::uvec2 getWindowSize() const = 0;

	virtual void swapBuffers() = 0;
};

void main(std::shared_ptr<Application> application);

template<typename T>
Event::Event(Type type, T payload) : type(type), payload(std::move(payload))
{}

} // namespace b2
