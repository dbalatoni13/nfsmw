#ifndef _IRecordablePlayer
#define _IRecordablePlayer

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"

struct IRecordablePlayer : public UTL::COM::IUnknown,
                           public UTL::Collections::Listable<IRecordablePlayer, 8> {
    static HINTERFACE _IHandle() { return (HINTERFACE)_IHandle; }

    IRecordablePlayer(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IRecordablePlayer() {}

    virtual void StartRecording() = 0;
    virtual void StopRecording() = 0;
    virtual void StartPlayback() = 0;
    virtual void Pause() = 0;
    virtual void UnPause() = 0;
    virtual float GetTimeTotalLength() = 0;
    virtual float GetTimeElapsed() = 0;
    virtual void SetTime() = 0;
    virtual void StopPlayback() = 0;
    virtual unsigned int GetPacketSize() = 0;
};

#endif
