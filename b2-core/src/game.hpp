#pragma once

#include <functional>
#include <map>
#include <memory>

#include <b2/application.hpp>

namespace b2
{

class Game
{
public:
	using Constructor = std::function<std::unique_ptr<Game>(std::shared_ptr<Application>)>;

	Game() = default;
	Game(const Game &) = delete;
	virtual ~Game() = 0;

	Game &operator=(const Game &) = delete;

	virtual void update() = 0;
	virtual void onSensorsEvent(const glm::vec3 &acceleration) = 0;

	[[nodiscard]] static std::unique_ptr<Game> create(
		const std::string &name, std::shared_ptr<Application> application);
	static void registerGame(const std::string &name, Constructor constructor);

private:
	static std::map<std::string, Constructor> gameList;
};

} // namespace b2
