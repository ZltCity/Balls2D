#include <thread>

#include "physics.hpp"
#include "threadpool.hpp"

#include "timer.hpp"

namespace b2::physics
{

Particle::Particle(const glm::vec3 &position) : position(position), delta(0.0f)
{}

Grid::Cell::Cell() : count(0)
{
	std::fill(slots, slots + Grid::cellCapacity, 0);
}

Grid::Grid(const glm::ivec3 &size) : size(size)
{
	const size_t linearSize = size.x * size.y * size.z;

	cells = std::vector<Cell>(linearSize);
	cellsLocks = std::vector<std::mutex>(linearSize);
}

Cloud::Cloud(const glm::ivec3 &gridSize, size_t particlesCount, std::function<Particle()> generator)
	: grid(gridSize), particles(particlesCount)
{
	for (int32_t i = 0; i < particlesCount; ++i)
		particles[i] = generator();
}

void Cloud::update(const glm::vec3 &acceleration, float dt, bool singleThread)
{
	moveParticles(acceleration, dt, singleThread);

	for (int32_t it = 0; it < solverIterations; ++it)
	{
		resolveBounds(singleThread);
		fill(singleThread);
		resolve(singleThread);
	}
}

glm::ivec3 Cloud::getGridSize() const
{
	return grid.size;
}

const std::vector<Particle> &Cloud::getParticles() const
{
	return particles;
}

void Cloud::moveParticles(const glm::vec3 &acceleration, float dt, bool singleThread)
{
	ThreadPool &threadPool = ThreadPool::getInstance();
	const size_t workersCount = threadPool.getWorkersCount(), particlesCount = particles.size(),
				 batchSize = particlesCount / workersCount;
	std::future<void> futures[workersCount];
	auto routine = [](std::vector<Particle> &particles, const glm::vec3 acceleration, size_t offset, size_t count,
					  float dt) {
		for (size_t i = offset; i < offset + count; ++i)
		{
			Particle &particle = particles[i];

			particle.delta += acceleration * dt * dt;
			particle.position += particle.delta;
		}
	};

	if (singleThread)
		routine(particles, acceleration, 0, particlesCount, dt);
	else
	{
		for (int32_t t = 0; t < workersCount; ++t)
			futures[t] = threadPool.pushTask(routine, std::ref(particles), acceleration, t * batchSize, batchSize, dt);

		for (auto &future : futures)
			future.wait();
	}
}

void Cloud::fill(bool singleThread)
{
	std::fill(grid.cells.begin(), grid.cells.end(), Grid::Cell());

	ThreadPool &threadPool = ThreadPool::getInstance();
	const size_t workersCount = threadPool.getWorkersCount(), particlesCount = particles.size(),
				 batchSize = particles.size() / workersCount;
	std::future<void> futures[workersCount];
	auto routine = [](Grid &grid, std::vector<Particle> &particles, size_t offset, size_t count) {
		const int32_t width = grid.size.x, square = width * grid.size.y;

		for (size_t i = offset; i < offset + count; ++i)
		{
			const glm::ivec3 cellCoord(particles[i].position);
			const size_t cellIdx = cellCoord.x + cellCoord.y * width + cellCoord.z * square;
			std::lock_guard lock(grid.cellsLocks[cellIdx]);
			Grid::Cell &cell = grid.cells[cellIdx];

			if (cell.count >= Grid::cellCapacity)
				continue;

			cell.slots[cell.count] = i;
			++cell.count;
		}
	};

	if (singleThread)
		routine(grid, particles, 0, particlesCount);
	else
	{
		for (int32_t t = 0; t < workersCount; ++t)
			futures[t] = threadPool.pushTask(routine, std::ref(grid), std::ref(particles), t * batchSize, batchSize);

		for (auto &future : futures)
			future.wait();
	}
}

void Cloud::resolve(bool singleThread)
{
	ThreadPool &threadPool = ThreadPool::getInstance();
	const size_t cellsCount = grid.cells.size(), batchSize = 1024, tasksCount = (cellsCount / batchSize) + 1;
	std::future<void> futures[tasksCount];
	auto routine = [](Cloud *self, Grid &grid, std::vector<Particle> &particles, size_t offset, size_t count) {
		const int32_t width = grid.size.x, square = width * grid.size.y;

		for (size_t ci1 = offset; ci1 < offset + count; ++ci1)
		{
			const Grid::Cell &cell1 = grid.cells[ci1];
			const glm::ivec3 cellCoord((ci1 % square) % width, (ci1 % square) / width, ci1 / square);

			for (int32_t si1 = 0; si1 < cell1.count; ++si1)
			{
				for (int32_t z = cellCoord.z - 1; z <= cellCoord.z + 1; ++z)
				{
					for (int32_t y = cellCoord.y; y <= cellCoord.y + 1; ++y)
					{
						for (int32_t x = cellCoord.x - 1; x <= cellCoord.x + 1; ++x)
						{
							if (x < 0 || y < 0 || z < 0 || x >= grid.size.x || y >= grid.size.y || z >= grid.size.z)
								continue;

							const size_t ci2 = x + y * width + z * square;
							const Grid::Cell &cell2 = grid.cells[ci2];

							for (int32_t si2 = 0; si2 < cell2.count; ++si2)
							{
								const size_t pi1 = cell1.slots[si1], pi2 = cell2.slots[si2];

								if (pi1 == pi2)
									continue;

								self->resolveParticles(particles[pi1], particles[pi2]);
							}
						}
					}
				}
			}
		}
	};

	if (singleThread)
		routine(this, grid, particles, 0, cellsCount);
	else
	{
		for (int32_t t = 0; t < tasksCount; ++t)
		{
			const size_t offset = t * batchSize;

			futures[t] = threadPool.pushTask(
				routine, this, std::ref(grid), std::ref(particles), offset, std::min(batchSize, cellsCount - offset));
		}

		for (auto &future : futures)
			future.wait();
	}
}

void Cloud::resolveParticles(Particle &p1, Particle &p2)
{
	glm::vec3 pv = p1.position - p2.position;
	const float distance = glm::length(pv), bounce = .5f;

	if (distance == 0.f)
		return;

	if (distance < 1.f)
	{
		float depth = (1.f - distance) * .5f;

		pv = glm::normalize(pv);

		pushParticle(p1, pv * depth);
		pushParticle(p2, pv * -depth);

		float exchange = ((1.f + bounce) * glm::dot(p1.delta - p2.delta, pv)) * .5f;

		if (exchange > 1.f)
		{
			p1.delta += pv * exchange;
			p2.delta -= pv * exchange;
		}
	}
}

void Cloud::resolveBounds(bool singleThread)
{
	auto collidePlane = [this](Particle &particle, const glm::vec3 &o, const glm::vec3 &normal) {
		const float depth = glm::dot(particle.position - o, normal);

		if (depth < 0.f)
		{
			pushParticle(particle, -normal * depth);
			particle.delta -= 0.5f * depth * normal;
		}
	};
	const glm::vec3 boxSize(grid.size);

	for (Particle &particle : particles)
	{
		const std::tuple<glm::vec3, glm::vec3> planes[] = {
			{glm::vec3(.5f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f)},
			{glm::vec3(boxSize.x - .5f, 0.f, 0.f), glm::vec3(-1.f, 0.f, 0.f)},
			{glm::vec3(0.f, .5f, 0.f), glm::vec3(0.f, 1.f, 0.f)},
			{glm::vec3(0.f, boxSize.y - .5f, 0.f), glm::vec3(0.f, -1.f, 0.f)},
			{glm::vec3(0.f, 0.f, .5f), glm::vec3(0.f, 0.f, 1.f)},
			{glm::vec3(0.f, 0.f, boxSize.z - .5f), glm::vec3(0.f, 0.f, -1.f)}};

		for (auto [o, normal] : planes)
			collidePlane(particle, o, normal);
	}
}

void Cloud::pushParticle(Particle &p, const glm::vec3 &v)
{
	p.position += v;
	p.delta += v;
};

} // namespace b2::physics
