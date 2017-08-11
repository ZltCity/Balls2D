#pragma once

#include <cstdint>

#include <vector>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>

#include "atomic.h"

const int DEFAULT_CELL_CAPACITY = 32;

class Particle {
public:
  Particle() = default;
  Particle(const glm::vec2 &pos);

  void setPosition(const glm::vec2 &pos);

  glm::vec2 &getPosition();
  const glm::vec2 &getPosition() const;

  glm::vec2 &getForses();
  const glm::vec2 &getForses() const;

  float getMass() const;

  void applyForce(const glm::vec2 &force);
  void move(float dt);

  bool intersect(Particle &particle, glm::vec2 &outDist, float &outDiff);

  void zeroDiffSum();

private:
  glm::vec2 pos, prev, forces;
  float     mass, diffSum;

  glm::vec2 calcAcceleration() const;
};

class GridCell {
public:
  GridCell();

  size_t getCount() const;
  
  Particle *getParticlePtr(size_t index);
  const Particle *getParticlePtr(size_t index) const;

  void push(Particle *particle);
  void clear();

private:
  size_t       count;
  Particle    *particles[DEFAULT_CELL_CAPACITY];
  AtomicLock   pushLock;
};

class Grid {
public:
  Grid(const glm::uvec2 &size);

  operator bool() const;

  GridCell &operator[](const glm::ivec2 &coord);
  const GridCell &operator[](const glm::ivec2 &coord) const;

  void push(Particle *particle);
  void clear();

  glm::uvec2 getSize() const;
  size_t getCount() const;

private:
  std::shared_ptr
    <GridCell>  cells;
  glm::uvec2    size;

  void alloc(const glm::uvec2 &size);
};

class Physics {
public:
  Physics(size_t count, const glm::vec2 &size, float dt);

  template<typename TGenerator>
  void init(TGenerator &&gen) {
    for (size_t i = 0; i < this->count; ++i)
      this->plist.push_back(gen(i));
  }

  void update(size_t offset, size_t count);
  void solve(const glm::ivec2 &offset, const glm::ivec2 size);
  void clearGrid();

  std::vector<Particle> &getParticles();
  const std::vector<Particle> &getParticles() const;

  glm::uvec2 getGridSize() const;

private:
  size_t  count;
  float   dt;

  std::vector
    <Particle>  plist;
  Grid          grid;

  void correctToBounds(Particle &particle);
};
