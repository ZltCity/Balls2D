#include <b2/logger.hpp>

namespace b2
{

auto Logger::setWriteCallback(Callback callback) -> std::shared_ptr<Callback>
{
	auto anchor = std::make_shared<Callback>(std::move(callback));

	targets.push_back(anchor);

	return anchor;
}

Logger &Logger::getInstance()
{
	static Logger instance;

	return instance;
}

std::string toString(LogLevel level)
{
	switch (level)
	{
		case LogLevel::Info: return {"info"};
		case LogLevel::Warning: return {"warning"};
		case LogLevel::Error: return {"error"};
		default: return {};
	}
}

} // namespace b2
