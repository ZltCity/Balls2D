#include <ctime>
#include <cstdlib>
#include <random>

#include "application.h"
#include "gl_context.h"
#include "graphics.h"
#include "window.h"
#include "file.h"
#include "log.h"
#include "config.h"
#include "threadpool.h"

#include "physics.h"

const float   G_CONST           = static_cast<float>(0.1f/*6.67e-11*/);

class ClearTask : public Task {
public:
  ClearTask(Physics &physics, size_t index, size_t count)
    : physics(physics), index(index), count(count) {}

  void doTask() {
    this->physics.clearGrid(index * count, count);
  }

private:
  Physics &physics;
  size_t index, count;
};

class UpdateTask : public Task {
public:
  UpdateTask(Physics &physics, size_t index, size_t count)
    : physics(physics), index(index), count(count) {}

  void doTask() {
    this->physics.update(index * count, count);
  }

private:
  Physics &physics;
  size_t index, count;
};

class SolverTask : public Task {
public:
  SolverTask(Physics &physics, size_t index, size_t step)
    : physics(physics), index(index), step(step) {}

  void doTask() {
    glm::uvec2  &gridSize   = physics.getGridSize();
    glm::ivec2   offset     = glm::ivec2(this->index * this->step, 0),
                 size       = glm::ivec2(this->step, gridSize.y);

    this->physics.solve(offset, size);
  }

private:
  Physics &physics;
  size_t index, step;
};

class MyApplication : public Application {
public:
  MyApplication()
    : state(ENTRY_STATE) {
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
    switch (this->state) {
      case ENTRY_STATE: {
        this->startClear();
        break;
      }
      case CLEAR_STATE: {
        this->checkClear();
        break;
      }
      case UPDATE_STATE: {
        this->checkUpdate();
        break;
      }
      case SOLVER_STATE: {
        this->checkSolver();
        break;
      }
    }   
  }

  void onMouseMove(const glm::vec2 &pos, bool relative) {
    if (!relative) {
      this->mousePosition.x = pos.x;
      this->mousePosition.y = this->config.windowSize.y - pos.y;
    }
  }

  void onButtonPressed(int button, const glm::vec2 &pos) {
    switch (button) {
      case MOUSE_BUTTON_LEFT: {
        this->flags.lBtnPressed = true;
        break;
      }
      case MOUSE_BUTTON_RIGHT: {
        this->flags.rBtnPressed = true;
        break;
      }
    }
  }

  void onButtonReleased(int button, const glm::vec2 &pos) {
    switch (button) {
      case MOUSE_BUTTON_LEFT: {
        this->flags.lBtnPressed = false;
        break;
      }
      case MOUSE_BUTTON_RIGHT: {
        this->flags.rBtnPressed = false;
        break;
      }
    }
  }

private:
  enum {
    ENTRY_STATE,
    CLEAR_STATE,
    UPDATE_STATE,
    SOLVER_STATE
  } state;

  Config        config;

  Window        mainWnd;
  GLContext     glContext;
  GLPreset      glpreset;
  GLResource    glvertices;
  Physics       physics;
  ThreadPool    threadPool;

  std::vector
    <Particle>  plistCopy;
  
  std::vector
    <TaskPtr>   clearTaskList,
                updateTaskList,
                solverTaskList;

  glm::mat4     orthoProjection;
  glm::vec2     mousePosition;

  struct {
    bool lBtnPressed, rBtnPressed;
  } flags;

