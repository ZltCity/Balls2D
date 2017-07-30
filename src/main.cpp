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
              PARTICLES_COUNT   = 5000;

const float   GRID_WIDTH        = 100.f,
              GRID_HEIGHT       = 100.f,
              G_CONST           = static_cast<float>(0.1f/*6.67e-11*/);

class MyApplication : public Application {
public:
  MyApplication()
    : physics(PARTICLES_COUNT, glm::vec2(GRID_WIDTH, GRID_HEIGHT)) {
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
    glm::uvec2 mousePosition = this->mousePosition(); 

    mousePosition.y = SCREEN_HEIGHT - mousePosition.y;

    for (Particle &particle : this->physics.particles()) {
      particle.applyForce(glm::vec2(0.0f, -1.0f));
      //
      glm::vec4   transformed = this->orthoProjection * glm::vec4(particle.position(), 0.0f, 1.0f);
      glm::vec2   windowed    = glm::vec2((transformed.x + 1.0f) * SCREEN_WIDTH * 0.5f, (transformed.y + 1.0f) * SCREEN_HEIGHT * 0.5f),
                  dist        = glm::vec2(mousePosition) - windowed;

      if (glm::length(dist) > 80)
        continue;

      particle.applyForce(dist * 2.0f);
    }

    this->physics.update(.02f);
    writeVertexBuffer(this->glvertices, PARTICLES_COUNT, this->physics.particles().data(), GL_DYNAMIC_DRAW);
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

private:
  Window      mainWnd;
  GLContext   glContext;
  GLPreset    glpreset;
  GLResource  glvertices;
  Physics     physics;

  glm::mat4   orthoProjection;

  //std::vector<glm::ivec2> ps;

  void init() {
    this->createWidnow();
    this->createGLContext();
    this->loadResources();
    //
    float halfWidth  = GRID_WIDTH  * 0.5f,
          halfHeight = GRID_HEIGHT * 0.5f;

    std::random_device                rdev;
    std::uniform_real_distribution<>  dx(-1.0f * halfWidth, halfWidth),
                                      dy(-1.0f * halfHeight, halfHeight);

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

    this->glpreset.setOrthoSize(glm::vec2(GRID_WIDTH, GRID_HEIGHT) * 1.1f);
    this->glpreset.setScreenSize(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));

    this->orthoProjection = this->glpreset.projection();
  }
};

int main(int argc, char **argv) {
  return MyApplication().start(argc, argv);
}
