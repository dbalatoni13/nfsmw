#include "Speed/Indep/Src/FEng/FEScript.h"
#include "Speed/Indep/Src/FEng/FESlotPool.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/FEng/ObjectPool.h"

ObjectPool<FEScript, 32> FEScript::NodePool;

void* FEScript::operator new(unsigned int) {
    FEScript* pNode = NodePool.AllocSingle();
    pNode->Init();
    return pNode;
}

void FEScript::operator delete(void* pNode) {
    NodePool.FreeSingle(static_cast<FEScript*>(pNode));
}

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

FEScript::~FEScript() {
    delete[] pTracks;
    pTracks = nullptr;
    delete[] pName;
    pName = nullptr;
    delete[] Events.pEvent;
    Events.pEvent = nullptr;
}

void FEScript::SetTrackCount(long Count) {
    delete[] pTracks;
    TrackCount = Count;
    pTracks = nullptr;
    if (Count != 0) {
        pTracks = new FEKeyTrack[Count];
    }
}

FEScript::FEScript(FEScript& Src, bool bReference) {
    Init();
    Length = Src.Length;
    CurTime = 0;
    Flags = Src.Flags;
    pChainTo = Src.pChainTo;
    SetTrackCount(Src.TrackCount);
    if (bReference) {
        unsigned long i = 0;
        if (TrackCount != 0) {
            do {
                pTracks[i] = Src.pTracks[i];
                i++;
            } while (i < TrackCount);
        }
    } else {
        unsigned long i = 0;
        if (TrackCount != 0) {
            do {
                FEKeyTrack* pDst = &pTracks[i];
                FEKeyTrack* pSrc = &Src.pTracks[i];
                pDst->ParamType = pSrc->ParamType;
                pDst->ParamSize = pSrc->ParamSize;
                pDst->InterpType = pSrc->InterpType;
                pDst->InterpAction = pSrc->InterpAction;
                *reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(pDst) + 4) =
                    (*reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(pSrc) + 4) & 0xFFFFFF00) |
                    (*reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(pDst) + 4) & 0xFF);
                *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(pDst) + 7) =
                    *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(pSrc) + 7);
                pDst->BaseKey = pSrc->BaseKey;
                pDst->DeltaKeys.ReferenceList(&pSrc->DeltaKeys);
                i++;
            } while (i < TrackCount);
        }
    }
    Events = Src.Events;
}

// Pool removed - using ObjectPool template
