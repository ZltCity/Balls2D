#pragma once

#include <android/asset_manager.h>
#include <android_native_app_glue.h>

#include "bytebuffer.hpp"

namespace b2
{

class System
{
	friend class Application;

public:
	System(const System &) = delete;
	System &operator=(const System &) = delete;

	Bytebuffer readFile(const char *filepath) const;

private:
	System(AAssetManager *assetManager);

	AAssetManager *assetManager;
};

} // namespace b2
