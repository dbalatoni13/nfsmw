#ifndef GENERATED_MESSAGES_MNOTIFYMILESTONEPROGRESS_H
#define GENERATED_MESSAGES_MNOTIFYMILESTONEPROGRESS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x18
class MNotifyMilestoneProgress : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyMilestoneProgress);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyMilestoneProgress");

        return k;
    }

    MNotifyMilestoneProgress(const char *_MilestoneName, float _ValueReached)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fMilestoneName(_MilestoneName), fValueReached(_ValueReached) {}

    ~MNotifyMilestoneProgress() {}

    const char *GetMilestoneName() const {
        return fMilestoneName;
    }

    void SetMilestoneName(const char *_MilestoneName) {
        fMilestoneName = _MilestoneName;
    }

    float GetValueReached() const {
        return fValueReached;
    }

    void SetValueReached(float _ValueReached) {
        fValueReached = _ValueReached;
    }

  private:
    const char *fMilestoneName; // offset 0x10, size 0x4
    float fValueReached;        // offset 0x14, size 0x4
};

#endif
