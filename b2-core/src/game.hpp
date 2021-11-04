#pragma once

#include <mutex>
#include <thread>

#include <b2/application.hpp>

#include "camera.hpp"
#include "gearbox.hpp"
#include "isosurface.hpp"
#include "physics.hpp"
#include "render.hpp"
#include "timer.hpp"

namespace b2
{

class Game
{
public:
	Game(std::shared_ptr<Application> application);
	Game(const Game &) = delete;

	Game &operator=(const Game &) = delete;

	void update();

	void onSensorsEvent(const glm::vec3 &acceleration);

	static const char *const configPath;
	static const uint32_t radius = 2, margin = (radius + 1) * 2;

private:
	using SurfaceMesh = std::vector<Isosurface::MeshVertex>;

	void initLogic(const glm::ivec2 &surfaceSize, size_t gridWidth, size_t particlesCount);
	void initRender(const glm::ivec2 &surfaceSize);
	void updatePhysics();
	void presentScene();

	std::shared_ptr<Application> application;

	std::atomic<glm::vec3> acceleration;

	physics::ParticleCloud particlesCloud;
	//	Isosurface isosurface;

	std::atomic_bool singleThread;
	std::shared_ptr<ThreadPool> threadPool;

	render::BasicMesh surfaceMesh;
	render::Material material;

	glm::ivec3 gridSize;
	Camera camera;
	glm::mat4 projection;
	glm::ivec2 surfaceSize;
	Timer globalTimer;
	float elapsed;
};

} // namespace b2
