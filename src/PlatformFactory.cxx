#include "PlatformFactory.h"

#ifdef USE_GLFW
#include "GLFWPlatform.h"
#else
#include "SDL2Platform.h"
#endif

BzfPlatform* PlatformFactory::get()
{
    static BzfPlatform* platform = nullptr;

    if (platform == nullptr)
#ifdef USE_GLFW
        platform = new GLFWPlatform();
#else
        platform = new SDL2Platform();
#endif

    return platform;
}
