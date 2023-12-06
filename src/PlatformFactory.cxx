#include "PlatformFactory.h"

#include "SDL2Platform.h"

SDL2Platform* PlatformFactory::get()
{
    static SDL2Platform* platform = nullptr;

    if (platform == nullptr)
        platform = new SDL2Platform();

    return platform;
}
