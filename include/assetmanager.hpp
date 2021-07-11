#pragma once

#include <string>

#include <android/asset_manager.h>
#include <android_native_app_glue.h>

#include "bytebuffer.hpp"

namespace b2::android
{

void run(android_app *);

}

namespace b2::system
{

class AssetManager
{
	friend void android::run(android_app *);

public:
	Bytebuffer readFile(const std::string &filepath) const;

private:
	AssetManager(AAssetManager *manager);

	AAssetManager *manager;
};

} // namespace b2::system
