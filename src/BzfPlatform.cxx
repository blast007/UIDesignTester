#include "BzfPlatform.h"

void BzfPlatform::addResizeCallback(std::function<void(BzfPlatform *, BzfWindow *, int, int)> callback)
{
    resizeCallbacks.push_back(callback);
}

void BzfPlatform::addMoveCallback(std::function<void(BzfPlatform *, BzfWindow *, int, int)> callback)
{
    moveCallbacks.push_back(callback);
}

void BzfPlatform::addKeyCallback(std::function<void(BzfPlatform *, BzfWindow *, BzfKey, BzfKeyAction, int)> callback)
{
    keyCallbacks.push_back(callback);
}

void BzfPlatform::addTextCallback(std::function<void(BzfPlatform *, BzfWindow *, char[32])> callback)
{
    textCallbacks.push_back(callback);
}

void BzfPlatform::addCursorPosCallback(std::function<void(BzfPlatform *, BzfWindow *, double, double)> callback)
{
    cursorPosCallbacks.push_back(callback);
}

void BzfPlatform::addMouseButtonCallback(
    std::function<void(BzfPlatform *, BzfWindow *, BzfMouseButton, BzfButtonAction, int)> callback)
{
    mouseButtonCallbacks.push_back(callback);
}

void BzfPlatform::addScrollCallback(std::function<void(BzfPlatform *, BzfWindow *, double, double)> callback)
{
    scrollCallbacks.push_back(callback);
}

void BzfPlatform::addJoystickButtonCallback(
    std::function<void(BzfPlatform *, BzfWindow *, BzfJoyButton, BzfButtonAction)> callback)
{
    joystickButtonCallbacks.push_back(callback);
}

void BzfPlatform::addJoystickHatCallback(std::function<void(BzfPlatform *, BzfWindow *, BzfJoyHat, BzfJoyHatDirection)>
        callback)
{
    joystickHatCallbacks.push_back(callback);
}
