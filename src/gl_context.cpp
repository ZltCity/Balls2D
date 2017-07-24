#include "gl_context.h"

GLContext::GLContext(const Window &w) {
  this->create(w);
}

bool GLContext::create(const Window &w) {
  this->cPtr.reset(
    SDL_GL_CreateContext(w.getHandle()),
    [](void *c) {
      SDL_GL_DeleteContext(c);
    }
  );

  if (this->cPtr) {
    ogl_LoadFunctions();

    return true;
  }
  else
    return false;
}

void GLContext::makeCurrent(const Window &w) {
  if (this->cPtr)
    SDL_GL_MakeCurrent(w.getHandle(), this->cPtr.get());
}
