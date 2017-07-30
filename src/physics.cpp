#include <numeric>

#include "physics.h"

Particle::Particle(const glm::vec2 &pos)
  : pos(pos), prev(pos), m(1.0f) {
}

glm::vec2 &Particle::position() {
  return this->pos;
}

const glm::vec2 &Particle::position() const {
  return this->pos;
}

float Particle::mass() const {
  return this->m;
}

void Particle::applyForce(const glm::vec2 force) {
  this->forces += force;
}

void Particle::move(float dt) {
  glm::vec2 delta = this->pos - this->prev;

  this->prev = this->pos;
  this->pos += delta + this->acceleration() * dt * dt;

  this->forces = glm::vec2(0.0f, 0.0f);
}

void Particle::setPosition(const glm::vec2 &pos) {
  this->pos = pos;
}

bool Particle::intersect(const Particle &particle, glm::vec2 &outDist, float &outDiff) {
  glm::vec2 dist = this->pos - particle.position();
  float     diff = 1.0f - glm::length(dist);

  if (diff <= 0.f)
    return false;

  outDist = dist;
  outDiff = diff;

  return true;
}

glm::vec2 Particle::acceleration() const {
  return this->forces * 1.0f / this->m;
}

Grid::Grid(const glm::vec2 &size)
  : gridSize(size) {
}

void Grid::push(Particle *particle) {
  this->grid[getCellKey(*particle)]
    .particles.push_back(particle);
}

void Grid::clear() {
  for (auto &cell : this->grid)
    cell.second.particles.clear();
}

glm::vec2 Grid::size() const {
  return this->gridSize;
}

Grid::Cell &Grid::cell(const glm::uvec2 coord) {
  return this->grid[getCellKey(coord)];
}

const Grid::Cell &Grid::cell(const glm::uvec2 coord) const {
  auto iter = this->grid.find(getCellKey(coord));

  if (iter != this->grid.end())
    return iter->second;

  static Cell _dummyCell;

  return _dummyCell;
}

uint64_t Grid::getCellKey(const Particle &particle) {
  uint64_t    key   = 0;
  glm::uvec2  coord = getCellCoord(particle);

  return (static_cast<uint64_t>(coord.x) << 32) + coord.y;
}

uint64_t Grid::getCellKey(const glm::uvec2 &coord) {
  return (static_cast<uint64_t>(coord.x) << 32) + coord.y;
}

glm::uvec2 Grid::getCellCoord(const Particle &particle) {
  glm::vec2 pos = particle.position();
  uint32_t  cx  = static_cast<uint32_t>(glm::floor(pos.x)) + std::numeric_limits<uint32_t>::max() / 2,
            cy  = static_cast<uint32_t>(glm::floor(pos.y)) + std::numeric_limits<uint32_t>::max() / 2;

  return glm::uvec2(cx, cy);
}

Physics::Physics(size_t count, const glm::vec2 &size)
  : count(count), dt(0.0f), grid(size) {
  this->plist.reserve(count);
}

void Physics::update(float dt) {
  this->solve();
  //
  this->grid.clear();

  for (size_t i = 0; i < this->plist.size(); ++i) {
    Particle *pPtr = &this->plist[i];

    pPtr->move(dt);
    this->grid.push(pPtr);
  }

  this->dt = dt;
}

std::vector<Particle> &Physics::particles() {
  return this->plist;
}

const std::vector<Particle> &Physics::particles() const {
  return this->plist;
}

void Physics::solve() {
  for (Particle &particle : this->plist) {
    this->solveBounds(particle);
    //
    glm::uvec2 cellCoord = Grid::getCellCoord(particle);

    for (uint32_t y = cellCoord.y - 1; y <= cellCoord.y + 1; ++y)
      for (uint32_t x = cellCoord.x - 1; x <= cellCoord.x + 1; ++x) {
        Grid::Cell &cell = this->grid.cell(glm::uvec2(x, y));

        for (Particle *cpPtr : cell.particles) {
          glm::vec2 dist;
          float     diff = 0.f;

          if (!particle.intersect(*cpPtr, dist, diff))
            continue;

          glm::normalize(dist);
          dist *= diff * 0.5f;
          particle.setPosition(particle.position() + dist);
          cpPtr->setPosition(cpPtr->position() - dist);
        }
      }
  }
}

void Physics::solveBounds(Particle &particle) {
  glm::vec2 gridSize    = this->grid.size();
  float     halfWidth   = gridSize.x * 0.5f,
            halfHeight  = gridSize.y * 0.5f;

  glm::vec2 &pos        = particle.position();

  if (pos.x < -1.0f * halfWidth) {
    particle.applyForce(glm::vec2(100.0f, 0.0f));
    particle.move(this->dt);
  }
  if (pos.x > halfWidth) {
    particle.applyForce(glm::vec2(-100.0f, 0.0f));
    particle.move(this->dt);
  }

  if (pos.y < -1.0f * halfHeight) {
    particle.applyForce(glm::vec2(0.0f, 100.0f));
    particle.move(this->dt);
  }
  if (pos.y > halfHeight) {
    particle.applyForce(glm::vec2(0.0f, -100.0f));
    particle.move(this->dt);
  }
}
