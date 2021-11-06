#pragma once

#include <string>
#include <variant>

#include <glm/glm.hpp>

#include "backends/gles3.hpp"

namespace b2::render
{

using namespace backends;

class Uniform
{
public:
	using Value = std::variant<int32_t, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

	Uniform() = default;
	template<class T>
	Uniform(std::string name, const T &value) noexcept;

	void set(const class Material &raw) const;

private:
	std::string name;
	Value value;
};

template<class T>
Uniform::Uniform(std::string name, const T &value) noexcept : name(std::move(name)), value(value)
{}

} // namespace b2::render
