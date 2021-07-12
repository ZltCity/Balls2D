#pragma once

#include <memory>

namespace b2
{

class Application
{
public:
	Application(const Application &) = delete;
	virtual ~Application() = 0;

	Application &operator=(const Application &) = delete;

	static std::shared_ptr<Application> create();
};

} // namespace b2
