#include "ControllerUnplugged.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"

const char* GetLocalizedString(unsigned int hash);
void FEPrintf(const char* pkg_name, unsigned int hash, const char* format, ...);

ControllerUnplugged::ControllerUnplugged(ScreenConstructorData* sd)
    : MenuScreen(sd) //
    , port(static_cast<JoystickPort>(sd->Arg))
{
    Setup();
}

ControllerUnplugged::~ControllerUnplugged() {}

void ControllerUnplugged::Setup() {
    const char* pkg_name = GetPackageName();
    const char* text = GetLocalizedString(0x54EEF4C5);
    FEPrintf(pkg_name, 0xB244CF71, text, port + 1);
}
