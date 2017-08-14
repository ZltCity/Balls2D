#include <numeric>
#include <mutex>

#include "physics.h"

Particle::Particle(const glm::vec2 &pos)
  : pos(pos), prev(pos), mass(1.0f), diffSum(0.0f) {
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
}

void Particle::move(float dt) {
  glm::vec2 delta = this->pos - this->prev;

  this->forces *= 0.85f;

  this->prev   = this->pos;
  this->pos   += delta * 0.998f/* * dt*/ + this->calcAcceleration() * dt * dt;
}

bool Particle::intersect(Particle &particle, glm::vec2 &outDist, float &outDiff) {
  glm::vec2 dist = this->pos - particle.getPosition();
  float     diff = 1.0f - glm::length(dist);

  if (diff < 0.0f)
    return false;

  outDist = dist;
  outDiff = diff;

  this->diffSum     += diff;
  particle.diffSum  += diff;

  return true;
}

void Particle::zeroDiffSum() {
  this->diffSum = 0.0f;
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
  if (index >= DEFAULT_CELL_CAPACITY)
    return nullptr;

  return this->particles[index];
}

void GridCell::push(Particle *particle) {
  std::lock_guard<AtomicLock> lock(this->pushLock);

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
  if (coord.x < 0 || coord.x >= static_cast<int>(this->size.x)
   || coord.y < 0 || coord.y >= static_cast<int>(this->size.y))
    throw std::out_of_range(std::string("Invalid grid coordinates."));

  return this->cells.get()[coord.x + coord.y * this->size.x];
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

void Grid::push(Particle *particle) {
  if (!(*this) || !particle)
    return;

  this->operator[](glm::uvec2(particle->getPosition())).push(particle);
}

void Grid::clear(size_t offset, size_t count) {
  for (size_t y = 0; y < this->size.y; ++y)
    for (size_t x = offset; x < offset + count; ++x)
      this->operator[](glm::uvec2(x, y)).clear();
}

glm::uvec2 Grid::getSize() const {
  return this->size;
}

size_t Grid::getCount() const {
  return this->size.x * this->size.y;
}

Physics::Physics()
  : count(0), dt(1.0f), grid(glm::uvec2(0, 0)) {}

Physics::Physics(size_t count, const glm::vec2 &size, float dt)
  : count(count), dt(dt), grid(size) {
  this->plist.reserve(count);
}

void Physics::clearGrid(size_t offset, size_t count) {
  this->grid.clear(offset, count);
}

void Physics::update(size_t offset, size_t count) {
  for (size_t i = offset; i < offset + count; ++i) {
    Particle &particle = this->plist[i];

    particle.zeroDiffSum();
    particle.move(this->dt);
    this->correctToBounds(particle);
    this->grid.push(&particle);
  }
}

void Physics::solve(const glm::ivec2 &offset, const glm::ivec2 size) {
  auto solveCell = [](Particle &p0, GridCell &cell, size_t offset) {
    for (size_t i = offset; i < cell.getCount(); ++i) {
      Particle  &p1 = *cell.getParticlePtr(i);

      glm::vec2  dist;
      float      diff = 0.0f;

      if (!p0.intersect(p1, dist, diff))
        continue;

      glm::normalize(dist);

      dist *= diff * 0.5f;
      p0.setPosition(p0.getPosition() + dist);
      p1.setPosition(p1.getPosition() - dist);
    }
  };
  //
  glm::uvec2 &gridSize = this->grid.getSize();

  for (int cy = offset.y; cy < offset.y + size.y; ++cy)
    for (int cx = offset.x; cx < offset.x + size.x; ++cx) {
      GridCell  &c0 = this->grid[glm::uvec2(cx, cy)];

      //  Проверка с самой собой(ячейкой).
      solveCell(*c0.getParticlePtr(0), c0, 1);

      for (size_t pi = 0; pi < c0.getCount(); ++pi) {
        Particle &p0 = *c0.getParticlePtr(pi);

        for (int y = cy; y <= cy + 1; ++y) {
          for (int x = cx - 1; x <= cx + 1; ++x) {
            if (
               (y == cy && x < cx + 1)
            //|| (x == cx && y == cy)
            || x < 0 || x > static_cast<int>(gridSize.x) - 1
            || y < 0 || y > static_cast<int>(gridSize.y) - 1)
              continue;

            solveCell(p0, this->grid[glm::uvec2(x, y)], 0);
          }
        }
      }
    }
}

void Physics::setParticlesCount(size_t count) {
  this->count = count;
}

void Physics::setGridSize(const glm::uvec2 &size) {
  this->grid.alloc(size);
}

void Physics::setDt(float dt) {
  this->dt = dt;
}

std::vector<Particle> &Physics::getParticles() {
  return this->plist;
}

const std::vector<Particle> &Physics::getParticles() const {
  return this->plist;
}

glm::uvec2 Physics::getGridSize() const {
  return this->grid.getSize();
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
