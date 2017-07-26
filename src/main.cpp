#include <ctime>
#include <cstdlib>

#include "application.h"
#include "gl_context.h"
#include "graphics.h"
#include "window.h"
#include "file.h"
#include "log.h"

#include "particle.h"

const int SCREEN_WIDTH  = 800,
          SCREEN_HEIGHT = 800,
          SCREEN_X      = 50,
          SCREEN_Y      = 50;

class MyApplication : public Application {
public:
  MyApplication() {}

  void onStart() {
    this->init();
  }
  int onQuit() {
    return 0;
  }

  void onPreFrame()   {}
  void onPostFrame()  {}
  void onFrame() {
    glm::vec4 c = this->glpreset.getColor();

    glClearColor(c.r, c.g, c.b, c.a);
    glClear(GL_COLOR_BUFFER_BIT);

    this->mainWnd.present();
  }

private:
  Window      mainWnd;
  GLContext   glContext;
  GLPreset    glpreset;

  //std::vector<glm::ivec2> ps;

  void init() {
    this->createWidnow();
    this->createGLContext();
    this->loadResources();
    //
    /*std::srand(static_cast<unsigned int>(std::time(nullptr)));

    for (int i = 0; i < 5000; ++i) {
      this->ps.push_back(glm::ivec2(std::rand() % SCREEN_WIDTH, std::rand() % SCREEN_HEIGHT));
    }*/
  }

  void free() {
  }

  void createWidnow() {
    Log &log = Log::instance();

    if (this->mainWnd.create(std::string("Some shitty physics, v1.0"), SCREEN_X, SCREEN_Y, SCREEN_WIDTH, SCREEN_HEIGHT))
      log.print(LOG_INFO, std::string("Main window creation succeed."));
    else {
      log.print(LOG_CRIT, std::string("Main window creation failed."));
      std::exit(EXIT_FAILURE);
    }
  }

  void createGLContext() {
    Log &log = Log::instance();

    if (this->glContext.create(this->mainWnd))
      log.print(LOG_INFO, std::string("OpenGL context creation succeed."));
    else {
      log.print(LOG_CRIT, std::string("OpenGL context creation failed."));
      std::exit(EXIT_FAILURE);
    }
  }

  void loadResources()  {
    Log &log = Log::instance();

    if (this->glpreset.loadFromFile(std::string("preset.json")))
      log.print(LOG_INFO, std::string("OpenGL preset loading succeed."));
    else {
      log.print(LOG_CRIT, std::string("OpenGL preset loading failed."));
      std::exit(EXIT_FAILURE);
    }
  }
};

int main(int argc, char **argv) {
  return MyApplication().start(argc, argv);
}
