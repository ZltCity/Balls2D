#pragma once

#include "backends/gles3.hpp"

namespace b2::render
{

using namespace backends;

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
	BasicMesh(const std::vector<VertexT> &vertices, std::vector<VertexAttribute> layout, Usage usage = StaticDraw);
	BasicMesh(const BasicMesh &) = delete;
	BasicMesh(BasicMesh &&other) noexcept = default;

	virtual ~BasicMesh() = default;

	BasicMesh &operator=(const BasicMesh &) = delete;
	BasicMesh &operator=(BasicMesh &&other) noexcept = default;

	template<class VertexT>
	void update(const std::vector<VertexT> &vertices, size_t offset = 0);

	virtual void bind() const;

private:
	gles3::GLhandle buffer;
	std::vector<VertexAttribute> layout;
};

class IndexedMesh : public BasicMesh
{};

template<class VertexT>
BasicMesh::BasicMesh(const std::vector<VertexT> &vertices, std::vector<VertexAttribute> layout, Usage usage)
	: layout(std::move(layout))
{
	using namespace gles3;

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
	using namespace gles3;

	_i(glBindBuffer, GL_ARRAY_BUFFER, GLuint(buffer));
	_i(glBufferSubData, GL_ARRAY_BUFFER, offset, vertices.size() * sizeof(VertexT), vertices.data());
	_i(glBindBuffer, GL_ARRAY_BUFFER, 0);
}

} // namespace b2::render
