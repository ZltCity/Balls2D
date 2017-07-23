#pragma once

class Application {
public:
  Application();
  virtual ~Application()      = 0;

  int start(int argc, char **argv);

  virtual void  onStart()     = 0;
  virtual int   onQuit()      = 0;

  virtual void  onPreFrame()  = 0;
  virtual void  onPostFrame() = 0;
  virtual void  onFrame()     = 0;

private:
  Application(const Application &)  = delete;
  Application(const Application &&) = delete;

  Application &operator=(const Application &)   = delete;
  Application &operator=(const Application &&)  = delete;
};
