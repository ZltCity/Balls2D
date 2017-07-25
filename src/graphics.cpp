#include <json/json.h>

#include "graphics.h"
#include "file.h"

void _parseScreen(const Json::Value &root, GLPreset &preset);
void _parsePrograms(const Json::Value &root, GLPreset &preset);
GLuint _buildProgram(const Json::Value &program);

void _parseScreen(const Json::Value &root, GLPreset &preset) {
  Json::Value screen = root["screen"];

  if (!screen.isObject())
    return;

  Json::Value color = screen["color"];

  if (color.isArray()) {
    preset.color = glm::vec4(color[0].asFloat(), color[1].asFloat(), color[2].asFloat(), color[3].asFloat());
  }
}

void _parsePrograms(const Json::Value &root, GLPreset &preset) {
  Json::Value programs = root["programs"];

  if (!programs.isArray())
    return;

  Json::ArrayIndex count = programs.size();

  for (Json::ArrayIndex i = 0; i < count; ++i) {
    Json::Value program = programs[i];
    std::string name    = program["name"].asString();
    GLuint      id      = _buildProgram(program);

    if (id == 0)
      continue;

    preset.programs[name] = id;
  }
}

GLuint _buildProgram(const Json::Value &program) {
  GLuint      id      = glCreateProgram();
  Json::Value shaders = program["shaders"];

  if (id == 0 || !shaders.isArray())
    return 0;

  Json::ArrayIndex count = shaders.size();

  for (Json::ArrayIndex i = 0; i < count; ++i) {
    Json::Value shader = shaders[i];
  }

  return id;
}

std::pair<bool, GLPreset> loadGLPreset(const std::string &filepath) {
  GLPreset  preset;
  auto      fDesc = loadFile(filepath);

  if (fDesc.size == 0)
    return std::make_pair(false, preset);

  Json::Value   root;
  char          *raw = reinterpret_cast<char *>(fDesc.buff.get());

  if (!Json::Reader().parse(raw, raw + fDesc.size, root))
    return std::make_pair(false, preset);

  _parseScreen(root, preset);
  _parsePrograms(root, preset);

  return std::make_pair(true, preset);
}
