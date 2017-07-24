#include "application.h"
#include "gl_context.h"
#include "window.h"
#include "log.h"

class MyApplication : public Application {
public:
  MyApplication() {}

  void onStart() {
    Log &log = Log::instance();

    if (this->w.create(std::string("SDL test"), 60, 60, 640, 480))
      log.print(LOG_INFO, std::string("Main window creation succeed."));
    else {
      log.print(LOG_CRIT, std::string("Main window creation failed."));
      std::exit(EXIT_FAILURE);
    }

    if (this->c.create(this->w))
      log.print(LOG_INFO, std::string("OpenGL context creation succeed."));
    else {
      log.print(LOG_CRIT, std::string("OpenGL context creation failed."));
      std::exit(EXIT_FAILURE);
    }
  }
  int onQuit() {
    return 0;
  }

  void onPreFrame()   {}
  void onPostFrame()  {}
  void onFrame() {
    glClearColor(0.3f, 0.6f, 0.45f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    this->w.present();
  }

private:
  Window    w;
  GLContext c;
};

int main(int argc, char **argv) {
  return MyApplication().start(argc, argv);
}
