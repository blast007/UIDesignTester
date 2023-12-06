#include "UI_SystemInterface.h"
#include "PlatformFactory.h"

double UI_SystemInterface::GetElapsedTime()
{
    return PlatformFactory::get()->getGameTime();
}
