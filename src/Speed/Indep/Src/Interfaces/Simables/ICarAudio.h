#ifndef INTERFACES_SIMABLES_ICARAUDIO_H
#define INTERFACES_SIMABLES_ICARAUDIO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// total size: 0x8
struct ICarAudio : public UTL::COM::IUnknown {
    static HINTERFACE _IHandle();

    ICarAudio(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, (HINTERFACE)_IHandle) {}

    virtual ~ICarAudio() {}

    virtual Rpm GetRPM() const = 0;
};

#endif
