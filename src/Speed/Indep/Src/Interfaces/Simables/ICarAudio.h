#ifndef ICARAUDIO_H
#define ICARAUDIO_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

// total size: 0x8
class ICarAudio : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ICarAudio);

    virtual Rpm GetRPM() const = 0;
};

#endif
