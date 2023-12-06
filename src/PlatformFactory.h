#pragma once

#include "SDL2Platform.h"

class PlatformFactory
{
public:
    static SDL2Platform* get();
};
