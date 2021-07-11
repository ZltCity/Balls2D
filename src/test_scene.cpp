#include <random>

#include <glm/gtc/matrix_transform.hpp>

#include "logger.hpp"
#include "test_scene.hpp"
#include "timer.hpp"

namespace b2
{

// TestScene::TestScene() : system(nullptr), gapi(nullptr)
//{}
//
// TestScene::TestScene(System *system, GAPI *gapi)
//	: system(system), gapi(gapi), grid(glm::ivec3(32, 32, 32))
//{
//	const glm::vec3 boxSize(grid.getResolution());
//	std::default_random_engine generator;
//	std::uniform_real_distribution<float> x(.5f, boxSize.x - .5f), y(.5f, boxSize.y - .5f),
//		z(.5f, boxSize.z - .5f);
//
//	for (int32_t i = 0; i < particlesCount; ++i)
//	{
//		glm::vec3 position(x(generator), y(generator), z(generator));
//
//		particles.push_back(physics::Particle({position, glm::vec3(.0f), 1.f}));
//	}
//
//	particlesBuffer = gapi->createBuffer<physics::Particle>(BufferType::Vertex, particles);
//
//	Shader v = gapi->createShader(ShaderType::Vertex, system->readFile("shaders/simple.v")),
//		   f = gapi->createShader(ShaderType::Fragment, system->readFile("shaders/simple.f"));
//
//	program = gapi->createShaderProgram({v, f});
//
//	model = glm::translate(glm::mat4(1.f), -boxSize * 0.5f);
//	camera.lookAt(
//		glm::vec3(.0f, 15.0f, -60.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0));
// }
//
// void TestScene::update()
//{
//	const glm::vec2 surfaceSize = gapi->getSurfaceSize();
//	float aspect = surfaceSize.x / surfaceSize.y, fovy = 75.f;
//
//	if (aspect > 1.f)
//		fovy /= aspect;
//
//	Timer timer;
//
//	for (int32_t i = 0; i < 2; ++i)
//	{
//		grid.fill(particles);
//		grid.resolve(particles);
//		physics::moveParticles(particles, glm::vec3(0.f, -15.f, 0.f), 0.01f);
//	}
//
//	info("%f", timer.getDeltaMs());
//
//	gapi->setClearColor({.5f, .6f, .4f, 1.f});
//	gapi->clear(ClearMode::Color | ClearMode::Depth);
//	program.use();
//	program.setUniform("in_point_size", 2.0f);
//	program.setUniform("in_surface_size", gapi->getSurfaceSize());
//	program.setUniform("in_projection", camera.getPerspective(fovy, aspect, 1000.f));
//	program.setUniform("in_modelview", camera.getView() * model);
//	particlesBuffer.bind();
//	particlesBuffer.write(0, particles);
//	gapi->enable(RenderFeature::DepthTest);
//	// gapi->enable(Feature::Blend);
//	// gapi->setBlendFunc(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);
//	gapi->setVertexFormat(
//		std::vector<VertexAttrib>({{3, sizeof(physics::Particle), AttribType::Float}}));
//	gapi->draw(DrawMode::Points, particlesCount);
// }
//
// void TestScene::rotate(float angle)
//{
//	camera.rotate(angle * 0.1f);
// }

} // namespace b2
