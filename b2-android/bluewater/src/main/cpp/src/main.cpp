#include <b2/logger.hpp>

#include "eventloop.hpp"
#include "io.hpp"

namespace b2
{

void main(std::shared_ptr<platform::EventLoop>, std::shared_ptr<platform::IO> io);

}

void android_main(android_app *androidApp)
try
{
	b2::main(
		std::make_shared<b2::android::AndroidEventLoop>(androidApp),
		std::make_shared<b2::android::AndroidIO>(androidApp));
}
catch (const std::exception &ex)
{
	b2::crit("%s", ex.what());

	throw;
}
