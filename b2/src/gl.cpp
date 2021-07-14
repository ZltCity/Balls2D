#include "gl.hpp"

namespace b2::gl
{

ClearMode operator|(ClearMode m1, ClearMode m2)
{
	return ClearMode(static_cast<int32_t>(m1) | static_cast<int32_t>(m2));
}

GLObject::GLObject(GLuint handle, std::function<void(GLuint)> destructor)
	: handle(new GLuint(handle), [destructor](GLuint *handle) {
		  destructor(*handle);
		  delete handle;
	  })
{}

GLObject::~GLObject()
{}

GLObject::operator bool() const
{
	return handle != nullptr && *handle != 0;
}

GLuint GLObject::getHandle() const
{
	return handle == nullptr ? 0 : *handle;
}

void Buffer::bind() const
{
	_assert(operator bool(), 0x13182aca);

	_i(glBindBuffer, static_cast<GLenum>(type), getHandle());
}

BufferType Buffer::getType() const
{
	_assert(operator bool(), 0x13182aca);

	return type;
}

size_t Buffer::getSize() const
{
	_assert(operator bool(), 0x13182aca);

	GLint size = 0;

	_i(glGetBufferParameteriv, static_cast<GLenum>(type), GL_BUFFER_SIZE, &size);

	return size;
}

void Texture::bind(int32_t unit) const
{
	_assert(operator bool(), 0x13182aca);

	_i(glActiveTexture, GL_TEXTURE0 + unit);
	_i(glBindTexture, static_cast<GLenum>(type), getHandle());
}

void Texture::setFilter(TextureType type, TextureFilter min, TextureFilter mag)
{
	GLenum target = static_cast<GLenum>(type);

	_i(glTexParameteri, target, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(min));
	_i(glTexParameteri, target, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(mag));
}

TextureType Texture::getType() const
{
	_assert(operator bool(), 0x13182aca);

	return type;
}

Shader::Shader(ShaderType type, const Bytebuffer &buffer)
	: GLObject(create(type, buffer), [](GLuint handle) { glDeleteShader(handle); })
{}

GLuint Shader::create(ShaderType type, const Bytebuffer &buffer)
{
	_assert(buffer.size() != 0, 0x461f5dfa);

	GLuint handle = _i(glCreateShader, static_cast<GLenum>(type));
	const GLchar *source = reinterpret_cast<const GLchar *>(buffer.data());
	const GLint length = static_cast<GLint>(buffer.size());

	_i(glShaderSource, handle, 1, &source, &length);
	_i(glCompileShader, handle);

	GLint status = GL_FALSE;

	_i(glGetShaderiv, handle, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		GLint logLength = 0;

		_i(glGetShaderiv, handle, GL_INFO_LOG_LENGTH, &logLength);

		std::string log(logLength + 1, 0);

		_i(glGetShaderInfoLog, handle, logLength, nullptr, reinterpret_cast<GLchar *>(log.data()));

		throw Exception(0x8909b556, log);
	}

	return handle;
}

ShaderProgram::ShaderProgram(const std::vector<Shader> &shaders)
	: GLObject(create(shaders), [](GLuint handle) { glDeleteProgram(handle); })
{}

GLuint ShaderProgram::create(const std::vector<Shader> &shaders)
{
	GLuint handle = _i(glCreateProgram);

	for (const Shader &shader : shaders)
	{
		_i(glAttachShader, handle, shader.getHandle());
	}

	_i(glLinkProgram, handle);

	GLint status = GL_FALSE;

	_i(glGetProgramiv, handle, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		GLint logLength = 0;

		_i(glGetProgramiv, handle, GL_INFO_LOG_LENGTH, &logLength);

		std::string log(logLength + 1, 0);

		_i(glGetProgramInfoLog, handle, logLength, nullptr, reinterpret_cast<GLchar *>(log.data()));

		throw Exception(0x1229cb91, log);
	}

	return handle;
}

void ShaderProgram::use() const
{
	_assert(operator bool(), 0x13182aca);

	_i(glUseProgram, getHandle());
}

Uniform::Uniform(const std::string &name) : name(name)
{}

Uniform::~Uniform()
{}

std::string Uniform::getName() const
{
	return name;
}

IntUniform::IntUniform(const std::string &name, int32_t value) : Uniform(name), value(value)
{}

void IntUniform::set(const ShaderProgram &program) const
{
	_assert(program, 0x13182aca);

	_i(glUniform1i, _i(glGetUniformLocation, program.getHandle(), getName().c_str()), value);
}

int32_t IntUniform::getValue() const
{
	return value;
}

FloatUniform::FloatUniform(const std::string &name, float value) : Uniform(name), value(value)
{}

void FloatUniform::set(const ShaderProgram &program) const
{
	_assert(program, 0x13182aca);

	_i(glUniform1f, _i(glGetUniformLocation, program.getHandle(), getName().c_str()), value);
}

float FloatUniform::getValue() const
{
	return value;
}

Vec2Uniform::Vec2Uniform(const std::string &name, const glm::vec2 &value) : Uniform(name), value(value)
{}

void Vec2Uniform::set(const ShaderProgram &program) const
{
	_assert(program, 0x13182aca);

	_i(glUniform2fv, _i(glGetUniformLocation, program.getHandle(), getName().c_str()), 1,
	   reinterpret_cast<const GLfloat *>(&value));
}

glm::vec2 Vec2Uniform::getValue()
{
	return value;
}

Vec3Uniform::Vec3Uniform(const std::string &name, const glm::vec3 &value) : Uniform(name), value(value)
{}

void Vec3Uniform::update(const glm::vec3 &value)
{
	this->value = value;
}

void Vec3Uniform::set(const ShaderProgram &program) const
{
	_assert(program, 0x13182aca);

	_i(glUniform3fv, _i(glGetUniformLocation, program.getHandle(), getName().c_str()), 1,
	   reinterpret_cast<const GLfloat *>(&value));
}

glm::vec3 Vec3Uniform::getValue()
{
	return value;
}

Vec4Uniform::Vec4Uniform(const std::string &name, const glm::vec4 &value) : Uniform(name), value(value)
{}

void Vec4Uniform::set(const ShaderProgram &program) const
{
	_assert(program, 0x13182aca);

	_i(glUniform4fv, _i(glGetUniformLocation, program.getHandle(), getName().c_str()), 1,
	   reinterpret_cast<const GLfloat *>(&value));
}

glm::vec4 Vec4Uniform::getValue()
{
	return value;
}

Mat4Uniform::Mat4Uniform(const std::string &name, const glm::mat4 &value) : Uniform(name), value(value)
{}

void Mat4Uniform::update(const glm::mat4 &value)
{
	this->value = value;
}

void Mat4Uniform::set(const ShaderProgram &program) const
{
	_assert(program, 0x13182aca);

	_i(glUniformMatrix4fv, _i(glGetUniformLocation, program.getHandle(), getName().c_str()), 1, GL_FALSE,
	   reinterpret_cast<const GLfloat *>(&value));
}

glm::mat4 Mat4Uniform::getValue()
{
	return value;
}

void clear(ClearMode mode)
{
	_i(glClear, static_cast<GLbitfield>(mode));
}

void enable(Feature feature)
{
	_i(glEnable, static_cast<GLenum>(feature));
}

void disable(Feature feature)
{
	_i(glDisable, static_cast<GLenum>(feature));
}

void draw(DrawMode mode, size_t count, size_t offset)
{
	_i(glDrawArrays, static_cast<GLenum>(mode), static_cast<GLint>(offset), count);
}

void setVertexFormat(const std::vector<VertexAttrib> &attrs)
{
	auto getTypeSize = [](AttribType type) -> size_t {
		switch (type)
		{
			case AttribType::Float: return sizeof(float);
		}

		return 0;
	};

	size_t offset = 0;
	GLuint index = 0;

	for (const VertexAttrib &attrib : attrs)
	{
		_i(glVertexAttribPointer, index, attrib.size, static_cast<GLenum>(attrib.type), GL_FALSE, attrib.stride,
		   reinterpret_cast<const void *>(offset));
		_i(glEnableVertexAttribArray, index);

		offset += attrib.size * getTypeSize(attrib.type);
		++index;
	}
}

void setBlendFunc(BlendFactor src, BlendFactor dst)
{
	_i(glBlendFunc, static_cast<GLenum>(src), static_cast<GLenum>(dst));
}

void setClearColor(const glm::vec4 &color)
{
	_i(glClearColor, color.r, color.g, color.b, color.a);
}

void clearGLerror()
{
	volatile GLenum error = GL_NO_ERROR;

	while ((error = glGetError()) != GL_NO_ERROR)
		;
}

int32_t appErrorCode(GLenum error)
{
	switch (error)
	{
		case GL_INVALID_ENUM: return 0x9800a19c;
		case GL_INVALID_VALUE: return 0x636e76a1;
		case GL_INVALID_OPERATION: return 0x02f3f490;
		case GL_OUT_OF_MEMORY: return 0x8e699b5d;
	}

	return 0x0;
}

} // namespace b2::gl