  void init() {
    flags.lBtnPressed = false;
    flags.rBtnPressed = false;
    //
    if (!config.loadFromFile("config.json"))
      Log::instance().print(LOG_WARN, std::string("Config file is missing, program defaults has loaded."));
    //
    this->createWidnow();
    this->createGLContext();
    this->loadResources();
    //
    //
    std::random_device                rdev;
    std::uniform_real_distribution<>  dx(5.0f, static_cast<float>(this->config.gridSize.x) - 5.0f),
                                      dy(5.0f, static_cast<float>(this->config.gridSize.y) - 5.0f);

    this->physics.setParticlesCount(this->config.particlesCount);
    this->physics.setGridSize(this->config.gridSize);
    this->physics.setDt(this->config.dt);
    this->physics.init(
      [&](size_t) {
        return Particle(glm::vec2(dx(rdev), dy(rdev)));
      }
    );   
        
    this->glvertices = createVertexBuffer<Particle>(0, nullptr, GL_DYNAMIC_DRAW);
    //
    this->threadPool.alloc(this->config.threadsCount);
    this->clearTaskList.resize(this->config.threadsCount);
    this->updateTaskList.resize(this->config.threadsCount);
    this->solverTaskList.resize(this->config.threadsCount);

    size_t gridStep = this->config.gridSize.x / this->config.threadsCount;

    for (size_t t = 0; t < this->config.threadsCount; ++t) {
      this->clearTaskList[t] = TaskPtr(new ClearTask(this->physics, t, gridStep));
      this->updateTaskList[t] = TaskPtr(new UpdateTask(this->physics, t, this->config.particlesCount / this->config.threadsCount));
      this->solverTaskList[t] = TaskPtr(new SolverTask(this->physics, t, gridStep));
    }
  }

  void free() {
  }

  void createWidnow() {
    Log &log = Log::instance();

    if (this->mainWnd.create(std::string("Some shitty physics, v1.0"), this->config.windowPos.x, this->config.windowPos.y, this->config.windowSize.x, this->config.windowSize.y))
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

    this->glpreset.setOrthoSize(this->config.gridSize);
    this->glpreset.setScreenSize(this->config.windowSize);

    this->orthoProjection = this->glpreset.projection();
  }

  void startTaskList(TaskPtr taskList[]) {
    for (size_t t = 0; t < this->config.threadsCount; ++t) {
      TaskPtr taskPtr = taskList[t];

      taskPtr->resetDone();
      this->threadPool.pushTask(taskPtr);
    }
  }

  bool checkTaskList(TaskPtr taskList[]) {
    bool done = true;

    for (size_t t = 0; t < this->config.threadsCount; ++t)
      if (!taskList[t]->done()) {
        done = false;

        break;
      }

    return done;
  }

  void startClear() {
    this->startTaskList(this->clearTaskList.data());
    this->state = CLEAR_STATE;
  }

  void checkClear() {
    if (checkTaskList(this->clearTaskList.data()))
      this->startUpdate();
  }

  void startUpdate() {
    this->startTaskList(this->updateTaskList.data());
    this->state = UPDATE_STATE;
  }

  void checkUpdate() {
    if (checkTaskList(this->updateTaskList.data()))
      this->startSolver();
  }

  void startSolver() {
    this->startTaskList(this->solverTaskList.data());
    this->state = SOLVER_STATE;
  }

  void checkSolver() {
    if (checkTaskList(this->solverTaskList.data()))
      this->doRender();
  }

  void doRender() {
    glm::mat4 inverted  = glm::inverse(this->orthoProjection);
    glm::vec4 cursor    = inverted * glm::vec4(
      mousePosition.x / this->config.windowSize.x * 2.0f - 1.0f, 
      mousePosition.y / this->config.windowSize.y * 2.0f - 1.0f, 0.0f, 1.0f);

    for (Particle &particle : this->physics.getParticles()) {
      particle.applyForce(glm::vec2(0.0f, -1.0f));

      if (!this->flags.lBtnPressed && !this->flags.rBtnPressed)
        continue;

      //
      glm::vec2 dist    = glm::vec2(cursor) - particle.getPosition();
      float     length  = glm::length(dist),
                strengh = (1.0f - length / 30.0f) * 10.0f;

      if (glm::length(dist) > 30.0f)
        continue;

      if (this->flags.rBtnPressed)
        strengh *= -1.0f;

      particle.applyForce(glm::normalize(dist) * strengh);
    }

    writeVertexBuffer(this->glvertices, this->config.particlesCount, this->physics.getParticles().data(), GL_DYNAMIC_DRAW);

    this->glpreset.use();
    bindVertexBuffer(this->glvertices);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<char *>(0) + sizeof(glm::vec2) * 3 + sizeof(float));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(4);

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, this->config.particlesCount);

    this->mainWnd.present();
    //
    this->state = ENTRY_STATE;
  }
};

int main(int argc, char **argv) {
  return MyApplication().start(argc, argv);
}
