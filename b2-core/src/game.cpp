#include <random>

#include <b2/bytebuffer.hpp>
#include <b2/exception.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <nlohmann/json.hpp>

#include "config.hpp"
#include "game.hpp"
#include "isosurface.hpp"
#include "logger.hpp"
#include "render.hpp"

namespace b2
{

const char *const Game::configPath = "configs/game.json";

Game::Game(std::shared_ptr<Application> application)
	: application(application),
	  acceleration(glm::vec3(0.0f, -9.8f, 0.0f)),
	  singleThread(true),
	  projection(1.0f),
	  elapsed(0.0f)
{
	using json = nlohmann::json;

	const Config config(application->readFile(configPath));
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

void Game::update()
{
	using namespace gl;

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
		info("Physics: %f, Render: %f", pTime / float(frames), rTime / float(frames));

		frames = 0;
		elapsed = pTime = rTime = 0.0f;
	}
}

void Game::onSensorsEvent(const glm::vec3 &acceleration)
{
	this->acceleration.store(acceleration);
}

void Game::initLogic(const glm::ivec2 &surfaceSize, size_t gridWidth, size_t particlesCount)
{
	_assert(gridWidth > 0, 0xd78eead8);
	_assert(particlesCount > 0, 0xd78eead8);

	gridSize = glm::ivec3(gridWidth, int32_t(gridWidth * float(surfaceSize.y) / surfaceSize.x), gridWidth);

	particlesCloud = physics::ParticleCloud(
		gridSize, particlesCount,
		[this]() -> physics::Particle {
			std::default_random_engine generator(Timer::getTimestamp());
			std::uniform_real_distribution<float> x(0.5f, float(gridSize.x) - 0.5f), y(0.5f, float(gridSize.y) - 0.5f),
				z(0.5f, float(gridSize.z) - 0.5f);

			return physics::Particle(glm::vec3(x(generator), y(generator), z(generator)));
		},
		threadPool);
	//	isosurface = Isosurface(gridSize + glm::ivec3(margin));
}

void Game::initRender(const glm::ivec2 &surfaceSize)
{
	using namespace gl;

	const Shader vertexShader(ShaderType::Vertex, application->readFile("shaders/particle.vs")),
		fragmentShader(ShaderType::Fragment, application->readFile("shaders/particle.fs"));

	shaderProgram = ShaderProgram({vertexShader, fragmentShader});
	projection = camera.getPerspective(75.0f, float(surfaceSize.x) / surfaceSize.y, 1000.f);

	camera.lookAt(glm::vec3(.0f, 0.0f, -100.f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0));

	surfaceMesh = render::BasicMesh(
		particlesCloud.getParticles(),
		{{3, sizeof(physics::Particle), render::VertexAttribute::Float},
		 {3, sizeof(physics::Particle), render::VertexAttribute::Float}},
		render::BasicMesh::DynamicDraw);
	this->surfaceSize = surfaceSize;
}

void Game::updatePhysics()
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
	crit("Error occurred: %s", ex.what());
	std::terminate();
}

void Game::presentScene()
{
	using namespace gl;

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
	surfaceMesh.update(particles);
	surfaceMesh.bind();
//	surfaceVertices.bind();
//	setVertexFormat(std::vector<VertexAttrib>());

	shaderProgram.use();

	Mat4Uniform("in_projection", projection).set(shaderProgram);
	Mat4Uniform("in_modelview", camera.getView() * glm::translate(glm::mat4(1.f), -boxSize * 0.5f)).set(shaderProgram);
	Vec2Uniform("in_surface_size", surfaceSize).set(shaderProgram);
	FloatUniform("in_point_size", 2.0f).set(shaderProgram);

	_i(glEnable, GL_DEPTH_TEST);

	setClearColor({.5f, .6f, .4f, 1.f});
	clear(ClearMode::Color | ClearMode::Depth);
	draw(DrawMode::Points, particles.size());

	application->swapBuffers();
}

} // namespace b2
