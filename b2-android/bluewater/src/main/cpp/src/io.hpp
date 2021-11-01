#pragma once

#include <string>

#include <android_native_app_glue.h>

#include <platform/io.hpp>

namespace b2::android
{

class AndroidIO : public platform::IO
{
public:
	AndroidIO(android_app *androidApp);
	AndroidIO(const AndroidIO &) = delete;

	AndroidIO &operator=(const AndroidIO &) = delete;

	Bytebuffer readFile(const std::string &filepath) const final;

private:
	android_app *androidApp;
};

} // namespace b2-core::android
