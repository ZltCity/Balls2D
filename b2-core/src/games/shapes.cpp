#include "shapes.hpp"
#include "../render.hpp"

namespace b2::games
{

struct SimpleVertex
{
	glm::vec3 position;
	glm::vec4 color;
};

ShapesGame::ShapesGame(std::shared_ptr<Application> application) : application(application)
{
	meshes.emplace_back(
		std::vector<SimpleVertex> {
			{glm::vec3 {-0.5f, -0.5f, 1.0f}, glm::vec4 {1.0f, 0.0f, 0.0f, 1.0f}},
			{glm::vec3 {0.5f, -0.5f, 1.0f}, glm::vec4 {0.0f, 1.0f, 0.0f, 1.0f}},
			{glm::vec3 {0.f, 0.5f, 1.0f}, glm::vec4 {0.0f, 0.0f, 1.0f, 1.0f}}},
		std::vector<render::VertexAttribute> {
			{3, sizeof(SimpleVertex), render::VertexAttribute::Float},
			{4, sizeof(SimpleVertex), render::VertexAttribute::Float},
		});
	material = render::Material(
		{{application->readFile("shaders/simple.vert"), render::Material::Shader::Vertex},
		 {application->readFile("shaders/simple.frag"), render::Material::Shader::Fragment}},
		{});
}

void ShapesGame::update()
{
	render::detail::_i(glClearColor, 0.15f, .15f, .15f, 1.f);
	render::detail::_i(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	material.bind();
	render::Uniform("in_projection", glm::mat4{1}).set(material);
	render::Uniform("in_modelview", glm::mat4{1}).set(material);

	for (const auto &mesh: meshes)
		mesh.bind();

	render::detail::_i(glDrawArrays, GL_TRIANGLES, 0, 3);

	application->swapBuffers();
}

void ShapesGame::onSensorsEvent(const glm::vec3 &acceleration)
{}

} // namespace b2::games
