#include "gles3.hpp"

namespace b2::render::backends::gles3
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

} // namespace b2::render::backends::gles3
