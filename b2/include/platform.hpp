#pragma once

#include "event.hpp"

namespace b2
{

class Platform
{
public:
	using EventHandler = std::function<void(const Event &event)>;

	Platform() = default;
	Platform(const Platform &) = delete;
	virtual ~Platform() = 0;

	Platform &operator=(const Platform &) = delete;

	virtual void nextTick() = 0;
	virtual Bytebuffer readFile(const std::string &filepath) const = 0;

	virtual void setEventHandler(EventHandler eventHandler) = 0;
};

} // namespace b2
