#pragma once

#include <cstdint>

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

class Particle {
public:
  Particle() = default;
  Particle(const glm::vec2 &pos);

  glm::vec2 &position();
  const glm::vec2 &position() const;

  float mass() const;

  void applyForce(const glm::vec2 force);
  void move(float dt);

  void setPosition(const glm::vec2 &pos);

  bool intersect(const Particle &particle, glm::vec2 &outDist, float &outDiff);

private:
  glm::vec2 pos, prev,
            forces;
  float     m;

  glm::vec2 acceleration() const;
};

class Grid {
public:
  struct Cell {
    std::vector<Particle *> particles;
  };

  Grid(const glm::vec2 &size);

  void push(Particle *particle);
  void clear();

  glm::vec2 size() const;

  Cell &cell(const glm::uvec2 coord);
  const Cell &cell(const glm::uvec2 coord) const;

  static uint64_t getCellKey(const Particle &particle);
  static uint64_t getCellKey(const glm::uvec2 &coord);
  static glm::uvec2 getCellCoord(const Particle &particle);

private:
  std::unordered_map
    <uint64_t, Cell>  grid;
  glm::vec2           gridSize;
};

class Physics {
public:
  Physics(size_t count, const glm::vec2 &size);

  template<typename TGenerator>
  void init(TGenerator &&gen) {
    for (size_t i = 0; i < this->count; ++i)
      this->plist.push_back(gen(i));
  }

  void update(float dt);

  std::vector<Particle> &particles();
  const std::vector<Particle> &particles() const;

private:
  size_t  count;
  float   dt;

  std::vector
    <Particle>  plist;
  Grid          grid;

  void solve();
  void solveBounds(Particle &particle);
};
