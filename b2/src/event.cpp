#include "event.hpp"

namespace b2
{

Touch::Touch(Type type, std::vector<glm::vec2> points) : type(type), points(std::move(points))
{}

Event::Event(Touch touch) : type(Event::TouchEvent), payload(std::move(touch))
{}

} // namespace b2
