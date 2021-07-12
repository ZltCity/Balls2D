#pragma once

#include <utility>

#include <android/log.h>

#include "config.hpp"

namespace b2
{

template<int32_t level>
struct Log
{
	template<typename... Args>
	Log(const char *format, Args &&...args)
	{
		__android_log_print(level, Config::tag, format, std::forward<Args>(args)...);
	}
};

using info = Log<ANDROID_LOG_INFO>;
using warn = Log<ANDROID_LOG_WARN>;
using crit = Log<ANDROID_LOG_ERROR>;

} // namespace b2
