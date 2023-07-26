#pragma

#include <vector>

#include "../game.hpp"
#include "../render.hpp"

namespace b2::games
{

class ShapesGame : public Game
{
public:
	explicit ShapesGame(std::shared_ptr<Application> application);
	ShapesGame(const ShapesGame &) = delete;

	ShapesGame &operator=(const ShapesGame &) = delete;

	void update() final;
	void onSensorsEvent(const glm::vec3 &acceleration) final;

private:
	std::shared_ptr<Application> application;

	std::vector<render::BasicMesh> meshes;
	render::Material material;
};

} // namespace b2::games
