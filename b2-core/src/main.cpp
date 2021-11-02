#include <memory>

#include <b2/application.hpp>

#include "game.hpp"
#include "logger.hpp"

namespace b2
{

void main(std::shared_ptr<Application> application)
{
	bool quitRequest = false;
	std::unique_ptr<Game> game;

	while (!quitRequest)
	{
		Event event {};

		while ((event = application->pollEvent()).type != Event::Empty)
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
					game = std::make_unique<Game>(application);
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
					return;
				}
			}
		}

		if (game != nullptr)
			game->update();
	}
}

} // namespace b2
