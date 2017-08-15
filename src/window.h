#pragma once

#include <memory>
#include <string>
#include <SDL.h>

class Window {
public:
  friend class GLContext;
  friend class Renderer2D;

  Window() = default;
  Window(const std::string &title, int x, int y, int w, int h);

  bool create(const std::string &title, int x, int y, int w, int h);
  void present();

  void setCaption(const std::string &title);

protected:
  SDL_Window *getHandle() const;

private:
  std::shared_ptr<SDL_Window> wPtr;
};
