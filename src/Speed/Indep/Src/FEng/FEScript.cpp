#include "Speed/Indep/Src/FEng/FEScript.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

extern const unsigned long FETrackOffsets[11] = {
    0x00000000,
    0x00000004,
    0x00000007,
    0x0000000A,
    0x0000000E,
    0x00000011,
    0x00000013,
    0x00000015,
    0x00000019,
    0x0000001D,
    0x00000021,
};

void FEScript::Init() {
    *reinterpret_cast<char**>(reinterpret_cast<char*>(this) + 0x2C) = nullptr;
    *reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(this) + 0x30) = 0;
    *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x0C) = 0;
    *reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(this) + 0x14) = 0;
    *reinterpret_cast<FEScript**>(reinterpret_cast<char*>(this) + 0x18) = nullptr;
    *reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(this) + 0x1C) = 0;
    *reinterpret_cast<FEKeyTrack**>(reinterpret_cast<char*>(this) + 0x20) = nullptr;
}

FEKeyTrack* FEScript::FindTrack(FEKeyTrack_Indices TrackIndex) const {
    unsigned long i = 0;

    if (TrackCount != 0) {
        do {
            FEKeyTrack* pTrack = pTracks + i;

            if (*(reinterpret_cast<unsigned char*>(pTrack) + 7) == FETrackOffsets[TrackIndex]) {
                return pTrack;
            }

            i++;
        } while (i < TrackCount);
    }

    return nullptr;
}

void FEScript::SetName(const char* pNewName) {
    if (pName) {
        delete[] pName;
        pName = nullptr;
    }

    ID = 0xFFFFFFFF;
    if (pNewName) {
        pName = static_cast<char*>(FEngMalloc(FEngStrLen(pNewName) + 1, nullptr, 0));
        FEngStrCpy(pName, pNewName);
        ID = FEHashUpper(pName);
    }
}
