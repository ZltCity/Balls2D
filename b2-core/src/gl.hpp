#pragma once

#include <functional>
#include <memory>
#include <string>

#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <b2/bytebuffer.hpp>
#include <b2/exception.hpp>

namespace b2::gl
{

enum class BufferType
{
	Vertex = GL_ARRAY_BUFFER,
	Index = GL_ELEMENT_ARRAY_BUFFER
};

enum class BufferUsage
{
	Stream = GL_STREAM_DRAW,
	Static = GL_STATIC_DRAW,
	Dynamic = GL_DYNAMIC_DRAW
};

enum class TextureType
{
	Texture2D = GL_TEXTURE_2D,
	Texture3D = GL_TEXTURE_3D
};

enum class TextureFormat
{
	Red8 = GL_RED,
	RGB = GL_RGB,
	RGBA = GL_RGBA
};

enum class TextureFilter
{
	Nearest = GL_NEAREST,
	Linear = GL_LINEAR
};

enum class TexelType
{
	Byte = GL_UNSIGNED_BYTE,
	Float = GL_FLOAT
};

enum class ShaderType
{
	Vertex = GL_VERTEX_SHADER,
	Fragment = GL_FRAGMENT_SHADER
};

enum class ClearMode
{
	Color = GL_COLOR_BUFFER_BIT,
	Depth = GL_DEPTH_BUFFER_BIT
};

ClearMode operator|(ClearMode m1, ClearMode m2);

enum class Feature
{
	DepthTest = GL_DEPTH_TEST,
	Blend = GL_BLEND
};

enum class BlendFactor
{
	SrcAlpha = GL_SRC_ALPHA,
	OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA
};

enum class DrawMode
{
	Points = GL_POINTS,
	Triangles = GL_TRIANGLES
};

enum class AttribType
{
	Float = GL_FLOAT
};

struct VertexAttrib
{
	int32_t size, stride;
	AttribType type;
};

class GLObject
{
public:
	GLObject() = default;
	GLObject(GLuint handle, std::function<void(GLuint)> destructor);
	virtual ~GLObject() = 0;

	operator bool() const;

	GLuint getHandle() const;

private:
	std::shared_ptr<GLuint> handle;
};

class Buffer : public GLObject
{
public:
	Buffer() = default;
	template<typename Primitive>
	Buffer(BufferType type, const std::vector<Primitive> &buffer = {}, BufferUsage usage = BufferUsage::Static);

	void bind() const;
	template<typename Primitive>
	void write(size_t offset, const std::vector<Primitive> &buffer);

	BufferType getType() const;
	size_t getSize() const;

private:
	template<typename Primitive>
	GLuint create(BufferType type, const std::vector<Primitive> &buffer, BufferUsage usage);

	BufferType type;
};

class Texture : public GLObject
{
public:
	Texture() = default;
	template<typename Texel, typename Size>
	Texture(
		TextureType type, TextureFormat format, TexelType texelType, const Size &size,
		const std::vector<Texel> &buffer);

	template<typename Texel, typename Size>
	void write(const Size &offset, const Size &size, const std::vector<Texel> &buffer);
	void bind(int32_t unit = 0) const;

	static void setFilter(TextureType type, TextureFilter min, TextureFilter mag);

	TextureType getType() const;

private:
	template<typename Texel, typename Size>
	GLuint create(
		TextureType type, TextureFormat format, TexelType texelType, const Size &size,
		const std::vector<Texel> &buffer);

	TextureType type;
	TextureFormat format;
	TexelType texelType;
};

class Shader : public GLObject
{
public:
	Shader() = default;
	Shader(ShaderType type, const Bytebuffer &buffer);

private:
	GLuint create(ShaderType type, const Bytebuffer &buffer);
};

class ShaderProgram : public GLObject
{
public:
	ShaderProgram() = default;
	ShaderProgram(const std::vector<Shader> &shaders);

	void use() const;

private:
	GLuint create(const std::vector<Shader> &shaders);
};

class Uniform
{
public:
	Uniform(const std::string &name);
	virtual ~Uniform() = 0;

	virtual void set(const ShaderProgram &) const = 0;

	std::string getName() const;

private:
	std::string name;
};

class IntUniform final : public Uniform
{
public:
	IntUniform(const std::string &name, int32_t value);

	void set(const ShaderProgram &program) const;

	int32_t getValue() const;

private:
	int32_t value;
};

class FloatUniform final : public Uniform
{
public:
	FloatUniform(const std::string &name, float value);

	void set(const ShaderProgram &program) const;

	float getValue() const;

private:
	float value;
};

class Vec2Uniform final : public Uniform
{
public:
	Vec2Uniform(const std::string &name, const glm::vec2 &value);

	void set(const ShaderProgram &program) const;

	glm::vec2 getValue();

private:
	glm::vec2 value;
};

class Vec3Uniform final : public Uniform
{
public:
	Vec3Uniform(const std::string &name, const glm::vec3 &value);

	void update(const glm::vec3 &value);
	void set(const ShaderProgram &program) const;

