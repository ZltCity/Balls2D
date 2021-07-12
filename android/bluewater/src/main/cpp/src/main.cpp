#include <stdexcept>

#include <android_native_app_glue.h>

namespace b2::android
{

void run(android_app *);

}

void android_main(android_app *androidApp)
try
{
	b2::android::run(androidApp);
}
// catch (const bluewater::Exception &ex)
//{
//	throw;
// }
catch (const std::exception &ex)
{
	throw;
}
