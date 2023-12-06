#include "OptionsModel.h"

// Initialize static data
Rml::DataModelHandle OptionsModel::handle;
int OptionsModel::radarStyle = RadarFast;

bool OptionsModel::setupDataBinding(Rml::Context *context, Rml::DataModelHandle &model) {
    Rml::DataModelConstructor constructor = context->CreateDataModel("options");
    if (!constructor)
        return false;

    constructor.Bind("radarstyle", &radarStyle);

    handle = model = constructor.GetModelHandle();

    return true;
}

void OptionsModel::update(Rml::DataModelHandle) {

}

void OptionsModel::dirtyVariable(Rml::String variable) {
    if (handle)
        handle.DirtyVariable(variable);
}

void OptionsModel::setRadarStyle(RadarStyle mode) {
    radarStyle = mode;
}
