#include "config.hpp"

namespace b2
{

const char *const Config::tag = "BlueWater2";

Config::Config(const Bytebuffer &buffer) : json(nlohmann::json::parse(buffer))
{}

Config::operator Bytebuffer() const
{
	std::string dump(json.dump());

	return Bytebuffer(dump.begin(), dump.end());
}

} // namespace b2
