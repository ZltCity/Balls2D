#include "window.h"

Window::Window(const std::string &title, int x, int y, int w, int h) {
  this->create(title, x, y, w, h);
}

bool Window::create(const std::string &title, int x, int y, int w, int h) {
  this->wPtr.reset(
    SDL_CreateWindow(title.c_str(), x, y, w, h, SDL_WINDOW_OPENGL),
    [](SDL_Window *w) {
      SDL_DestroyWindow(w);
    }
  );
  
  return this->wPtr.operator bool();
}

void Window::present() {
  if (this->wPtr)
    SDL_GL_SwapWindow(this->wPtr.get());
}

SDL_Window *Window::getHandle() const {
  return this->wPtr.get();
}
