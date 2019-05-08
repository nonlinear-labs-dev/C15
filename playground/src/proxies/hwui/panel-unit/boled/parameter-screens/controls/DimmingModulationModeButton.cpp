#include "DimmingModulationModeButton.h"
#include "parameters/ModulateableParameter.h"
#include <proxies/hwui/buttons.h>

DimmingModulationModeButton::DimmingModulationModeButton(const Glib::ustring &text, Buttons id)
    : super(text, id)
{
}

DimmingModulationModeButton::~DimmingModulationModeButton()
{
}
