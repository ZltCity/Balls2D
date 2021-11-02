#include <string>

#include <b2/exception.hpp>

namespace b2
{

std::string errorBrief(uint32_t code)
{
	switch (code)
	{
		case 0x071fb7d9: return {"Index is out of the range."};
		case 0x461f5dfa: return {"Input buffer is empty."};
		case 0x0f77c02e: return {"File open error."};
		case 0x1e49e9a9: return {"IO error."};
		case 0x620a358f: return {"Invalid workers count."};
		case 0x5fd881d9: return {"Unable to get sensor manager."};
		case 0x37753fba: return {"Could not find accelerometer."};
		case 0xcc5b7924: return {"Unable to create sensor event queue."};
		case 0xae3cf11b: return {"Could not enable accelerometer."};
		case 0x8ea6aefc: return {"Unable to get display connection."};
		case 0xe18e3ae1: return {"Unable to create window."};
		case 0xd6a07fa8: return {"Unable to create rendering context."};
		case 0x0ba4af75: return {"Unable to initialize display connection."};
		case 0xf7669c12: return {"Unable to choose EGL config."};
		case 0x238112b7: return {"There is no suitable EGL config."};
		case 0xc639ca32: return {"Unable to create EGL surface."};
		case 0x5d236635: return {"Unable to set rendering context."};
		case 0x13182aca: return {"Uninitialized GAPI object."};
		case 0xa2f0db57: return {"Undefined texture format."};
		case 0x9800a19c: return {"GL_INVALID_ENUM."};
		case 0x636e76a1: return {"GL_INVALID_VALUE."};
		case 0x02f3f490: return {"GL_INVALID_OPERATION."};
		case 0x8e699b5d: return {"GL_OUT_OF_MEMORY."};
		case 0xd78eead8: return {"Invalid config value."};
		case 0x4f62c1fa: return {"Cell capacity exceeded."};
		default: return {"Undefined error."};
	}
}

} // namespace b2
