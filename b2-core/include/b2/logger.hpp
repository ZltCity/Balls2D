#pragma once

#include <functional>
#include <list>
#include <utility>

#include <fmt/chrono.h>
#include <fmt/format.h>

namespace b2
{

enum class LogLevel
{
	Info,
	Warning,
	Error
};

class Logger
{
public:
	using Callback = std::function<void(const std::string &)>;

	Logger(const Logger &) = delete;
	Logger &operator=(const Logger &) = delete;

	inline void log(LogLevel level, const std::string &message) const;

	std::shared_ptr<Callback> setWriteCallback(Callback callback);

	static Logger &getInstance();

private:
	Logger() = default;

	mutable std::list<std::weak_ptr<Callback>> targets;
};

std::string toString(LogLevel level);

inline void info1(const std::string &message);
inline void warning(const std::string &message);
inline void error(const std::string &message);

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

void info1(const std::string &message)
{
	Logger::getInstance().log(LogLevel::Info, message);
}

void warning(const std::string &message)
{
	Logger::getInstance().log(LogLevel::Warning, message);
}

void error(const std::string &message)
{
	Logger::getInstance().log(LogLevel::Error, message);
}

} // namespace b2
