#include "material.hpp"

namespace b2::render
{

template<class T>
void setUniform(const std::string &, const T &, const gles3::GLhandle &);

template<>
void setUniform(const std::string &name, const int32_t &raw, const gles3::GLhandle &program)
{
	using namespace gles3;

	_i(glUniform1i, _i(glGetUniformLocation, GLuint(program), name.c_str()), raw);
}

template<>
void setUniform(const std::string &name, const float &raw, const gles3::GLhandle &program)
{
	using namespace gles3;

	_i(glUniform1f, _i(glGetUniformLocation, GLuint(program), name.c_str()), raw);
}

template<>
void setUniform(const std::string &name, const glm::vec2 &raw, const gles3::GLhandle &program)
{
	using namespace gles3;

	_i(glUniform2fv, _i(glGetUniformLocation, GLuint(program), name.c_str()), 1,
	   reinterpret_cast<const GLfloat *>(&raw));
}

template<>
void setUniform(const std::string &name, const glm::vec3 &raw, const gles3::GLhandle &program)
{
	using namespace gles3;

	_i(glUniform3fv, _i(glGetUniformLocation, GLuint(program), name.c_str()), 1,
	   reinterpret_cast<const GLfloat *>(&raw));
}

template<>
void setUniform(const std::string &name, const glm::vec4 &raw, const gles3::GLhandle &program)
{
	using namespace gles3;

	_i(glUniform4fv, _i(glGetUniformLocation, GLuint(program), name.c_str()), 1,
	   reinterpret_cast<const GLfloat *>(&raw));
}

template<>
void setUniform(const std::string &name, const glm::mat4 &raw, const gles3::GLhandle &program)
{
	using namespace gles3;

	_i(glUniformMatrix4fv, _i(glGetUniformLocation, GLuint(program), name.c_str()), 1, GL_FALSE,
	   reinterpret_cast<const GLfloat *>(&raw));
}

void Uniform::set(const class Material &material) const
{
	std::visit([this, &material](auto &&raw) { setUniform(name, raw, material.program); }, value);
}

} // namespace b2::render