	glm::vec3 getValue();

private:
	glm::vec3 value;
};

class Vec4Uniform final : public Uniform
{
public:
	Vec4Uniform(const std::string &name, const glm::vec4 &value);

	void set(const ShaderProgram &program) const;

	glm::vec4 getValue();

private:
	glm::vec4 value;
};

class Mat4Uniform final : public Uniform
{
public:
	Mat4Uniform(const std::string &name, const glm::mat4 &value);

	void update(const glm::mat4 &value);
	void set(const ShaderProgram &program) const;

	glm::mat4 getValue();

private:
	glm::mat4 value;
};

void clear(ClearMode mode);
void enable(Feature feature);
void disable(Feature feature);
void draw(DrawMode mode, size_t count, size_t offset = 0);

void setVertexFormat(const std::vector<VertexAttrib> &attrs);
void setBlendFunc(BlendFactor src, BlendFactor dst);
void setClearColor(const glm::vec4 &color);

void clearGLerror();

int32_t appErrorCode(GLenum error);

template<typename F, typename... Args>
auto _i(F f, Args... args) -> typename std::invoke_result<F, Args...>::type;

template<typename Primitive>
Buffer::Buffer(BufferType type, const std::vector<Primitive> &buffer, BufferUsage usage)
	: GLObject(create(type, buffer, usage), [](GLuint handle) { glDeleteBuffers(1, &handle); }), type(type)
{}

template<typename Primitive>
GLuint Buffer::create(BufferType type, const std::vector<Primitive> &buffer, BufferUsage usage)
{
	size_t bufferSize = buffer.size();
	GLuint handle = 0;
	GLenum target = static_cast<GLenum>(type);

	_i(glGenBuffers, 1, &handle);
	_i(glBindBuffer, target, handle);

	if (bufferSize > 0)
		_i(glBufferData, target, bufferSize * sizeof(Primitive), buffer.data(), static_cast<GLenum>(usage));

	_i(glBindBuffer, target, 0);

	return handle;
}

template<typename Texel, typename Size>
Texture::Texture(
	TextureType type, TextureFormat format, TexelType texelType, const Size &size, const std::vector<Texel> &buffer)
	: GLObject(create(type, format, texelType, size, buffer), [](GLuint handle) { glDeleteTextures(1, &handle); }),
	  type(type),
	  format(format),
	  texelType(texelType)
{}

template<typename Texel, typename Size>
GLuint Texture::create(
	TextureType type, TextureFormat format, TexelType texelType, const Size &size, const std::vector<Texel> &buffer)
{
	GLuint handle = 0;
	GLenum target = static_cast<GLenum>(type);
	GLenum internal;

	switch (format)
	{
		case TextureFormat::Red8: internal = GL_R8; break;
		case TextureFormat::RGB:
		case TextureFormat::RGBA: internal = static_cast<GLenum>(format); break;

		default: _assert(false, 0xa2f0db57);
	}

	_i(glGenTextures, 1, &handle);
	_i(glBindTexture, target, handle);

	if (buffer.size() > 0)
		switch (type)
		{
			case TextureType::Texture2D:
			{
				_i(glTexImage2D, target, 0, internal, size.x, size.y, 0, static_cast<GLenum>(format),
				   static_cast<GLenum>(texelType), buffer.data());
				break;
			}
			case TextureType::Texture3D:
			{
				_i(glTexImage3D, target, 0, internal, size.x, size.y, size.z, 0, static_cast<GLenum>(format),
				   static_cast<GLenum>(texelType), buffer.data());
				break;
			}
		}

	_i(glBindTexture, target, 0);

	return handle;
}

template<typename Primitive>
void Buffer::write(size_t offset, const std::vector<Primitive> &buffer)
{
	_assert(operator bool(), 0x13182aca);

	_i(glBufferSubData, static_cast<GLenum>(type), offset, buffer.size() * sizeof(Primitive), buffer.data());
}

template<typename Texel, typename Size>
void Texture::write(const Size &offset, const Size &size, const std::vector<Texel> &buffer)
{
	_assert(operator bool(), 0x13182aca);

	switch (type)
	{
		case TextureType::Texture2D:
		{
			_i(glTexSubImage2D, static_cast<GLenum>(type), 0, offset.x, offset.y, size.x, size.y,
			   static_cast<GLenum>(format), static_cast<GLenum>(texelType), buffer.data());
			break;
		}
		case TextureType::Texture3D:
		{
			_i(glTexSubImage3D, static_cast<GLenum>(type), 0, offset.x, offset.y, offset.z, size.x, size.y, size.z,
			   static_cast<GLenum>(format), static_cast<GLenum>(texelType), buffer.data());
			break;
		}
	}
}

template<typename F, typename... Args>
auto _i(F f, Args... args) -> typename std::invoke_result<F, Args...>::type
{
	using Result = typename std::invoke_result<F, Args...>::type;

	auto _glassert = []() {
		GLenum error = glGetError();

		_assert(error == GL_NO_ERROR, appErrorCode(error));
	};

	if constexpr (std::is_void_v<Result>)
	{
		f(args...);
		_glassert();
	}
	else
	{
		Result result = f(args...);

		_glassert();

		return result;
	}
}

} // namespace b2-core::gl
