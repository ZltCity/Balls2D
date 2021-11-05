#include "material.hpp"

namespace b2::render
{

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

} // namespace b2::render
