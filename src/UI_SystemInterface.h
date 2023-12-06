#include <RmlUi/Core.h>

class UI_SystemInterface : public Rml::SystemInterface
{
    public:
        virtual double GetElapsedTime();
};
