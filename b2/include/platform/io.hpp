#pragma once

#include <string>

#include "../bytebuffer.hpp"

namespace b2::platform
{

class IO
{
public:
	IO() = default;
	IO(const IO &) = delete;
	virtual ~IO() = 0;

	IO &operator=(const IO &) = delete;

	virtual Bytebuffer readFile(const std::string &filepath) const = 0;
};

} // namespace b2::platform
