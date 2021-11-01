#pragma once

#include <mutex>
#include <thread>

namespace b2
{

template<typename T>
class Gearbox
{
public:
	explicit Gearbox(const T &input = T());
	Gearbox(const Gearbox &) = delete;

	Gearbox &operator=(const Gearbox &) = delete;

	void turn(T &input);

	T &get();

private:
	std::mutex spindleLock;
	T output, spindle;
	bool turned;
};

template<typename T>
Gearbox<T>::Gearbox(const T &input) : output(input), turned(false)
{}

template<typename T>
void Gearbox<T>::turn(T &input)
{
	std::lock_guard lock(spindleLock);

	std::swap(spindle, input);
	turned = true;
}

template<typename T>
T &Gearbox<T>::get()
{
	std::lock_guard lock(spindleLock);

	if (turned)
	{
		std::swap(spindle, output);
		turned = false;
	}

	return output;
}

} // namespace b2
