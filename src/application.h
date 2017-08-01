#pragma once

#include <glm/glm.hpp>

class Application {
public:
  enum {
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_X1,
    MOUSE_BUTTON_X2
  };

  Application();
  virtual ~Application()      = 0;

  int start(int argc, char **argv);

  virtual void  onStart()     = 0;
  virtual int   onQuit()      = 0;

  virtual void  onPreFrame()  = 0;
  virtual void  onPostFrame() = 0;
  virtual void  onFrame()     = 0;

  virtual void  onMouseMove(const glm::vec2 &pos, bool relative = false);

  virtual void  onButtonPressed(int button, const glm::vec2 &pos);
  virtual void  onButtonReleased(int button, const glm::vec2 &pos);

private:
  Application(const Application &)            = delete;
  Application &operator=(const Application &) = delete;
};
