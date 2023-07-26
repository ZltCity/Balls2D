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

	void log(LogLevel level, const std::string &message) const;

	std::shared_ptr<Callback> setWriteCallback(Callback callback);

	static Logger &getInstance();

private:
	Logger() = default;

	mutable std::list<std::weak_ptr<Callback>> targets;
};

std::string toString(LogLevel level);

inline void info(const std::string &message);
inline void warning(const std::string &message);
inline void error(const std::string &message);

void info(const std::string &message)
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
