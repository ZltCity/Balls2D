#include <memory>

#include "gl.hpp"
#include "logger.hpp"
#include "platform.hpp"

namespace b2
{

void entry(std::shared_ptr<Platform> platform)
{
	while (true)
	{
		for (const Event &event : platform->pollEvents())
		{
			switch (event.type)
			{
				case Event::TouchEvent:
				{
					const Touch &touch = std::get<Touch>(event.payload);

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
					const glm::vec3 &a = std::get<glm::vec3>(event.payload);

					info("%f, %f, %f", a.x, a.y, a.z);

					break;
				}

				case Event::QuitRequest: return;
			}
		}

		gl::setClearColor({0.45f, 0.6f, 0.35f, 1.0f});
		gl::clear(gl::ClearMode::Color);

		platform->updateDisplay();
	}
}

} // namespace b2
