#pragma once

#include <string>
#include <utility>
#include <map>
#include <glm/glm.hpp>

#include "gl_core_4_5.h"

struct GLPreset {
  typedef std::map<std::string, GLuint> ResourceList;

  glm::vec4 color;
  ResourceList programs;
};

std::pair<bool, GLPreset> loadGLPreset(const std::string &filepath);
