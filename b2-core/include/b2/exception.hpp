#pragma once

#include <stdexcept>

namespace b2
{

class Exception : public std::runtime_error
{
public:
	explicit inline Exception(uint32_t code);
	inline Exception(uint32_t code, const std::string &brief);

	[[nodiscard]] inline uint32_t getCode() const;

private:
	uint32_t code;
};

inline void _assert(bool x, uint32_t code);

std::string errorBrief(uint32_t code);

Exception::Exception(uint32_t code) : runtime_error(errorBrief(code)), code(code)
{}

Exception::Exception(uint32_t code, const std::string &brief) : std::runtime_error(brief), code(code)
{}

uint32_t Exception::getCode() const
{
	return code;
}

void _assert(bool x, uint32_t code)
{
	if (!x)
		throw Exception(code);
}

} // namespace b2
