#include <fstream>

#include <fmt/format.h>

#include "utils.hpp"

namespace b2
{

Bytebuffer readFile(const std::filesystem::path &path)
{
	std::fstream stream(path, std::ios::in | std::ios::binary);

	if (!stream.is_open())
		throw std::runtime_error(fmt::format("Unable to open file '{}'.", path.string()));

	stream.seekg(0, std::fstream::end);

	auto size = size_t(stream.tellg());
	auto buffer = Bytebuffer(size);

	stream.seekg(0, std::fstream::beg);
	stream.read(reinterpret_cast<char *>(buffer.data()), static_cast<std::streamsize>(size));

	return buffer;
}

} // namespace b2
