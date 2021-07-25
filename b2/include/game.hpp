#pragma once

#include <mutex>
#include <thread>

#include "camera.hpp"
#include "gearbox.hpp"
#include "gl.hpp"
#include "isosurface.hpp"
#include "physics.hpp"
#include "platform.hpp"

namespace b2
{

class Game
{
public:
	Game(std::shared_ptr<Platform> platform, const glm::ivec2 &surfaceSize);
	Game(const Game &) = delete;
	~Game();

	Game &operator=(const Game &) = delete;

	void update();

	void onSensorsEvent(const glm::vec3 &acceleration);

	static const char *const configPath;
	static const uint32_t radius = 2, margin = (radius + 1) * 2;

private:
	using SurfaceMesh = std::vector<Isosurface::MeshVertex>;

	void initLogicThread(const glm::ivec2 &surfaceSize, size_t gridWidth, size_t particlesCount);
	void initRender(const glm::ivec2 &surfaceSize);
	void presentScene();

	static void logicRoutine(Game *self);

	std::shared_ptr<Platform> platform;

	Gearbox<SurfaceMesh> mesh;

	std::atomic<glm::vec3> acceleration;

	physics::Cloud particlesCloud;
	Isosurface isosurface;

	std::thread logicThread;
	std::atomic_bool alive, singleThread;

	gl::Buffer surfaceVertices;
	gl::ShaderProgram shaderProgram;

	glm::ivec3 gridSize;
	Camera camera;
	glm::mat4 projection;
};

} // namespace b2
