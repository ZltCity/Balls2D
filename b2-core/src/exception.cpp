#include <map>
#include <string>

#include "exception.hpp"

namespace b2
{

static const std::map<int32_t, std::string> errTable = {
	{0x071fb7d9, std::string("Index is out of the range.")},
	{0x461f5dfa, std::string("Input buffer is empty.")},
	{0x0f77c02e, std::string("File does not exist.")},
	{0x1e49e9a9, std::string("IO error while reading file.")},
	{0x620a358f, std::string("Invalid workers count.")},
	{0x5fd881d9, std::string("Unable to get sensor manager.")},
	{0x37753fba, std::string("Could not find accelerometer.")},
	{0xcc5b7924, std::string("Unable to create sensor event queue.")},
	{0xae3cf11b, std::string("Could not enable accelerometer.")},
	{0x8ea6aefc, std::string("Unable to get display connection.")},
	{0x0ba4af75, std::string("Unable to initialize display connection.")},
	{0xf7669c12, std::string("Unable to choose EGL config.")},
	{0x238112b7, std::string("There is no suitable EGL config.")},
	{0xc639ca32, std::string("Unable to create EGL surface.")},
	{0xe2c81c3d, std::string("Unable to create EGL context.")},
	{0x5d236635, std::string("Unable to set EGL context.")},
	{0x13182aca, std::string("Uninitialized GAPI object.")},
	{0xa2f0db57, std::string("Undefined texture format.")},
	{0x9800a19c, std::string("GL_INVALID_ENUM.")},
	{0x636e76a1, std::string("GL_INVALID_VALUE.")},
	{0x02f3f490, std::string("GL_INVALID_OPERATION.")},
	{0x8e699b5d, std::string("GL_OUT_OF_MEMORY.")},
	{0xd78eead8, std::string("Invalid config value.")}};

Exception::Exception(int32_t code, const char *brief) : runtime_error(brief), code(code)
{}

Exception::Exception(int32_t code, const std::string &brief) : runtime_error(brief), code(code)
{}

int32_t Exception::getCode() const
{
	return code;
}

void _assert(bool x, int32_t code)
{
	if (!x)
		throw Exception(code, errorBrief(code));
}

std::string errorBrief(int32_t code)
{
	auto iter = errTable.find(code);

	return iter == errTable.end() ? std::string() : iter->second;
}

} // namespace b2-core
