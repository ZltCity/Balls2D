#pragma once

#include <string>
#include <utility>

#include <GLES3/gl3.h>

#include <b2/logger.hpp>
#include <glm/glm.hpp>

namespace b2::render::backends::gles3
{

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

std::string toString(GLenum error);

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
			error(fmt::format("Error occurred on GL resource free: {}", ex.what()));
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

		if (error != GL_NO_ERROR)
			throw std::runtime_error(fmt::format("GLES3 error: {}.", toString(error)));
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

} // namespace b2::render::backends::gles3
