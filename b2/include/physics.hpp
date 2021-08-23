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
	explicit Particle(const glm::vec3 &position);

	glm::vec3 position, delta;
};

class ParticleCloud
{
public:
	using Generator = std::function<Particle()>;

	ParticleCloud() = default;
	ParticleCloud(const glm::ivec3 &gridSize, size_t particlesCount, Generator generator);

	void update(const glm::vec3 &acceleration, float dt, bool singleThread = true);

	[[nodiscard]] glm::ivec3 getGridSize() const;
	[[nodiscard]] const std::vector<Particle> &getParticles() const;

private:
	static const size_t solverIterations = 2, cellCapacity = 16;

	struct Cell
	{
		Cell();

		void reset();

		size_t slots[cellCapacity];
		std::atomic_uint8_t count;
	};

	struct Grid
	{
		Grid() = default;
		explicit Grid(const glm::ivec3 &size);

		std::vector<Cell> cells;
		glm::ivec3 size;
	};

	void moveParticles(const glm::vec3 &acceleration, float dt, bool singleThread);
	void fill(bool singleThread);
	void resolve(bool singleThread);
	void resolveParticles(Particle &p1, Particle &p2);
	void resolveBounds(bool singleThread);
	void pushParticle(Particle &p, const glm::vec3 &v);

	Grid grid;
	std::vector<Particle> particles;
	Generator generator;
};

} // namespace b2::physics
