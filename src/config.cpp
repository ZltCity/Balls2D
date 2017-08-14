#include <json/json.h>

#include "file.h"
#include "config.h"

Config::Config()
  : windowSize(600, 600),
    windowPos(50, 50),
    gridSize(128, 128),
    particlesCount(12000),
    threadsCount(4),
    dt(0.01f) {
}

bool Config::loadFromFile(const std::string &filePath) {
  File file;

  if (!file.load(filePath))
    return false;

  const char *fileRaw = file.getPtr<char>();
  Json::Value root;
  
  if (!Json::Reader().parse(fileRaw, fileRaw + file.getSize(), root, false))
    return false;

  this->windowSize = glm::ivec2(root.get("windowWidth", 600).asInt(), root.get("windowHeight", 600).asInt());
  this->windowPos = glm::ivec2(root.get("windowPosX", 10).asInt(), root.get("windowPosY", 10).asInt());
  this->gridSize = glm::ivec2(root.get("gridWidth", 128).asInt(), root.get("gridHeight", 128).asInt());
  this->particlesCount = root.get("particlesCount", 12000).asInt();
  this->threadsCount = root.get("threadsCount", 4).asInt();
  this->dt = root.get("dt", 0.01f).asFloat();

  return true;
}
