#pragma once

#include <stdexcept>

namespace b2
{

class Exception : public std::runtime_error
{
public:
	Exception(int32_t code, const char *brief);
	Exception(int32_t code, const std::string &brief);

	int32_t getCode() const;

private:
	int32_t code;
};

void _assert(bool x, int32_t code);

std::string errorBrief(int32_t code);

} // namespace b2-core
