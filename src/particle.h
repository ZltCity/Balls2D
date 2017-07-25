#pragma once

#include <glm/glm.hpp>

class Particle {
public:
  Particle() = default;
  Particle(const glm::vec2 &p);

  glm::vec2 &position();
  const glm::vec2 &position() const;

private:
  glm::vec2 p, pr;
};
