#pragma once

#include <thread>

#include "spinlock.hpp"

namespace b2
{

template<typename T>
class Gearbox
{
public:
	Gearbox(const T &value = T());
	Gearbox(const Gearbox &) = delete;

	Gearbox &operator=(const Gearbox &) = delete;

	void swap(T &value);
	void push(const T &value);

	T &get();

private:
	SpinLock centerLock;
	T left, center;
	bool swapped;
};

template<typename T>
Gearbox<T>::Gearbox(const T &value) : left(value), swapped(false)
{}

template<typename T>
void Gearbox<T>::swap(T &value)
{
	std::lock_guard lock(centerLock);

	std::swap(center, value);
	swapped = true;
}

template<typename T>
void Gearbox<T>::push(const T &value)
{
	std::lock_guard lock(centerLock);

	center = value;
	swapped = true;
}

template<typename T>
T &Gearbox<T>::get()
{
	std::lock_guard lock(centerLock);

	if (swapped)
	{
		std::swap(center, left);
		swapped = false;
	}

	return left;
}

} // namespace b2
