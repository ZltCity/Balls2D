#include <fmt/format.h>

#include <nlohmann/json.hpp>

#include "../utils.hpp"
#include "material.hpp"

#include "../logger.hpp"

namespace b2::render
{

std::vector<render::Shader> parseShaders(const nlohmann::json &meta, const std::filesystem::path &materialsRoot);
std::vector<render::Uniform> parseUniforms(const nlohmann::json &meta);

Material::Material(const std::vector<Shader> &shaders, std::vector<Uniform> uniforms) : uniforms(std::move(uniforms))
{
	using namespace gles3;

	program = GLhandle(_i(glCreateProgram), [](GLuint id) { _i(glDeleteProgram, id); });

	for (const auto &shader : shaders)
		_i(glAttachShader, GLuint(program), GLuint(loadShader(shader)));

	GLint status = GL_FALSE;

	_i(glLinkProgram, GLuint(program));
	_i(glGetProgramiv, GLuint(program), GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		GLint logLength = 0;
		std::string log;

		_i(glGetProgramiv, GLuint(program), GL_INFO_LOG_LENGTH, &logLength);
		log.resize(logLength + 1, 0);
		_i(glGetProgramInfoLog, GLuint(program), logLength, nullptr, reinterpret_cast<GLchar *>(log.data()));

		throw Exception(0x1229cb91, log);
	}
}

void Material::bind() const
{
	using namespace gles3;

	_i(glUseProgram, GLuint(program));

	for (const auto &uniform : uniforms)
		uniform.set(*this);
}

gles3::GLhandle Material::loadShader(const Shader &shader)
{
	using namespace gles3;

	_assert(!shader.source.empty(), 0x461f5dfa);

	GLhandle handle(_i(glCreateShader, GLenum(shader.type)), [](GLuint id) { _i(glDeleteShader, id); });
	auto source = reinterpret_cast<const GLchar *>(shader.source.data());
	const auto length = GLint(shader.source.size());
	GLint status = GL_FALSE;

	_i(glShaderSource, GLuint(handle), 1, &source, &length);
	_i(glCompileShader, GLuint(handle));
	_i(glGetShaderiv, GLuint(handle), GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		GLint logLength = 0;
		std::string log;

		_i(glGetShaderiv, GLuint(handle), GL_INFO_LOG_LENGTH, &logLength);
		log.resize(logLength + 1, 0);
		_i(glGetShaderInfoLog, GLuint(handle), logLength, nullptr, reinterpret_cast<GLchar *>(log.data()));

		throw Exception(0x8909b556, log);
	}

	return handle;
}

Cache<Material> loadMaterials(const std::filesystem::path &materialsRoot)
{
	namespace fs = std::filesystem;

	auto cache = Cache<Material> {};

	for (const auto &entry : fs::directory_iterator(materialsRoot, fs::directory_options::follow_directory_symlink))
	{
		const auto &entryPath = entry.path();

		if (entryPath.extension() != ".json")
			continue;

		auto meta = nlohmann::json::parse(readFile(entryPath));

		cache.put(meta["id"], {parseShaders(meta["shaders"], materialsRoot), parseUniforms(meta["constants"])});
	}

	return cache;
}

std::vector<render::Shader> parseShaders(const nlohmann::json &meta, const std::filesystem::path &materialsRoot)
{
	namespace fs = std::filesystem;

	auto shaders = std::vector<Shader> {};

	for (const auto &shader : meta)
	{
		const auto shaderPath = fs::path(shader["path"]);
		const auto extension = shaderPath.extension().string();
		auto shaderType = render::Shader::Type {};

		if (extension == ".vert")
			shaderType = render::Shader::Type::Vertex;
		else if (extension == ".frag")
			shaderType = render::Shader::Type::Fragment;
		else
		{
			warn("%s", fmt::format("Unknown shader file extension '{}'.", extension).c_str());
			continue;
		}

		shaders.emplace_back(readFile(materialsRoot / "shaders" / shaderPath), shaderType);
	}

	return shaders;
}

std::vector<render::Uniform> parseUniforms(const nlohmann::json &meta)
{
	auto uniforms = std::vector<Uniform> {};

	for (const auto &constant : meta)
	{
		const auto id = constant["id"];
		auto uniform = Uniform {};

		switch (constant["value"].type())
		{
			case nlohmann::json::value_t::number_float:
			{
				uniform = Uniform(id, constant["value"].get<float>());
				break;
			}
		}

		uniforms.emplace_back(uniform);
	}

	return uniforms;
}

} // namespace b2::render
