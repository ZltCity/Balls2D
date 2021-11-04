#include "render.hpp"

namespace b2::render
{

namespace detail
{

uint32_t b2ErrorCode(GLenum error)
{
	switch (error)
	{
		case GL_INVALID_ENUM: return 0x9800a19c;
		case GL_INVALID_VALUE: return 0x636e76a1;
		case GL_INVALID_OPERATION: return 0x02f3f490;
		case GL_OUT_OF_MEMORY: return 0x8e699b5d;
		default: return 0x0;
	}
}

size_t getTypeSize(VertexAttribute::Type type)
{
	switch (type)
	{
		case VertexAttribute::Float: return sizeof(float);
		default: return 0;
	}
}

template<>
void setUniform(const std::string &name, const int32_t &raw, const GLhandle &program)
{
	_i(glUniform1i, _i(glGetUniformLocation, GLuint(program), name.c_str()), raw);
}

template<>
void setUniform(const std::string &name, const float &raw, const GLhandle &program)
{
	_i(glUniform1f, _i(glGetUniformLocation, GLuint(program), name.c_str()), raw);
}

template<>
void setUniform(const std::string &name, const glm::vec2 &raw, const GLhandle &program)
{
	_i(glUniform2fv, _i(glGetUniformLocation, GLuint(program), name.c_str()), 1,
	   reinterpret_cast<const GLfloat *>(&raw));
}

template<>
void setUniform(const std::string &name, const glm::vec3 &raw, const GLhandle &program)
{
	_i(glUniform3fv, _i(glGetUniformLocation, GLuint(program), name.c_str()), 1,
	   reinterpret_cast<const GLfloat *>(&raw));
}

template<>
void setUniform(const std::string &name, const glm::vec4 &raw, const GLhandle &program)
{
	_i(glUniform4fv, _i(glGetUniformLocation, GLuint(program), name.c_str()), 1,
	   reinterpret_cast<const GLfloat *>(&raw));
}

template<>
void setUniform(const std::string &name, const glm::mat4 &raw, const GLhandle &program)
{
	_i(glUniformMatrix4fv, _i(glGetUniformLocation, GLuint(program), name.c_str()), 1, GL_FALSE,
	   reinterpret_cast<const GLfloat *>(&raw));
}

} // namespace detail

void BasicMesh::bind() const
{
	using namespace detail;

	size_t offset = 0;
	GLuint index = 0;

	_i(glBindBuffer, GL_ARRAY_BUFFER, GLuint(buffer));

	for (const auto &attribute : layout)
	{
		_i(glVertexAttribPointer, index, attribute.size, GLenum(attribute.type), GL_FALSE, attribute.stride,
		   reinterpret_cast<const void *>(offset));
		_i(glEnableVertexAttribArray, index);

		offset += attribute.size * getTypeSize(attribute.type);
		++index;
	}
}

void Uniform::set(const class Material &material) const
{
	using namespace detail;

	std::visit([this, &material](auto &&raw) { setUniform(name, raw, material.program); }, value);
}

Material::Material(const std::vector<Shader> &shaders, std::vector<Uniform> uniforms) : uniforms(std::move(uniforms))
{
	using namespace detail;

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
	using namespace detail;

	_i(glUseProgram, GLuint(program));

	for (const auto &uniform : uniforms)
		uniform.set(*this);
}

detail::GLhandle Material::loadShader(const Shader &shader)
{
	using namespace detail;

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
