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
    FEScript* pDeleteNode = static_cast<FEScript*>(pNode);
    pDeleteNode->~FEScript();
    NodePool.FreeSingleNoDestroy(pDeleteNode);
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
    unsigned long count = TrackCount;
    int offset = static_cast<int>(FETrackOffsets[TrackIndex]);

    if (i < count) {
        FEKeyTrack* pBase = pTracks;
        do {
            FEKeyTrack* pTrack = pBase + i;

            if (pTrack->LongOffset == offset) {
                return pTrack;
            }

            i++;
        } while (i < count);
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
        int Len = FEngStrLen(pNewName) + 1;

        pName = FENG_NEW char[Len];
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
        pTracks = FENG_NEW FEKeyTrack[Count];
    }
}

FEScript::FEScript(FEScript& Src, bool bReference) {
    Init();
    SetName(Src.pName);
    ID = Src.ID;
    Length = Src.Length;
    CurTime = Src.CurTime;
    Flags = Src.Flags;
    SetTrackCount(Src.TrackCount);
    if (bReference) {
        for (unsigned long i = 0; i < TrackCount; i++) {
            pTracks[i].ParamType = Src.pTracks[i].ParamType;
            pTracks[i].ParamSize = Src.pTracks[i].ParamSize;
            pTracks[i].InterpType = Src.pTracks[i].InterpType;
            pTracks[i].InterpAction = Src.pTracks[i].InterpAction;
            pTracks[i].Length = Src.pTracks[i].Length;
            *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(&pTracks[i]) + 7) =
                *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(&Src.pTracks[i]) + 7);
            pTracks[i].BaseKey = Src.pTracks[i].BaseKey;
            pTracks[i].DeltaKeys.ReferenceList(&Src.pTracks[i].DeltaKeys);
        }
    } else {
        for (unsigned long i = 0; i < TrackCount; i++) {
            pTracks[i] = Src.pTracks[i];
        }
    }
    Events = Src.Events;
}

// Pool removed - using ObjectPool template
