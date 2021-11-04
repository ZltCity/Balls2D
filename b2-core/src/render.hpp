#pragma once

#include <memory>
#include <vector>

#include <GLES3/gl3.h>

#include <b2/exception.hpp>

#include "logger.hpp"

namespace b2::render
{

namespace detail
{

uint32_t b2ErrorCode(GLenum error);

class GLhandle
{
public:
	inline GLhandle() noexcept;
	inline GLhandle(GLuint handle, void (*deleter)(GLuint)) noexcept;
	GLhandle(const GLhandle &) = delete;
	inline GLhandle(GLhandle &&other) noexcept;
	inline ~GLhandle();

	GLhandle &operator=(const GLhandle &) = delete;
	inline GLhandle &operator=(GLhandle &&other) noexcept;

	explicit inline operator GLuint() const;

private:
	inline void release();

	GLuint handle;
	void (*deleter)(GLuint);
};

} // namespace detail

struct VertexAttribute
{
	enum Type
	{
		Float = GL_FLOAT
	};

	int32_t size, stride;
	Type type;
};

class BasicMesh
{
public:
	enum Usage
	{
		StaticDraw = GL_STATIC_DRAW,
		DynamicDraw = GL_DYNAMIC_DRAW
	};

	BasicMesh() = default;
	template<class VertexT>
	BasicMesh(
		const std::vector<VertexT> &vertices, const std::vector<VertexAttribute> &layout, Usage usage = StaticDraw);
	BasicMesh(const BasicMesh &) = delete;
	BasicMesh(BasicMesh &&other) noexcept;

	virtual ~BasicMesh() = default;

	BasicMesh &operator=(const BasicMesh &) = delete;
	BasicMesh &operator=(BasicMesh &&other) noexcept;

	template<class VertexT>
	void update(const std::vector<VertexT> &vertices, size_t offset = 0);

	virtual void bind() const;

private:
	detail::GLhandle buffer;
	std::vector<VertexAttribute> layout;
};

class IndexedMesh : public BasicMesh
{};

class Material
{};

namespace detail
{

size_t getTypeSize(VertexAttribute::Type type);

GLhandle::GLhandle() noexcept : handle(0), deleter(nullptr)
{}

GLhandle::GLhandle(GLuint handle, void (*deleter)(GLuint)) noexcept : handle(handle), deleter(deleter)
{}

GLhandle::GLhandle(GLhandle &&other) noexcept : handle(0), deleter(nullptr)
{
	std::swap(handle, other.handle);
	std::swap(deleter, other.deleter);
}

GLhandle::~GLhandle()
{
	release();
}

GLhandle &GLhandle::operator=(GLhandle &&other) noexcept
{
	if (this != &other)
	{
		release();
		std::swap(handle, other.handle);
		std::swap(deleter, other.deleter);
	}

	return *this;
}

GLhandle::operator GLuint() const
{
	return handle;
}

void GLhandle::release()
{
	if (handle && deleter)
		try
		{
			deleter(handle);
		}
		catch (const std::exception &ex)
		{
			crit("Error occurred on GL resource free: ", ex.what());
		}

	handle = 0;
	deleter = nullptr;
}

template<typename F, typename... Args>
auto _i(F f, Args... args) -> typename std::invoke_result<F, Args...>::type
{
	using Result = typename std::invoke_result<F, Args...>::type;

	auto _glassert = []() {
		GLenum error = glGetError();

		_assert(error == GL_NO_ERROR, b2ErrorCode(error));
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

} // namespace detail

template<class VertexT>
BasicMesh::BasicMesh(const std::vector<VertexT> &vertices, const std::vector<VertexAttribute> &layout, Usage usage)
	: layout(layout)
{
	using namespace detail;

	GLuint id = 0;

	_i(glGenBuffers, 1, &id);

	buffer = GLhandle(id, [](GLuint id) { _i(glDeleteBuffers, 1, &id); });

	_i(glBindBuffer, GL_ARRAY_BUFFER, GLuint(buffer));
	_i(glBufferData, GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexT), vertices.data(), GLenum(usage));
	_i(glBindBuffer, GL_ARRAY_BUFFER, 0);
}

template<class VertexT>
void BasicMesh::update(const std::vector<VertexT> &vertices, size_t offset)
{
	using namespace detail;

	_i(glBindBuffer, GL_ARRAY_BUFFER, GLuint(buffer));
	_i(glBufferSubData, GL_ARRAY_BUFFER, offset, vertices.size() * sizeof(VertexT), vertices.data());
	_i(glBindBuffer, GL_ARRAY_BUFFER, 0);
}

} // namespace b2::render
