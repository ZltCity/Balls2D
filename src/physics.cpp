#include <numeric>

#include "physics.h"

Particle::Particle(const glm::vec2 &pos)
  : pos(pos), prev(pos), mass(1.0f) {
}

void Particle::setPosition(const glm::vec2 &pos) {
  this->pos = pos;
}

glm::vec2 &Particle::getPosition() {
  return this->pos;
}

const glm::vec2 &Particle::getPosition() const {
  return this->pos;
}

glm::vec2 &Particle::getForses() {
  return this->forces;
}

const glm::vec2 &Particle::getForses() const {
  return this->forces;
}

float Particle::getMass() const {
  return this->mass;
}

void Particle::applyForce(const glm::vec2 &force) {
  this->forces += force;
  this->forces *= 0.9f;
}

void Particle::move(float dt) {
  glm::vec2 delta = this->pos - this->prev;

  this->prev   = this->pos;
  this->pos   += delta/* * dt*/ + this->calcAcceleration() * dt * dt;
}

bool Particle::intersect(const Particle &particle, glm::vec2 &outDist, float &outDiff) {
  glm::vec2 dist = this->pos - particle.getPosition();
  float     diff = 1.0f - glm::length(dist);

  if (diff < 0.0f)
    return false;

  outDist = dist;
  outDiff = diff;

  return true;
}

glm::vec2 Particle::calcAcceleration() const {
  return this->forces / this->mass;
}

GridCell::GridCell()
  : count(0) {}

size_t GridCell::getCount() const {
  return this->count;
}

Particle *GridCell::getParticlePtr(size_t index) {
  if (index >= DEFAULT_CELL_CAPACITY)
    return nullptr;

  return this->particles[index];
}

const Particle *GridCell::getParticlePtr(size_t index) const {
  return this->getParticlePtr(index);
}

void GridCell::push(Particle *particle) {
  if (this->count >= DEFAULT_CELL_CAPACITY)
    return;

  this->particles[this->count++] = particle;
}

void GridCell::clear() {
  this->count = 0;
}

Grid::Grid(const glm::uvec2 &size)
  : cells(nullptr), size(glm::vec2(0.0f, 0.0f)) {
  this->alloc(size);
}

Grid::operator bool() const {
  return this->cells.operator bool();
}

GridCell &Grid::operator[](const glm::ivec2 &coord) {
  if (coord.x < 0 || coord.x >= static_cast<int>(this->size.x)
   || coord.y < 0 || coord.y >= static_cast<int>(this->size.y))
    throw std::out_of_range(std::string("Invalid grid coordinates."));

  return this->cells.get()[coord.x + coord.y * this->size.x];
}

const GridCell &Grid::operator[](const glm::ivec2 &coord) const {
  return this->operator[](coord);
}

void Grid::push(Particle *particle) {
  if (!(*this) || !particle)
    return;

  this->operator[](glm::uvec2(particle->getPosition())).push(particle);
}

void Grid::clear() {
  for (size_t y = 0; y < this->size.y; ++y)
    for (size_t x = 0; x < this->size.x; ++x)
      this->operator[](glm::uvec2(x, y)).clear();
}

glm::uvec2 Grid::getSize() const {
  return this->size;
}

size_t Grid::getCount() const {
  return this->size.x * this->size.y;
}

void Grid::alloc(const glm::uvec2 &size) {
  try {
    this->cells.reset(new GridCell[size.x * size.y],
      [](GridCell *cells) {
        delete[]cells;
      }
    );
  } catch (const std::bad_alloc &) { return; }

  this->size = size;
}

Physics::Physics(size_t count, const glm::vec2 &size, float dt)
  : count(count), dt(dt), grid(size) {
  this->plist.reserve(count);
}

void Physics::update() {
  this->grid.clear();

  for (size_t i = 0; i < this->plist.size(); ++i) {
    Particle &particle = this->plist[i];

    particle.move(this->dt);
    this->correctToBounds(particle);
    this->grid.push(&particle);
  }

  this->solve();
}

std::vector<Particle> &Physics::getParticles() {
  return this->plist;
}

const std::vector<Particle> &Physics::getParticles() const {
  return this->plist;
}

void Physics::solve() {
  for (size_t i = 0; i < this->plist.size(); ++i) {
    Particle    &p0         = this->plist[i];
    glm::uvec2  &gridSize   = this->grid.getSize();
    glm::ivec2   gridCoord  = glm::uvec2(p0.getPosition());

    for (int y = gridCoord.y - 1; y <= gridCoord.y + 1; ++y) {
      if (y < 0 || y >= static_cast<int>(gridSize.y))
        continue;

      for (int x = gridCoord.x - 1; x <= gridCoord.x + 1; ++x) {
        if (x < 0 || x >= static_cast<int>(gridSize.x))
          continue;

        GridCell &cell = this->grid[glm::uvec2(x, y)];

        for (size_t pi = 0; pi < cell.getCount(); ++pi) {
          Particle  &p1 = *cell.getParticlePtr(pi);
          glm::vec2  dist;
          float      diff = 0.f;

          if (!p0.intersect(p1, dist, diff)/* || glm::length(dist) == 0.0f*/)
            continue;

          glm::normalize(dist);

          dist *= diff * 0.5f;
          p0.setPosition(p0.getPosition() + dist);
          p1.setPosition(p1.getPosition() - dist);
        }
      }
    }
  }
}

void Physics::correctToBounds(Particle &particle) {
  const glm::vec2 &gridSize = this->grid.getSize();
  glm::vec2       &pos      = particle.getPosition();
  float            left     = 0.0f,
                   right    = static_cast<float>(gridSize.x),
                   bottom   = 0.0f,
                   top      = static_cast<float>(gridSize.y);

  if (pos.x - 0.5f < left)
    pos.x = left + 0.5f;
  if (pos.x + 0.5f > right)
    pos.x = right - 0.5f;

  if (pos.y - 0.5f < bottom)
    pos.y = bottom + 0.5f;
  if (pos.y + 0.5f > top)
    pos.y = top - 0.5f;
}