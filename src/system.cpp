#include "system.hpp"
#include "exception.hpp"

namespace b2
{

System::System(AAssetManager *assetManager) : assetManager(assetManager)
{}

Bytebuffer System::readFile(const char *filepath) const
{
	std::unique_ptr<AAsset, std::function<void(AAsset *)>> asset(
		AAssetManager_open(assetManager, filepath, AASSET_MODE_BUFFER), [](AAsset *asset) { AAsset_close(asset); });

	_assert(asset != nullptr, 0x0f77c02e);

	size_t fileLength = AAsset_getLength(asset.get());
	Bytebuffer buffer(fileLength);

	_assert(AAsset_read(asset.get(), buffer.data(), fileLength) == fileLength, 0x1e49e9a9);

	return buffer;
}

} // namespace b2
