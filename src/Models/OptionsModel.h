#pragma once

#include "../global.h"

#include <RmlUi/Core.h>

class OptionsModel {
public:
    static bool setupDataBinding(Rml::Context* context, Rml::DataModelHandle& model);

    static void update(Rml::DataModelHandle model);

    // Options
    static void setRadarStyle(RadarStyle mode);

private:
    static Rml::DataModelHandle handle;

    static int radarStyle;

    static void dirtyVariable(Rml::String variable);
};
