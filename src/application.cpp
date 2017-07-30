#include <cstdlib>
#include <SDL.h>

#include "log.h"
#include "application.h"

class __InitHelper {
public:
  __InitHelper() {
    Log &log = Log::instance();

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
      log.print(LOG_CRIT, std::string("SDL init failed."));
      std::exit(EXIT_FAILURE);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,  /*SDL_GL_CONTEXT_PROFILE_CORE*/SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    log.print(LOG_INFO, std::string("SDL init succeed."));
  }

  ~__InitHelper() {
    SDL_Quit();
  }

};

static __InitHelper __initHelper;

Application::Application()
{}

Application::~Application()
{}

int Application::start(int argc, char **argv)
{
  this->onStart();

  SDL_Event event;
  bool      quit = false;

  while (!quit) {
    this->onPreFrame();

    while (SDL_PollEvent(&event))
      switch (event.type) {
        case SDL_QUIT: {
          quit = true;
          break;
        }
      }

    this->onFrame();
    this->onPostFrame();
  }

  return this->onQuit();
}

glm::uvec2 Application::mousePosition() const {
  int x, y;

  SDL_GetMouseState(&x, &y);

  return glm::uvec2(x, y);
}
