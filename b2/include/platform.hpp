#pragma once

#include "event.hpp"

namespace b2
{

class Platform
{
public:
	Platform() = default;
	Platform(const Platform &) = delete;
	virtual ~Platform() = 0;

	Platform &operator=(const Platform &) = delete;

	virtual std::list<Event> pollEvents() = 0;
	virtual void updateDisplay() = 0;
};

} // namespace b2
