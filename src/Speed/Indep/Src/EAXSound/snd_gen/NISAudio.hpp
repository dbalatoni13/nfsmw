#ifndef EAXSOUND_SND_GEN_NISAUDIO_H
#define EAXSOUND_SND_GEN_NISAUDIO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/aud_moment_strm.h"

namespace Csis {
extern FunctionHandle gSoundFX_SelectHandle;
extern InterfaceId SoundFX_SelectId;

struct SoundFX_SelectStruct {
    Type_SoundFX_Type SoundFX_Type;   // offset 0x0, size 0x4
    Type_SoundFX_Param SoundFX_Param; // offset 0x4, size 0x4
};

inline System::Result SoundFX_Select(Type_SoundFX_Type SoundFX_Type, Type_SoundFX_Param SoundFX_Param) {
    System::Result result;
    SoundFX_SelectStruct data;

    data.SoundFX_Type = SoundFX_Type;
    data.SoundFX_Param = SoundFX_Param;
    result = Function::Call(&gSoundFX_SelectHandle, &data);
    if (result < System::kResult_Ok) {
        gSoundFX_SelectHandle.Set(&SoundFX_SelectId);
        Function::Call(&gSoundFX_SelectHandle, &data);
    }
    return result;
}
} // namespace Csis



#endif
