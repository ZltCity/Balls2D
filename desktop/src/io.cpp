#include <fstream>

#include "io.hpp"

namespace b2::desktop
{

Bytebuffer DesktopIO::readFile(const std::string &filepath) const
{
	std::fstream stream(filepath, std::fstream::in | std::fstream::binary);

	if (!stream.is_open())
		throw std::runtime_error("Unable to open file.");

	stream.seekg(0, std::fstream::end);

	auto size = size_t(stream.tellg());
	auto buffer = Bytebuffer(size);

	stream.seekg(0, std::fstream::beg);
	stream.read(reinterpret_cast<char *>(buffer.data()), size);

	return buffer;
}

} // namespace b2::desktop
