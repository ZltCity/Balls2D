#include <ctime>
#include <cstdlib>
#include <random>

#include "application.h"
#include "gl_context.h"
#include "graphics.h"
#include "window.h"
#include "file.h"
#include "log.h"

#include "physics.h"

const int     SCREEN_WIDTH      = 950,
              SCREEN_HEIGHT     = 950,
              SCREEN_X          = 50,
              SCREEN_Y          = 50,
              GRID_WIDTH        = 100,
              GRID_HEIGHT       = 100,
              PARTICLES_COUNT   = 5000;

const float   G_CONST           = static_cast<float>(0.1f/*6.67e-11*/);

class MyApplication : public Application {
public:
  MyApplication()
    : physics(PARTICLES_COUNT, glm::uvec2(GRID_WIDTH, GRID_HEIGHT), 0.005f) {
  }

  void onStart() {
    this->init();
  }
  int onQuit() {
    return 0;
  }

  void onPreFrame()   {}
  void onPostFrame()  {}
  void onFrame() {
    glm::mat4 inverted  = glm::inverse(this->orthoProjection);
    glm::vec4 cursor    = inverted * glm::vec4(
      mousePosition.x / SCREEN_WIDTH * 2.0f - 1.0f, 
      mousePosition.y / SCREEN_HEIGHT * 2.0f - 1.0f, 0.0f, 1.0f);

    for (Particle &particle : this->physics.getParticles()) {
      particle.applyForce(glm::vec2(0.0f, -1.0f));

      if (!this->flags.lBtnPressed)
        continue;

      //
      glm::vec2 dist = glm::vec2(cursor) - particle.getPosition();

      if (glm::length(dist) > 10.0f)
        continue;

      particle.applyForce(glm::normalize(dist) * 50.0f);
    }

    for (int i = 0; i < 2; ++i)
      this->physics.update();

    writeVertexBuffer(this->glvertices, PARTICLES_COUNT, this->physics.getParticles().data(), GL_DYNAMIC_DRAW);
    //
    while (glGetError() != GL_NO_ERROR) {}

    this->glpreset.use();
    bindVertexBuffer(this->glvertices);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
    glEnableVertexAttribArray(0);

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, PARTICLES_COUNT);

    this->mainWnd.present();
  }

  void onMouseMove(const glm::vec2 &pos, bool relative) {
    if (!relative) {
      this->mousePosition.x = pos.x;
      this->mousePosition.y = SCREEN_HEIGHT - pos.y;
    }
  }

  void onButtonPressed(int button, const glm::vec2 &pos) {
    this->flags.lBtnPressed = true;
  }

  void onButtonReleased(int button, const glm::vec2 &pos) {
    this->flags.lBtnPressed = false;
  }

private:
  Window      mainWnd;
  GLContext   glContext;
  GLPreset    glpreset;
  GLResource  glvertices;
  Physics     physics;

  glm::mat4   orthoProjection;
  glm::vec2   mousePosition;

  struct {
    bool lBtnPressed;
  } flags;

  //std::vector<glm::ivec2> ps;

  void init() {
    flags.lBtnPressed = false;
    //
    this->createWidnow();
    this->createGLContext();
    this->loadResources();
    //
    std::random_device                rdev;
    std::uniform_real_distribution<>  dx(5.0f, static_cast<float>(GRID_WIDTH) - 5.0f),
                                      dy(5.0f, static_cast<float>(GRID_HEIGHT) - 5.0f);

    this->physics.init(
      [&](size_t) {
        return Particle(glm::vec2(dx(rdev), dy(rdev)));
      }
    );
        
    this->glvertices = createVertexBuffer<Particle>(0, nullptr, GL_DYNAMIC_DRAW);
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

    this->glpreset.setOrthoSize(glm::vec2(GRID_WIDTH, GRID_HEIGHT));
    this->glpreset.setScreenSize(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));

    this->orthoProjection = this->glpreset.projection();
  }
};

int main(int argc, char **argv) {
  return MyApplication().start(argc, argv);
}
