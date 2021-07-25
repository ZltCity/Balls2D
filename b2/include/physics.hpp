#pragma once

#include <functional>
#include <mutex>
#include <vector>

#include <glm/glm.hpp>

namespace b2::physics
{

struct Particle
{
	Particle() = default;
	Particle(const glm::vec3 &position);

	glm::vec3 position, delta;
};

class Grid
{
public:
	Grid() = default;
	Grid(const glm::ivec3 &size);

	static const size_t cellCapacity = 6;

	struct Cell
	{
		Cell();

		size_t slots[cellCapacity], count;
	};

	std::vector<Cell> cells;
	std::vector<std::mutex> cellsLocks;
	glm::ivec3 size;
};

class Cloud
{
public:
	Cloud() = default;
	Cloud(const glm::ivec3 &gridSize, size_t particlesCount, std::function<Particle()> generator);

	void update(const glm::vec3 &acceleration, float dt, bool singleThread = true);

	glm::ivec3 getGridSize() const;
	const std::vector<Particle> &getParticles() const;

	static const int32_t solverIterations = 2;

private:
	void moveParticles(const glm::vec3 &acceleration, float dt, bool singleThread);
	void fill(bool singleThread);
	void resolve(bool singleThread);
	void resolveParticles(Particle &p1, Particle &p2);
	void resolveBounds(bool singleThread);
	void pushParticle(Particle &p, const glm::vec3 &v);

	Grid grid;
	std::vector<Particle> particles;
};

} // namespace b2::physics
