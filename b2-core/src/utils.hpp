#pragma once

#include <filesystem>

#include <b2/bytebuffer.hpp>

namespace b2
{

[[nodiscard]] Bytebuffer readFile(const std::filesystem::path &path);

}
