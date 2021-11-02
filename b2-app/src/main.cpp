#include <threadpool.hpp>

#include "desktopapp.hpp"

int main(int argc, const char **argv)
{
	b2::main(std::make_shared<b2::desktop::DesktopApplication>());
	b2::ThreadPool::getInstance().stop();

	return 0;
}
