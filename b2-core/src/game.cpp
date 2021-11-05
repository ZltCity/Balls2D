#include "game.hpp"

namespace b2
{

Game::~Game()
{}

std::unique_ptr<Game> Game::create(const std::string &name, std::shared_ptr<Application> application)
{
	auto game = gameList.find(name);

	return game == gameList.end() ? nullptr : game->second(std::move(application));
}

void Game::registerGame(const std::string &name, Constructor constructor)
{
	gameList[name] = std::move(constructor);
}

std::map<std::string, Game::Constructor> Game::gameList;

} // namespace b2
