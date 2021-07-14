#pragma once

#include <functional>
#include <memory>

#include <android/sensor.h>

#include <b2/event.hpp>

namespace b2::android
{

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

} // namespace b2::android
