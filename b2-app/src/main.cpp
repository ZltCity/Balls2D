#include "desktopapp.hpp"

int main(int argc, const char **argv)
{
	b2::main(std::make_shared<b2::desktop::DesktopApplication>());

	return 0;
}
