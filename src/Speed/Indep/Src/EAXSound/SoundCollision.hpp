#ifndef EAXSOUND_SOUNDCOLLISION_H
#define EAXSOUND_SOUNDCOLLISION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

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

struct AudioEvent {
    unsigned int mFactoryPad;
    AudioEventParams mParams;
    Attrib::Instance mAttributes;
    void *mVTableAudioEvent;

    AudioEvent(const AudioEventParams &params)
        : mFactoryPad(0) //
        , mParams(params) //
        , mAttributes(params.attributes, 0, nullptr) //
        , mVTableAudioEvent(nullptr)
    {}

    const AudioEventParams &GetParameters() const {
        return mParams;
    }

    const Attrib::Instance &GetAttributes() const {
        return mAttributes;
    }

    void Update(const bVector3 &p, const bVector3 &n, const bVector3 &v, float mag);
};

} // namespace Sound



#endif
