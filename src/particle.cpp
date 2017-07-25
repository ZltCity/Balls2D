#include "particle.h"

Particle::Particle(const glm::vec2 &p)
  : p(p) {}

glm::vec2 &Particle::position() {
  return this->p;
}

const glm::vec2 &Particle::position() const {
  return this->p;
}