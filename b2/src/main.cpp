#include <memory>

#include "game.hpp"
#include "logger.hpp"
#include "platform/eventloop.hpp"
#include "platform/io.hpp"

//#include <fenv.h>

namespace b2
{

void main(std::shared_ptr<platform::EventLoop> eventLoop, std::shared_ptr<platform::IO> io)
{
	using namespace platform;

	//	feenableexcept(FE_DIVBYZERO);

	bool quitRequest = false;
	std::unique_ptr<Game> game;
	auto eventHandler = [&quitRequest, &game, eventLoop, io](const Event &event) {
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
				game = std::make_unique<Game>(std::get<std::shared_ptr<RenderContext>>(event.payload), io);

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
	};

	eventLoop->setEventHandler(eventHandler);

	while (!quitRequest)
	{
		if (game != nullptr)
			game->update();

		eventLoop->nextTick();
	}
}

} // namespace b2
