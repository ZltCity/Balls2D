#include <b2/exception.hpp>

#include "io.hpp"

namespace b2::android
{

AndroidIO::AndroidIO(android_app *androidApp) : androidApp(androidApp)
{}

Bytebuffer AndroidIO::readFile(const std::string &filepath) const
{
	std::unique_ptr<AAsset, std::function<void(AAsset *)>> asset(
		AAssetManager_open(androidApp->activity->assetManager, filepath.c_str(), AASSET_MODE_BUFFER),
		[](AAsset *asset) { AAsset_close(asset); });

	_assert(asset != nullptr, 0x0f77c02e);

	size_t fileLength = AAsset_getLength(asset.get());
	Bytebuffer buffer(fileLength);

	_assert(AAsset_read(asset.get(), buffer.data(), fileLength) == fileLength, 0x1e49e9a9);

	return buffer;
}

} // namespace b2::android
