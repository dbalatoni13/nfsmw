#include "ControllerUnplugged.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"

ControllerUnplugged::ControllerUnplugged(ScreenConstructorData* sd)
    : MenuScreen(sd) //
    , port(PORT_INVALID)
{
    Setup();
}

ControllerUnplugged::~ControllerUnplugged() {}

void ControllerUnplugged::Setup() {
    const char* pkg_name = GetPackageName();
    FEObject* pObj = FEngFindObject(pkg_name, 0xC7AB3F6D);
    if (pObj != nullptr) {
        FEngSetInvisible(pObj);
    }
}
