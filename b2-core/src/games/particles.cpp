#include <iostream>
#include <random>

#include <b2/bytebuffer.hpp>
#include <b2/exception.hpp>
#include <b2/logger.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <nlohmann/json.hpp>

#include "../config.hpp"
#include "../isosurface.hpp"
#include "../render.hpp"
#include "../utils.hpp"
#include "particles.hpp"

namespace b2::games
{

const char *const ParticlesGame::configPath = "configs/game.json";

ParticlesGame::ParticlesGame(std::shared_ptr<Application> application)
	: application(application),
	  acceleration(glm::vec3(0.0f, -9.8f, 0.0f)),
	  singleThread(true),
	  materials(render::loadMaterials("materials/")),
	  projection(1.0f),
	  elapsed(0.0f)
{
	using json = nlohmann::json;

	const Config config(readFile(configPath));
	const json physicsConfig = config.json.at("physics");

	singleThread.store(config.json.at("singleThread").get<bool>());

	if (!singleThread)
		threadPool = std::make_shared<ThreadPool>();

	const auto surfaceSize = application->getWindowSize();

	initLogic(
		surfaceSize, physicsConfig.at("gridSize").at("width").get<size_t>(),
		physicsConfig.at("particlesCount").get<size_t>());
	initRender(surfaceSize);
}

void ParticlesGame::update()
{
	size_t frames = 0;
	float pTime = 0.0f, rTime = 0.0f;
	Timer localTimer;

	updatePhysics();
	pTime += localTimer.getDeltaMs();

	presentScene();
	rTime += localTimer.getDeltaMs();

	++frames;
	elapsed += globalTimer.getDeltaMs();

	if (elapsed >= 1000.0f)
	{
		info1(fmt::format("Physics: {}, Render: {}", pTime / float(frames), rTime / float(frames)));

		frames = 0;
		elapsed = pTime = rTime = 0.0f;
	}
}

void ParticlesGame::onSensorsEvent(const glm::vec3 &acceleration)
{
	this->acceleration.store(acceleration);
}

void ParticlesGame::initLogic(const glm::ivec2 &surfaceSize, size_t gridWidth, size_t particlesCount)
{
	_assert(gridWidth > 0, 0xd78eead8);
	_assert(particlesCount > 0, 0xd78eead8);

	gridSize = glm::ivec3(gridWidth, int32_t(gridWidth * float(surfaceSize.y) / surfaceSize.x), gridWidth);

	particlesCloud = physics::ParticleCloud(
		gridSize, particlesCount,
		[this](size_t idx) -> physics::Particle {
			//			std::default_random_engine generator(Timer::getTimestamp());
			//			std::uniform_real_distribution<float> x(0.5f, float(gridSize.x) - 0.5f), y(0.5f,
			// float(gridSize.y)
			//- 0.5f), 				z(0.5f, float(gridSize.z) - 0.5f);
			//
			//			return physics::Particle(glm::vec3(x(generator), y(generator), z(generator)));
			auto square = gridSize.x * gridSize.y;
			auto x = (idx % square) % gridSize.x, y = (idx % square) / gridSize.x, z = idx / square;

			return physics::Particle(glm::vec3 {x, z * 2.0f, y} + glm::vec3 {0.5f, 0.5f, 0.5f});
		},
		threadPool);
	//	isosurface = Isosurface(gridSize + glm::ivec3(margin));
}

void ParticlesGame::initRender(const glm::ivec2 &surfaceSize)
{
	projection = camera.getPerspective(75.0f, float(surfaceSize.x) / surfaceSize.y, 1000.f);

	camera.lookAt(glm::vec3(.0f, 0.0f, -100.f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0));

	surfaceMesh = render::BasicMesh(
		particlesCloud.getParticles(),
		{{3, sizeof(physics::Particle), render::VertexAttribute::Float},
		 {3, sizeof(physics::Particle), render::VertexAttribute::Float}},
		render::BasicMesh::DynamicDraw);

	this->surfaceSize = surfaceSize;
}

void ParticlesGame::updatePhysics()
try
{
	particlesCloud.update(acceleration.load(), 0.01f, singleThread);

	//	ToDo: remove it!
	static float angle = 0.0f;
	static Timer timer;
	auto gravity = glm::vec4 {0.0f, -9.81f, 0.0f, 0.0f};
	auto transform = glm::rotate(glm::mat4 {1}, angle, glm::vec3 {0.0f, 0.0f, 1.0f});

	gravity = gravity * transform;
	angle += timer.getDeltaMs() * 0.0001f;
	acceleration = gravity;
}
catch (const std::exception &ex)
{
	error(fmt::format("Error occurred: {}", ex.what()));
	std::terminate();
}

void ParticlesGame::presentScene()
{
	const glm::vec3 boxSize(gridSize /* + glm::ivec3(margin)*/);
	const auto &particles = particlesCloud.getParticles();

	//	if (!surfaceVertices || surfaceVertices.getSize() < particles.size() * sizeof(physics::Particle))
	//	{
	//		surfaceVertices = Buffer(BufferType::Vertex, particles);
	//	}
	//	else
	//	{
	//		surfaceVertices.bind();
	//		surfaceVertices.write(0, particles);
	//	}
	auto material = materials.get("particles");

	surfaceMesh.update(particles);
	surfaceMesh.bind();
	material->bind();

	render::Uniform("in_projection", projection).set(*material);
	render::Uniform("in_modelview", camera.getView() * glm::translate(glm::mat4(1.f), -boxSize * 0.5f)).set(*material);
	render::Uniform("in_surface_size", surfaceSize).set(*material);

	render::gles3::_i(glEnable, GL_DEPTH_TEST);

	render::gles3::_i(glClearColor, .5f, .6f, .4f, 1.f);
	render::gles3::_i(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	render::gles3::_i(glDrawArrays, GL_POINTS, 0, particles.size());

	application->swapBuffers();
}

} // namespace b2::games
