#pragma once

#include <memory>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include <json/json.h>

#include "gl_core_4_5.h"

class GLPreset {
public:
  typedef std::map<std::string, class GLResource> ResourceList;

  GLPreset() = default;

  bool loadFromString(const std::string &preset);
  bool loadFromFile(const std::string &filepath);

  glm::vec4 getColor() const;

private:
  glm::vec4 color;
  ResourceList programs;

  void parseScreen(const Json::Value &root);
  void parsePrograms(const Json::Value &root);
  GLResource buildProgram(const Json::Value &program);
};

class GLResource {
public:
  typedef std::shared_ptr<GLuint> ptrType;

  GLResource();

  operator bool() const;

  template<typename TDestructor>
  void alloc(GLuint id, TDestructor &&d) {
    this->idPtr.reset(new GLuint(id), d);
  }

  GLuint &id();
  const GLuint &id() const;

private:
  ptrType idPtr;
};
