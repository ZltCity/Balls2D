#pragma once

#include <b2/platform/io.hpp>

namespace b2::desktop
{

class DesktopIO : public platform::IO
{
public:
	DesktopIO() = default;
	DesktopIO(const DesktopIO &) = delete;

	DesktopIO &operator=(const DesktopIO &) = delete;

	[[nodiscard]] Bytebuffer readFile(const std::string &filepath) const final;
};

} // namespace b2::desktop
