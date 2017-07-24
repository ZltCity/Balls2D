#pragma once

#include "window.h"
#include "gl_core_4_5.h"

class GLContext {
public:
  GLContext() = default;
  GLContext(const Window &w);

  bool create(const Window &w);
  void makeCurrent(const Window &w);

private:
  std::shared_ptr<void> cPtr;
};
