#ifndef CAMERA_ICE_ICEREPLAY_H
#define CAMERA_ICE_ICEREPLAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"

namespace ICEReplay {
static int nRecentlyUsedIndex;
static int RecentlyUsedTracks[3];

bool WasRecentlyUsed(class ICETrack *pTrack);
void ClearRecentlyUsed(void);
} // namespace ICEReplay

struct ICEAnchor;

namespace ICEReplay {

class ICETrack *ChooseGoodCamera(ICEAnchor *pAnchor, class ICEGroup *pGroup, int n);
bool CameraCutIsGood(struct ICEData *pKey, float fParameter, ICEAnchor *pAnchor);

} // namespace ICEReplay

namespace ICE {

// total size: 0x18
struct ReplayCategory {
    char *pName;               // offset 0x0, size 0x4
    unsigned int nNameHash;    // offset 0x4, size 0x4
    char *pReplayName;         // offset 0x8, size 0x4
    unsigned int nReplayHash;  // offset 0xC, size 0x4
    float (*pScore)(ICEAnchor *);  // offset 0x10, size 0x4
    bool (*pMirror)(ICEAnchor *);  // offset 0x14, size 0x4

    // Cuerpo de asignaciones, no lista de inicializacion: el objetivo guarda
    // los cuatro constantes ANTES de las dos llamadas y calcula el hash de
    // pReplayName ANTES que el de pName. Una lista de inicializacion los
    // reordena a orden de declaracion y una lista de llaves mete memset+temporal.
    ReplayCategory(char *name, char *replay, float (*score)(ICEAnchor *), bool (*mirror)(ICEAnchor *))
        : pName(name), pReplayName(replay), pScore(score), pMirror(mirror) {
        nReplayHash = bStringHash(replay);
        nNameHash = bStringHash(name);
    }

    float GetScore(ICEAnchor *pAnchor) { return pScore ? pScore(pAnchor) : 0.0f; }

    bool GetMirror(ICEAnchor *pAnchor) { return pMirror ? pMirror(pAnchor) : false; }
};

int GetReplayCategoryNumElements();
unsigned int GetReplayCategoryHash(int n);
ReplayCategory *GetReplayCategory(unsigned int hash);

} // namespace ICE
#endif
