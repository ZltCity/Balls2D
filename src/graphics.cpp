#include <glm/gtc/matrix_transform.hpp>

#include "graphics.h"
#include "file.h"
#include "log.h"

GLenum getShaderType(const std::string &stype) {
  std::pair<std::string, GLenum> list[] = {
    std::make_pair(std::string("GL_VERTEX_SHADER"),   GLenum(GL_VERTEX_SHADER)),
    std::make_pair(std::string("GL_FRAGMENT_SHADER"), GLenum(GL_FRAGMENT_SHADER)),
    std::make_pair(std::string("GL_GEOMETRY_SHADER"), GLenum(GL_GEOMETRY_SHADER)),
  };

  for (auto item : list)
    if (item.first == stype)
      return item.second;

  return 0;
}

GLResource loadShader(const Json::Value &shader) {
  GLResource  ress;

  if (!shader.isObject())
    return ress;

  std::string type = shader["type"].asString(),
              file = shader["file"].asString();

  File fDesc;

  if (!fDesc.load(file))
    return ress;

  ress.alloc(glCreateShader(getShaderType(type)),
    [](GLuint *id) {
      glDeleteShader(*id);
      delete id;
    }
  );

  if (!ress)
    return ress;

  const GLchar  *raw  = fDesc.getPtr<const GLchar>();
  GLint         size  = static_cast<GLint>(fDesc.getSize()),
                param = 0;

  glShaderSource(ress.id(), 1, &raw, &size);
  glCompileShader(ress.id());
  glGetShaderiv(ress.id(), GL_COMPILE_STATUS, &param);

  Log &log = Log::instance();

  if (param == GL_TRUE)
    log.print(LOG_INFO, std::string("OpenGL shader compiling succeed - ") + file);
  else {
    std::unique_ptr<GLchar[]> buff;

    glGetShaderiv(ress.id(), GL_INFO_LOG_LENGTH, &param);
    buff.reset(new GLchar[param + 1]);
    std::memset(buff.get(), 0, param + 1);
    glGetShaderInfoLog(ress.id(), param + 1, nullptr, buff.get());

    log.print(LOG_WARN,
      std::string("OpenGL shader compiling failed - ")
    + file
    + (param > 1 ? std::string("\n") + std::string(reinterpret_cast<char *>(buff.get())) : std::string()));
  }

  return ress;
}

GLResource::GLResource()
  : idPtr(new GLuint(0))
{}

GLResource::operator bool() const {
  return this->id() != 0;
}

GLuint &GLResource::id() {
  return *this->idPtr;
}

const GLuint &GLResource::id() const {
  return *this->idPtr;
}

GLPreset::GLPreset() {}

bool GLPreset::loadFromString(const std::string &preset) {
  Json::Value root;

  if (!Json::Reader().parse(preset, root))
    return false;

  this->parseScreen(root);
  this->parseProgram(root);

  return true;
}

bool GLPreset::loadFromFile(const std::string &filepath) {
  File fDesc;

  if (!fDesc.load(filepath))
    return false;

  return this->loadFromString(std::string(fDesc.getPtr<char>()));
}

void GLPreset::setOrthoSize(const glm::vec2 &size) {
  this->orthoSize       = size;
  this->orthoProjection = this->projection();
}

void GLPreset::setScreenSize(const glm::vec2 &size) {
  this->screenSize = size;
}

glm::vec4 GLPreset::getColor() const {
  return this->backgroundColor;
}

glm::mat4 GLPreset::projection() const {
  float halfWidth   = this->orthoSize.x * 0.5f,
        halfHeight  = this->orthoSize.y * 0.5f;

  return glm::ortho(-1.f * halfWidth, halfWidth, -1.f * halfHeight, halfHeight);
}

void GLPreset::use() const {
  glm::mat4 pm = this->orthoProjection;

  glClearColor(
    this->backgroundColor.r,
    this->backgroundColor.g,
    this->backgroundColor.b,
    this->backgroundColor.a); 

  if (this->program) {
    GLuint pId = this->program.id();

    glUseProgram(pId);
    glUniformMatrix4fv(glGetUniformLocation(pId, "projection"),
      1, GL_FALSE, reinterpret_cast<GLfloat *>(&pm));
    glUniform2fv(glGetUniformLocation(pId, "screenSize"),
      1, reinterpret_cast<const GLfloat *>(&this->screenSize));
    glUniform1f(glGetUniformLocation(pId, "pointSize"), this->screenSize.x / this->orthoSize.x);
  }
}

void GLPreset::parseScreen(const Json::Value &root) {
  Json::Value screen = root["screen"];

  if (!screen.isObject())
    return;

  Json::Value color         = screen["backgroundColor"];

  if (color.isArray())
    this->backgroundColor = glm::vec4(color[0].asFloat(), color[1].asFloat(), color[2].asFloat(), color[3].asFloat());
}

void GLPreset::parseProgram(const Json::Value &root) {
  Json::Value program = root["program"];

  if (!program.isObject())
    return;

  Json::Value shaders = program["shaders"];

  if (!shaders.isArray())
    return;

  this->program.alloc(glCreateProgram(), 
    [](GLuint *id) {
      glDeleteProgram(*id);
      delete id;
    }
  );

  if (!this->program)
    return;

  GLuint id = this->program.id();

  Json::ArrayIndex        count = shaders.size();
  std::vector<GLResource> slist;

  for (Json::ArrayIndex i = 0; i < count; ++i) {
    Json::Value shader  = shaders[i];
    GLResource  ress    = loadShader(shader);

    if (!ress)
      continue;

    slist.push_back(ress);
    glAttachShader(id, ress.id());
  }

  GLint param = 0;

  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &param);

  Log &log = Log::instance();

  if (param == GL_TRUE)
    log.print(LOG_INFO, std::string("OpenGL program building succeed - ") + program["name"].asString());
  else {
    std::unique_ptr<GLchar[]> buff;

    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &param);
    buff.reset(new GLchar[param + 1]);
    std::memset(buff.get(), 0, param + 1);
    glGetProgramInfoLog(id, param + 1, nullptr, buff.get());

    log.print(LOG_WARN,
      std::string("OpenGL program building failed - ")
    + program["name"].asString()
    + (param > 1 ? std::string("\n") + std::string(reinterpret_cast<char *>(buff.get())) : std::string()));
  }

  for (auto ress : slist)
    glDetachShader(id, ress.id());
}

void bindVertexBuffer(const GLResource &res) {
  if (!res)
    return;

  glBindBuffer(GL_ARRAY_BUFFER, res.id());
}
