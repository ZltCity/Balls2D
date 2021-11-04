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

} // namespace detail

BasicMesh::BasicMesh(BasicMesh &&other) noexcept
{
	std::swap(buffer, other.buffer);
	std::swap(layout, other.layout);
}

BasicMesh &BasicMesh::operator=(BasicMesh &&other) noexcept
{
	buffer = std::move(other.buffer);
	layout = std::move(other.layout);

	return *this;
}

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

} // namespace b2::render
