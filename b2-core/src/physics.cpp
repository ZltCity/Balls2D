#include <thread>

#include <b2/exception.hpp>
#include <b2/logger.hpp>
#include <glm/gtc/random.hpp>

#include "physics.hpp"
#include "threadpool.hpp"

#include "timer.hpp"

namespace b2::physics
{

Particle::Particle(const glm::vec3 &position) : position(position), delta(0.0f), active(true)
{}

ParticleCloud::Cell::Cell() : count(0), slots {}
{}

void ParticleCloud::Cell::reset()
{
	count.store(0);
}

ParticleCloud::Grid::Grid(const glm::ivec3 &size) : size(size)
{
	const size_t linearSize = size.x * size.y * size.z;

	cells = std::vector<Cell>(linearSize);
}

ParticleCloud::ParticleCloud(
	const glm::ivec3 &gridSize, size_t particlesCount, Generator generator, std::shared_ptr<ThreadPool> threadPool)
	: grid(gridSize), particles(particlesCount), generator(std::move(generator)), threadPool(std::move(threadPool))
{
	for (size_t i = 0; i < particlesCount; ++i)
		particles[i] = this->generator(i);
}

void ParticleCloud::update(const glm::vec3 &acceleration, float dt, bool singleThread)
{
	moveParticles(acceleration, dt, singleThread);

	for (size_t i = 0; i < solverIterations; ++i)
	{
		resolveBounds(singleThread);
		fill(singleThread);
		resolve(singleThread);
	}
}

glm::ivec3 ParticleCloud::getGridSize() const
{
	return grid.size;
}

const std::vector<Particle> &ParticleCloud::getParticles() const
{
	return particles;
}

void ParticleCloud::moveParticles(const glm::vec3 &acceleration, float dt, bool singleThread)
{
	const size_t workersCount = threadPool->getWorkersCount(), particlesCount = particles.size(),
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
		for (size_t i = 0; i < workersCount; ++i)
			futures[i] = threadPool->pushTask(routine, std::ref(particles), acceleration, i * batchSize, batchSize, dt);

		for (auto &future : futures)
			future.wait();
	}
}

void ParticleCloud::fill(bool singleThread)
{
	for (auto &cell : grid.cells)
		cell.reset();

	const size_t workersCount = threadPool->getWorkersCount(), particlesCount = particles.size(),
				 batchSize = particles.size() / workersCount;
	std::future<void> futures[workersCount];
	auto routine = [this](Grid &grid, std::vector<Particle> &particles, size_t offset, size_t count) {
		const int32_t width = grid.size.x, square = width * grid.size.y;

		for (size_t i = offset; i < offset + count; ++i)
		{
			auto &particle = particles[i];
			const glm::vec3 &position = particle.position;
			const glm::ivec3 cellCoord(position);
			const size_t cellIdx = cellCoord.x + cellCoord.y * width + cellCoord.z * square;

			if (cellIdx >= grid.cells.size())
				particle.active = false;

			if (!particle.active)
				continue;

			Cell &cell = grid.cells[cellIdx];
			const uint8_t cellFilling = cell.count++;

			_assert(cellFilling < cellCapacity, 0x4f62c1fa);

			cell.slots[cellFilling] = i;
		}
	};

	if (singleThread)
		routine(grid, particles, 0, particlesCount);
	else
	{
		for (size_t i = 0; i < workersCount; ++i)
			futures[i] = threadPool->pushTask(routine, std::ref(grid), std::ref(particles), i * batchSize, batchSize);

		for (auto &future : futures)
			future.wait();
	}
}

void ParticleCloud::resolve(bool singleThread)
{
	const size_t cellsCount = grid.cells.size(), batchSize = 1024, tasksCount = (cellsCount / batchSize) + 1;
	std::future<void> futures[tasksCount];
	auto routine = [](ParticleCloud *self, Grid &grid, std::vector<Particle> &particles, size_t offset, size_t count) {
		const int32_t width = grid.size.x, square = width * grid.size.y;

		for (size_t ci1 = offset; ci1 < offset + count; ++ci1)
		{
			const Cell &cell1 = grid.cells[ci1];
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
							const Cell &cell2 = grid.cells[ci2];

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

			futures[t] = threadPool->pushTask(
				routine, this, std::ref(grid), std::ref(particles), offset, std::min(batchSize, cellsCount - offset));
		}

		for (auto &future : futures)
			future.wait();
	}
}

void ParticleCloud::resolveParticles(Particle &p1, Particle &p2)
{
	glm::vec3 pv = p1.position - p2.position;
	float distance = glm::length(pv), bounce = 0.5f;

	if (distance < 1.0f)
	{
		float depth = (1.0f - distance) * 0.5f;

		if (distance == 0.0f)
			pv = glm::sphericalRand(1.0f);
		else
			pv /= distance;

		pushParticle(p1, pv * depth);
		pushParticle(p2, pv * -depth);

		float exchange = ((1.0f + bounce) * glm::dot(p1.delta - p2.delta, pv)) * 0.5f;

		if (exchange > 1.0f)
		{
			p1.delta += pv * exchange;
			p2.delta -= pv * exchange;
		}
	}
}

void ParticleCloud::resolveBounds(bool singleThread)
{
	auto collidePlane = [this](Particle &particle, const glm::vec3 &o, const glm::vec3 &normal) {
		const float distance = glm::dot(particle.position - o, normal);

		if (distance <= 0.5f)
		{
			const float depth = 0.5f - distance;

			pushParticle(particle, normal * depth);
			particle.delta += 0.5f * depth * normal;
		}
	};
	const glm::vec3 boxSize(grid.size);

	for (Particle &particle : particles)
	{
		const std::tuple<glm::vec3, glm::vec3> planes[] = {
			{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
			{glm::vec3(boxSize.x, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
			{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			{glm::vec3(0.0f, boxSize.y, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
			{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
			{glm::vec3(0.0f, 0.0f, boxSize.z), glm::vec3(0.0f, 0.0f, -1.0f)}};

		for (const auto &[o, normal] : planes)
			collidePlane(particle, o, normal);
	}
}

void ParticleCloud::pushParticle(Particle &p, const glm::vec3 &v)
{
	p.position += v;
	p.delta += v;
};

} // namespace b2::physics
