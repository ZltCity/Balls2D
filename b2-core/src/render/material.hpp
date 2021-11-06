#pragma once

#include <filesystem>

#include <b2/bytebuffer.hpp>

#include "backends/gles3.hpp"
#include "cache.hpp"
#include "uniform.hpp"

namespace b2::render
{

using namespace backends;

struct Shader
{
	enum class Type
	{
		Vertex = GL_VERTEX_SHADER,
		Fragment = GL_FRAGMENT_SHADER
	};

	Bytebuffer source;
	Type type;
};

class Material
{
	friend class Uniform;

public:
	Material() = default;
	Material(const std::vector<Shader> &shaders, std::vector<Uniform> uniforms);
	Material(const Material &) = delete;
	Material(Material &&other) noexcept = default;

	Material &operator=(const Material &) = delete;
	Material &operator=(Material &&other) noexcept = default;

	void bind() const;

private:
	[[nodiscard]] static gles3::GLhandle loadShader(const Shader &shader);

	gles3::GLhandle program;
	std::vector<Uniform> uniforms;
};

Cache<Material> loadMaterials(const std::filesystem::path &materialsRoot);

} // namespace b2::render
