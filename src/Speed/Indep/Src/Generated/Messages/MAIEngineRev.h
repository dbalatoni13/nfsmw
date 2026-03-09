#ifndef GENERATED_MESSAGES_MAIENGINEREV_H
#define GENERATED_MESSAGES_MAIENGINEREV_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x20
class MAIEngineRev : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MAIEngineRev);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MAIEngineRev");

        return k;
    }

    MAIEngineRev(unsigned int _hSimable, unsigned int _CarID, EAX_CarState *_CarState, unsigned int _PatterToPlay)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fhSimable(_hSimable), fCarID(_CarID), fCarState(_CarState), fPatterToPlay(_PatterToPlay) {}

    ~MAIEngineRev() {}

    unsigned int GethSimable() const {
        return fhSimable;
    }

    void SethSimable(unsigned int _hSimable) {
        fhSimable = _hSimable;
    }

    unsigned int GetCarID() const {
        return fCarID;
    }

    void SetCarID(unsigned int _CarID) {
        fCarID = _CarID;
    }

    EAX_CarState *GetCarState() const {
        return fCarState;
    }

    void SetCarState(EAX_CarState *_CarState) {
        fCarState = _CarState;
    }

    unsigned int GetPatterToPlay() const {
        return fPatterToPlay;
    }

    void SetPatterToPlay(unsigned int _PatterToPlay) {
        fPatterToPlay = _PatterToPlay;
    }

  private:
    unsigned int fhSimable;     // offset 0x10, size 0x4
    unsigned int fCarID;        // offset 0x14, size 0x4
    EAX_CarState *fCarState;    // offset 0x18, size 0x4
    unsigned int fPatterToPlay; // offset 0x1c, size 0x4
};

#endif
