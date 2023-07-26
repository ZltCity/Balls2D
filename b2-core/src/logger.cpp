#include <b2/logger.hpp>

namespace b2
{

void Logger::log(LogLevel level, const std::string &message) const
{
	auto it = targets.begin();

	while (it != targets.end())
	{
		auto callback = it->lock();

		if (callback == nullptr)
			it = targets.erase(it);
		else
		{
			using namespace std::chrono;

			const auto now = high_resolution_clock::now();
			const auto fraction = time_point_cast<microseconds>(now).time_since_epoch().count() % uint32_t(1e+6);

			(*callback)(fmt::format("{:%H:%M:%S}.{:0<6} [{:^7}] {}\n", now, fraction, toString(level), message));
			++it;
		}
	}
}

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
