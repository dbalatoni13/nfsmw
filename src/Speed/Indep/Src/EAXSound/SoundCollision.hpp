#ifndef EAXSOUND_SOUNDCOLLISION_H
#define EAXSOUND_SOUNDCOLLISION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace Sound {

struct AudioEventParams {
    bVector3 position;            // offset 0x0, size 0x10
    bVector3 normal;              // offset 0x10, size 0x10
    bVector3 velocity;            // offset 0x20, size 0x10
    float magnitude;              // offset 0x30, size 0x4
    Attrib::RefSpec attributes;   // offset 0x34, size 0xC
    unsigned int object;          // offset 0x40, size 0x4
    unsigned int other_object;    // offset 0x44, size 0x4
};

struct CollisionDescription {
    int _pad;
};

struct AudioEvent : public UTL::COM::Factory<const AudioEventParams &, AudioEvent, unsigned int> {
    AudioEventParams mParams;
    Attrib::Instance mAttributes;

    AudioEvent(const AudioEventParams &params)
        : mParams(params) //
        , mAttributes(params.attributes, 0, nullptr)
    {}

    const AudioEventParams &GetParameters() const {
        return mParams;
    }

    const Attrib::Instance &GetAttributes() const {
        return mAttributes;
    }

    virtual ~AudioEvent() {}
    virtual void Release() {}
    virtual void Pause(bool) {}
    virtual void Update(const bVector3 &p, const bVector3 &n, const bVector3 &v, float mag);
};

} // namespace Sound



#endif
