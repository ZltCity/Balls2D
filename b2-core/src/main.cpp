#include <iostream>
#include <memory>

#include <b2/application.hpp>

#include "game.hpp"
#include "games/particles.hpp"
#include "games/shapes.hpp"
#include "logger.hpp"

namespace b2
{

void registerGames();

void main(std::shared_ptr<Application> application)
{
	registerGames();

	bool quitRequest = false;
	std::unique_ptr<Game> game;

	while (!quitRequest)
	{
		for (const auto &event : application->pollEvents())
		{
			switch (event.type)
			{
				case Event::TouchEvent:
				{
					const auto &touch = std::get<Touch>(event.payload);

					switch (touch.type)
					{
						case Touch::Move: info("Move", 1); break;
						case Touch::Down: info("Down", 1); break;
						case Touch::Up: info("Up", 1); break;
					}

					for (const glm::vec2 &p : touch.points)
						info("%f, %f", p.x, p.y);

					break;
				}
				case Event::AccelerationEvent:
				{
					const auto &a = std::get<glm::vec3>(event.payload);

					if (game != nullptr)
						game->onSensorsEvent({-a.x, -a.y, a.z});

					break;
				}
				case Event::WindowCreated:
				{
					game = Game::create("particles", application);
					break;
				}
				case Event::WindowDestroyed:
				{
					game.reset();
					break;
				}

				case Event::QuitRequest:
				{
					quitRequest = true;
					break;
				}
			}
		}

		if (game != nullptr)
			game->update();
	}
}

void registerGames()
{
	Game::registerGame(
		"particles", [](std::shared_ptr<Application> application) -> auto {
			return std::make_unique<games::ParticlesGame>(std::move(application));
		});
	Game::registerGame(
		"shapes", [](std::shared_ptr<Application> application) -> auto {
			return std::make_unique<games::ShapesGame>(std::move(application));
		});
}

} // namespace b2
