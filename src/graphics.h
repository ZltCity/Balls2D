#pragma once

#include <memory>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include <json/json.h>

#include "gl_core_4_5.h"

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

class GLPreset {
public:
  GLPreset();

  bool loadFromString(const std::string &preset);
  bool loadFromFile(const std::string &filepath);

  void setOrthoSize(const glm::vec2 &size);
  void setScreenSize(const glm::vec2 &size);

  glm::vec4 getColor() const;
  glm::mat4 projection() const;

  void use() const;

private:
  glm::vec4 backgroundColor;
  glm::vec2 orthoSize, screenSize;
  glm::mat4 orthoProjection;
  GLResource program;

  void parseScreen(const Json::Value &root);
  void parseProgram(const Json::Value &root);
};

template<typename TVertex>
GLResource createVertexBuffer(size_t count, const TVertex *data, GLenum usage) {
  GLuint      id = 0;
  GLResource  resb;

  glGenBuffers(1, &id);
  resb.alloc(id,
    [](GLuint *id) {
      glDeleteBuffers(1, id);
      delete id;
    }
  );

  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  writeVertexBuffer(resb, count, data, usage);

  return resb;
}

template<typename TVertex>
void writeVertexBuffer(GLResource &res, size_t count, const TVertex *data, GLenum usage) {
  if (!res || count == 0)
    return;

  glNamedBufferData(res.id(), sizeof(TVertex) * count, data, usage);
}

void bindVertexBuffer(const GLResource &res);
