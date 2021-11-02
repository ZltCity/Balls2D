#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "logger.hpp"
#include "threadpool.hpp"

namespace b2
{

class Isosurface
{
public:
	struct MeshVertex
	{
		glm::vec3 position, normal;
	};

	Isosurface() = default;
	Isosurface(const glm::ivec3 &fieldSize, std::shared_ptr<ThreadPool> threadPool);

	template<typename Particle>
	std::vector<MeshVertex> &generateMesh(
		const std::vector<Particle> &particles, uint32_t radius, bool singleThread = true);

private:
	struct SpacePoint
	{
		glm::vec3 normal;
		float value;
	};

	template<typename Particle>
	void generateScalarField(const std::vector<Particle> &particles, uint32_t radius, bool singleThread = true);
	void generateNormals(bool singleThread);

	static const int32_t edgesTable[256];
	static const int32_t trianglesTable[256][16];
	static const float threshold;

	std::vector<MeshVertex> mesh;
	std::vector<SpacePoint> field;
	glm::ivec3 fieldSize;
	std::shared_ptr<ThreadPool> threadPool;
};

template<typename Particle>
std::vector<Isosurface::MeshVertex> &Isosurface::generateMesh(
	const std::vector<Particle> &particles, uint32_t radius, bool singleThread)
{
	auto getIndex = [this](const glm::ivec3 &s) -> int32_t {
		return s.x + s.y * fieldSize.x + s.z * fieldSize.x * fieldSize.y;
	};
	auto lerp = [](const std::tuple<SpacePoint, glm::vec3> &s1,
				   const std::tuple<SpacePoint, glm::vec3> &s2) -> std::tuple<glm::vec3, glm::vec3> {
		auto [sp1, p1] = s1;
		auto [sp2, p2] = s2;
		float factor = (threshold - sp1.value) / (sp2.value - sp1.value);

		return std::make_tuple(p1 + (p2 - p1) * factor, sp1.normal * (1.0f - factor) + sp2.normal * factor);
	};

	mesh.clear();

	generateScalarField(particles, radius, singleThread);
	generateNormals(singleThread);

	for (int32_t z = 0; z < fieldSize.z - 1; ++z)
		for (int32_t y = 0; y < fieldSize.y - 1; ++y)
			for (int32_t x = 0; x < fieldSize.x - 1; ++x)
			{
				const std::tuple<SpacePoint, glm::vec3> scalars[] = {
					{field[getIndex({x, y, z})], glm::vec3(x, y, z)},
					{field[getIndex({x + 1, y, z})], glm::vec3(x + 1, y, z)},
					{field[getIndex({x + 1, y, z + 1})], glm::vec3(x + 1, y, z + 1)},
					{field[getIndex({x, y, z + 1})], glm::vec3(x, y, z + 1)},
					{field[getIndex({x, y + 1, z})], glm::vec3(x, y + 1, z)},
					{field[getIndex({x + 1, y + 1, z})], glm::vec3(x + 1, y + 1, z)},
					{field[getIndex({x + 1, y + 1, z + 1})], glm::vec3(x + 1, y + 1, z + 1)},
					{field[getIndex({x, y + 1, z + 1})], glm::vec3(x, y + 1, z + 1)}};
				int32_t config = 0x00;

				if (std::get<0>(scalars[0]).value > threshold)
					config |= 0x01;
				if (std::get<0>(scalars[1]).value > threshold)
					config |= 0x02;
				if (std::get<0>(scalars[2]).value > threshold)
					config |= 0x04;
				if (std::get<0>(scalars[3]).value > threshold)
					config |= 0x08;
				if (std::get<0>(scalars[4]).value > threshold)
					config |= 0x10;
				if (std::get<0>(scalars[5]).value > threshold)
					config |= 0x20;
				if (std::get<0>(scalars[6]).value > threshold)
					config |= 0x40;
				if (std::get<0>(scalars[7]).value > threshold)
					config |= 0x80;

				if (edgesTable[config] == 0x00)
					continue;

				std::tuple<glm::vec3, glm::vec3> vertices[12] = {};

				if (edgesTable[config] & 0x01)
					vertices[0] = lerp(scalars[0], scalars[1]);
				if (edgesTable[config] & 0x02)
					vertices[1] = lerp(scalars[1], scalars[2]);
				if (edgesTable[config] & 0x04)
					vertices[2] = lerp(scalars[2], scalars[3]);
				if (edgesTable[config] & 0x08)
					vertices[3] = lerp(scalars[3], scalars[0]);
				if (edgesTable[config] & 0x10)
					vertices[4] = lerp(scalars[4], scalars[5]);
				if (edgesTable[config] & 0x20)
					vertices[5] = lerp(scalars[5], scalars[6]);
				if (edgesTable[config] & 0x40)
					vertices[6] = lerp(scalars[6], scalars[7]);
				if (edgesTable[config] & 0x80)
					vertices[7] = lerp(scalars[7], scalars[4]);
				if (edgesTable[config] & 0x0100)
					vertices[8] = lerp(scalars[0], scalars[4]);
				if (edgesTable[config] & 0x0200)
					vertices[9] = lerp(scalars[1], scalars[5]);
				if (edgesTable[config] & 0x0400)
					vertices[10] = lerp(scalars[2], scalars[6]);
				if (edgesTable[config] & 0x0800)
					vertices[11] = lerp(scalars[3], scalars[7]);

				for (int32_t i = 0; trianglesTable[config][i] != -1; i += 3)
				{
					auto [p1, n1] = vertices[trianglesTable[config][i]];
					auto [p2, n2] = vertices[trianglesTable[config][i + 1]];
					auto [p3, n3] = vertices[trianglesTable[config][i + 2]];

					mesh.push_back({p1, n1});
					mesh.push_back({p2, n2});
					mesh.push_back({p3, n3});
				}
			}

	return mesh;
}

template<typename Particle>
void Isosurface::generateScalarField(const std::vector<Particle> &particles, uint32_t radius, bool singleThread)
{
	std::fill(field.begin(), field.end(), SpacePoint({glm::vec3(0.0f), 0.0f}));

	const size_t particlesCount = particles.size(), workersCount = threadPool->getWorkersCount(),
				 batchSize = particlesCount / workersCount;
	std::future<void> futures[workersCount];
	auto routine = [this, radius](const std::vector<Particle> &particles, size_t offset, size_t count) {
		const int32_t square = fieldSize.x * fieldSize.y;

		for (size_t i = offset; i < offset + count; ++i)
		{
			const glm::vec3 position(particles[i].position + glm::vec3(radius + 1));
			const glm::ivec3 coord(glm::roundEven(position));

			for (int32_t z = coord.z - radius; z <= coord.z + radius; ++z)
				for (int32_t y = coord.y - radius; y <= coord.y + radius; ++y)
					for (int32_t x = coord.x - radius; x <= coord.x + radius; ++x)
					{
						if (x < 0 || y < 0 || z < 0 || x >= fieldSize.x || y >= fieldSize.y || z >= fieldSize.z)
							continue;

						const int32_t index = x + y * fieldSize.x + z * square;
						float distance = glm::length(glm::vec3(x, y, z) - position),
							  factor = distance / (float(radius) * 1.5f);

						field[index].value += 1.0f - factor;
					}
		}
	};

	if (singleThread)
		routine(particles, 0, particlesCount);
	else
	{
		for (int32_t t = 0; t < workersCount; ++t)
			futures[t] = threadPool->pushTask(routine, std::ref(particles), t * batchSize, batchSize);

		for (auto &future : futures)
			future.wait();
	}
}

} // namespace b2
