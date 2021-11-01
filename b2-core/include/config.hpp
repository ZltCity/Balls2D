#pragma once

#include <nlohmann/json.hpp>

#include "bytebuffer.hpp"

namespace b2
{

struct Config
{
	Config() = default;
	Config(const Bytebuffer &buffer);

	operator Bytebuffer() const;

	static const char *const tag;

	nlohmann::json json;
};

} // namespace b2-core
