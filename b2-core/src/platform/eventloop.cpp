#include "platform/eventloop.hpp"

namespace b2::platform
{

Touch::Touch(Type type, std::vector<glm::vec2> points) : type(type), points(std::move(points))
{}

Event::Event(Touch touch) : type(Event::TouchEvent), payload(std::move(touch))
{}

EventLoop::~EventLoop()
{}

} // namespace b2-core::platform
