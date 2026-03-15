#ifndef CAMERA_ICE_ICEREPLAY_H
#define CAMERA_ICE_ICEREPLAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct ICEAnchor;
struct ICETrack;

struct ICEReplayCategory {
    float GetScore(ICEAnchor *car) {
        if (ScoreFunction != 0) {
            return ScoreFunction(car);
        }
        return 0.0f;
    }

    bool GetMirror(ICEAnchor *car) {
        if (MirrorFunction != 0) {
            return MirrorFunction(car);
        }
        return false;
    }

    const char *pCategoryName;
    unsigned int nCategoryHash;
    const char *pSceneName;
    unsigned int nSceneHash;
    float (*ScoreFunction)(ICEAnchor *);
    bool (*MirrorFunction)(ICEAnchor *);
};

namespace ICE {

int GetReplayCategoryNumElements();
unsigned int GetReplayCategoryHash(int category);
ICEReplayCategory *GetReplayCategory(unsigned int category_hash);

} // namespace ICE

namespace ICEReplay {

extern ICETrack *RecentlyUsedTracks[3];
extern int nRecentlyUsedIndex;

bool WasRecentlyUsed(ICETrack *track);
void ClearRecentlyUsed();

} // namespace ICEReplay



#endif
