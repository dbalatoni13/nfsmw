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
    bVector3 position;
    bVector3 normal;
    bVector3 velocity;
    float magnitude;
    Attrib::RefSpec attributes;
    unsigned int object;
    unsigned int other_object;
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
