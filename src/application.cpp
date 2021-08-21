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

int castFromSdlButton(int sdlButton);

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
        case SDL_MOUSEMOTION: {
          this->onMouseMove(glm::vec2(event.motion.x, event.motion.y));
          this->onMouseMove(glm::vec2(event.motion.xrel, event.motion.yrel), true);
          break;
        }
        case SDL_MOUSEBUTTONDOWN: {
          this->onButtonPressed(castFromSdlButton(event.button.button), glm::vec2(event.button.x, event.button.y));
          break;
        }
        case SDL_MOUSEBUTTONUP: {
          this->onButtonReleased(castFromSdlButton(event.button.button), glm::vec2(event.button.x, event.button.y));
          break;
        }
      }

    this->onFrame();
    this->onPostFrame();
  }

  return this->onQuit();
}

void Application::onMouseMove(const glm::vec2 &pos, bool relative) {}

void Application::onButtonPressed(int button, const glm::vec2 &pos) {}
void Application::onButtonReleased(int button, const glm::vec2 &pos) {}

int castFromSdlButton(int sdlButton) {
  switch (sdlButton) {
    case SDL_BUTTON_LEFT    : return Application::MOUSE_BUTTON_LEFT;
    case SDL_BUTTON_MIDDLE  : return Application::MOUSE_BUTTON_MIDDLE;
    case SDL_BUTTON_RIGHT   : return Application::MOUSE_BUTTON_RIGHT;
    case SDL_BUTTON_X1      : return Application::MOUSE_BUTTON_X1;
    case SDL_BUTTON_X2      : return Application::MOUSE_BUTTON_X2;
  }
}
