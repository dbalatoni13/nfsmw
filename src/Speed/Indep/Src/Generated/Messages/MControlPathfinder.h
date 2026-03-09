#ifndef GENERATED_MESSAGES_MCONTROLPATHFINDER_H
#define GENERATED_MESSAGES_MCONTROLPATHFINDER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x20
class MControlPathfinder : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MControlPathfinder);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MControlPathfinder");

        return k;
    }

    MControlPathfinder(bool _Licenced, unsigned int _PathEvent, unsigned int _PathControl, unsigned int _PartID)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fLicenced(_Licenced), fPathEvent(_PathEvent), fPathControl(_PathControl), fPartID(_PartID) {}

    ~MControlPathfinder() {}

    bool GetLicenced() const {
        return fLicenced;
    }

    void SetLicenced(bool _Licenced) {
        fLicenced = _Licenced;
    }

    unsigned int GetPathEvent() const {
        return fPathEvent;
    }

    void SetPathEvent(unsigned int _PathEvent) {
        fPathEvent = _PathEvent;
    }

    unsigned int GetPathControl() const {
        return fPathControl;
    }

    void SetPathControl(unsigned int _PathControl) {
        fPathControl = _PathControl;
    }

    unsigned int GetPartID() const {
        return fPartID;
    }

    void SetPartID(unsigned int _PartID) {
        fPartID = _PartID;
    }

  private:
    bool fLicenced;            // offset 0x10, size 0x1
    unsigned int fPathEvent;   // offset 0x14, size 0x4
    unsigned int fPathControl; // offset 0x18, size 0x4
    unsigned int fPartID;      // offset 0x1c, size 0x4
};

#endif
