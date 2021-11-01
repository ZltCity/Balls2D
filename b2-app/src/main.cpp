#include <iostream>

#include <SDL.h>
#include <threadpool.hpp>

#include "eventloop.hpp"
#include "io.hpp"

namespace b2
{

void main(std::shared_ptr<platform::EventLoop>, std::shared_ptr<platform::IO> io);

}

int main(int argc, const char **argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	std::unique_ptr<SDL_Window, void (*)(SDL_Window *)> window(
		SDL_CreateWindow(
			"An SDL2 window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 900, 900, SDL_WINDOW_OPENGL),
		[](SDL_Window *window) { SDL_DestroyWindow(window); });

	if (window == nullptr)
	{
		std::cout << "Unable to create window." << std::endl;
		return -1;
	}

	b2::main(
		std::make_shared<b2::desktop::DesktopEventLoop>(std::move(window)), std::make_shared<b2::desktop::DesktopIO>());
	b2::ThreadPool::getInstance().stop();

	SDL_Quit();

	return 0;
}
