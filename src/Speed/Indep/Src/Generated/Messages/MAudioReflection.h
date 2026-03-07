#ifndef GENERATED_MESSAGES_MAUDIOREFLECTION_H
#define GENERATED_MESSAGES_MAUDIOREFLECTION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x1c
class MAudioReflection : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MAudioReflection);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MAudioReflection");

        return k;
    }

    MAudioReflection(int _PlayerNum, float _Dist, bool _Covered)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fPlayerNum(_PlayerNum), fDist(_Dist), fCovered(_Covered) {}

    ~MAudioReflection() {}

    int GetPlayerNum() const {
        return fPlayerNum;
    }

    void SetPlayerNum(int _PlayerNum) {
        fPlayerNum = _PlayerNum;
    }

    float GetDist() const {
        return fDist;
    }

    void SetDist(float _Dist) {
        fDist = _Dist;
    }

    bool GetCovered() const {
        return fCovered;
    }

    void SetCovered(bool _Covered) {
        fCovered = _Covered;
    }

  private:
    int fPlayerNum; // offset 0x10, size 0x4
    float fDist;    // offset 0x14, size 0x4
    bool fCovered;  // offset 0x18, size 0x1
};

#endif
