#include "sensormanager.hpp"

namespace b2::android
{

SensorManager::SensorManager()
{
	ASensorManager *sensorManager = ASensorManager_getInstance();

	_assert(sensorManager != nullptr, 0x5fd881d9);

	ASensorList sensors;
	const ASensor *accelSensor = nullptr;

	for (int32_t i = 0; i < ASensorManager_getSensorList(sensorManager, &sensors); ++i)
	{
		int32_t sensorType = ASensor_getType(sensors[i]);

		switch (sensorType)
		{
			case ASENSOR_TYPE_ACCELEROMETER: accelSensor = sensors[i]; break;
		}
	}

	_assert(accelSensor != nullptr, 0x37753fba);

	ASensorEventQueue *eventQueue = ASensorManager_createEventQueue(
		sensorManager, ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS), 0, nullptr, nullptr);

	_assert(eventQueue != nullptr, 0xcc5b7924);

	sensorEventQueue = EventQueue(eventQueue, [sensorManager](ASensorEventQueue *queue) {
		ASensorManager_destroyEventQueue(sensorManager, queue);
	});

	_assert(ASensorEventQueue_enableSensor(eventQueue, accelSensor) == 0, 0xae3cf11b);
}

std::list<Event> SensorManager::pollEvents() const
{
	std::list<Event> result;
	ASensorEvent events[4];
	size_t count = ASensorEventQueue_getEvents(sensorEventQueue.get(), events, sizeof(events));

	for (size_t i = 0; i < count; ++i)
	{
		const ASensorEvent &event = events[i];

		switch (event.type)
		{
			case ASENSOR_TYPE_ACCELEROMETER:
			{
				result.emplace_back(
					Event::AccelerationEvent,
					glm::vec3(event.acceleration.x, event.acceleration.y, event.acceleration.z));

				break;
			}
		}
	}

	return result;
}

} // namespace b2-core::android
