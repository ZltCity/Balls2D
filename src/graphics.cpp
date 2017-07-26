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

bool GLPreset::loadFromString(const std::string &preset) {
  Json::Value root;

  if (!Json::Reader().parse(preset, root))
    return false;

  this->parseScreen(root);
  this->parsePrograms(root);

  return true;
}

bool GLPreset::loadFromFile(const std::string &filepath) {
  File fDesc;

  if (!fDesc.load(filepath))
    return false;

  return this->loadFromString(std::string(fDesc.getPtr<char>()));
}

glm::vec4 GLPreset::getColor() const {
  return this->color;
}

void GLPreset::parseScreen(const Json::Value &root) {
  Json::Value screen = root["screen"];

  if (!screen.isObject())
    return;

  Json::Value color = screen["color"];

  if (color.isArray()) {
    this->color = glm::vec4(color[0].asFloat(), color[1].asFloat(), color[2].asFloat(), color[3].asFloat());
  }
}

void GLPreset::parsePrograms(const Json::Value &root) {
  Json::Value programs = root["programs"];

  if (!programs.isArray())
    return;

  Json::ArrayIndex count = programs.size();

  for (Json::ArrayIndex i = 0; i < count; ++i) {
    Json::Value program = programs[i];
    std::string name    = program["name"].asString();
    GLResource  resp    = this->buildProgram(program);

    if (!resp)
      continue;

    this->programs[name] = resp;
  }
}

GLResource GLPreset::buildProgram(const Json::Value &program) {
  GLResource  resp;

  if (!program.isObject())
    return resp;

  Json::Value shaders = program["shaders"];

  if (!shaders.isArray())
    return resp;

  resp.alloc(glCreateProgram(), 
    [](GLuint *id) {
      glDeleteProgram(*id);
      delete id;
    }
  );

  if (!resp)
    return resp;

  Json::ArrayIndex        count = shaders.size();
  std::vector<GLResource> slist;

  for (Json::ArrayIndex i = 0; i < count; ++i) {
    Json::Value shader  = shaders[i];
    GLResource  ress    = loadShader(shader);

    if (!ress)
      continue;

    slist.push_back(ress);
    glAttachShader(resp.id(), ress.id());
  }

  GLint param = 0;

  glLinkProgram(resp.id());
  glGetProgramiv(resp.id(), GL_LINK_STATUS, &param);

  Log &log = Log::instance();

  if (param == GL_TRUE)
    log.print(LOG_INFO, std::string("OpenGL program building succeed - ") + program["name"].asString());
  else {
    std::unique_ptr<GLchar[]> buff;

    glGetProgramiv(resp.id(), GL_INFO_LOG_LENGTH, &param);
    buff.reset(new GLchar[param + 1]);
    std::memset(buff.get(), 0, param + 1);
    glGetProgramInfoLog(resp.id(), param + 1, nullptr, buff.get());

    log.print(LOG_WARN,
      std::string("OpenGL program building failed - ")
    + program["name"].asString()
    + (param > 1 ? std::string("\n") + std::string(reinterpret_cast<char *>(buff.get())) : std::string()));
  }

  for (auto ress : slist)
    glDetachShader(resp.id(), ress.id());

  return resp;
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
