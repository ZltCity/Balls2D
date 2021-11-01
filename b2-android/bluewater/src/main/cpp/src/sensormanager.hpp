#pragma once

#include <functional>
#include <memory>

#include <android/sensor.h>

#include <exception.hpp>
#include <platform/eventloop.hpp>

namespace b2::android
{

using namespace platform;

class SensorManager
{
public:
	SensorManager();
	SensorManager(const SensorManager &) = delete;

	SensorManager &operator=(const SensorManager &) = delete;

	std::list<Event> pollEvents() const;

private:
	using EventQueue = std::unique_ptr<ASensorEventQueue, std::function<void(ASensorEventQueue *)>>;

	EventQueue sensorEventQueue;
};

} // namespace b2-core::android
