#pragma once

#include <string>
#include <glm/glm.hpp>

class Config {
public:
  glm::ivec2 windowSize, windowPos, gridSize;
  int particlesCount, threadsCount;
  float dt;

  Config();

  bool loadFromFile(const std::string &filePath);
};
