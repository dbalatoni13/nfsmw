#include "types.h"
#include "Speed/Indep/Src/FEng/FEScript.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/FEng/ObjectPool.h"

// size: 0x2C, address: 0x803EAC10, Decl: speed/indep/src/feng/FEScript.cpp:11
const u32 FETrackOffsets[11] = {
    0x00000000, 0x00000004, 0x00000007, 0x0000000A, 0x0000000E, 0x00000011, 0x00000013, 0x00000015, 0x00000019, 0x0000001D, 0x00000021,
};

// size: 0x10, address: 0x80473DFC, Decl: speed/indep/src/feng/FEScript.cpp:27
ObjectPool<FEScript, 32> FEScript::NodePool = ObjectPool<FEScript, 32>();

void FEScript::Init() {
    pName = nullptr;
    ID = 0;
    Length = 0;
    Flags = 0;
    pChainTo = nullptr;
    TrackCount = 0;
    pTracks = nullptr;
}

FEScript::~FEScript() {
    delete[] pTracks;
    pTracks = nullptr;
    delete[] pName;
    pName = nullptr;
}

FEScript::FEScript(FEScript &Src, bool bReference) {
    Init();
    SetName(Src.GetName());
    ID = Src.ID;
    Length = Src.Length;
    CurTime = Src.CurTime;
    Flags = Src.Flags;
    SetTrackCount(Src.TrackCount);
    if (bReference) {
        for (u32 i = 0; i < TrackCount; i++) {
            pTracks[i].ParamType = Src.pTracks[i].ParamType;
            pTracks[i].ParamSize = Src.pTracks[i].ParamSize;
            pTracks[i].InterpType = Src.pTracks[i].InterpType;
            pTracks[i].InterpAction = Src.pTracks[i].InterpAction;
            pTracks[i].Length = Src.pTracks[i].Length;
            pTracks[i].LongOffset = Src.pTracks[i].LongOffset;
            pTracks[i].BaseKey = Src.pTracks[i].BaseKey;
            pTracks[i].DeltaKeys.ReferenceList(&Src.pTracks[i].DeltaKeys);
        }
    } else {
        for (u32 i = 0; i < TrackCount; i++) {
            pTracks[i] = Src.pTracks[i];
        }
    }
    Events = Src.Events;
}

void *FEScript::operator new(size_t) {
    FEScript *pNode = NodePool.AllocSingle();
    pNode->Init();
    return pNode;
}

void FEScript::operator delete(void *pNode) {
    FEScript *pDeleteNode = static_cast<FEScript *>(pNode);
    pDeleteNode->~FEScript();
    NodePool.FreeSingle(pDeleteNode);
}

void FEScript::SetName(const char *pNewName) {
    if (pName) {
        delete[] pName;
        pName = nullptr;
    }

    ID = 0xFFFFFFFF;
    if (pNewName) {
        int Len = FEngStrLen(pNewName) + 1;
        pName = FNEW char[Len];
        FEngStrCpy(pName, pNewName);
        ID = FEHashUpper(pName);
    }
}

void FEScript::SetTrackCount(i32 Count) {
    delete[] pTracks;
    pTracks = nullptr;
    TrackCount = Count;
    if (Count != 0) {
        pTracks = FNEW FEKeyTrack[Count];
    }
}

FEKeyTrack *FEScript::FindTrack(FEKeyTrack_Indices TrackIndex) const {

    u32 Offset = FETrackOffsets[TrackIndex];

    for (u32 Index = 0; Index < TrackCount; Index++) {
        if (pTracks[Index].LongOffset == Offset) {
            return &pTracks[Index];
        }
    }

    return nullptr;
}
