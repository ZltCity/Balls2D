#include "mesh.hpp"

namespace b2::render
{

size_t getTypeSize(VertexAttribute::Type type);

void BasicMesh::bind() const
{
	using namespace gles3;

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

size_t getTypeSize(VertexAttribute::Type type)
{
	switch (type)
	{
		case VertexAttribute::Float: return sizeof(float);
		default: return 0;
	}
}

} // namespace b2::render
