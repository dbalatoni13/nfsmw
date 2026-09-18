#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Animation/AnimDirectory.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEAnimScene.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

namespace ICE {

unsigned int GetSceneCount();
unsigned int GetSceneHash(unsigned int scene_slot_position);
void GetNameOfSceneHash(unsigned int scene_hash, char *buffer);

}; // namespace ICE

unsigned int ICE::GetSceneCount() {
    unsigned int sceneCount = 0;
    if (TheAnimDirectory != nullptr) {

        sceneCount = TheAnimDirectory->GetSceneCount();
    }

    return sceneCount;
}

unsigned int ICE::GetSceneHash(unsigned int scene_slot_position) {
    if (TheAnimDirectory != nullptr) {

        if (scene_slot_position < TheAnimDirectory->GetSceneCount()) {

            AnimSceneLoadInfo anim_scene_load_info;

            TheAnimDirectory->GetSceneLoadInfo(scene_slot_position, anim_scene_load_info);

            return anim_scene_load_info.mAnimSceneHash;
        }
    }

    return 0;
}

void ICE::GetNameOfSceneHash(unsigned int scene_hash, char *buffer) {
    buffer[0] = '\0';

    if (TheAnimDirectory != nullptr) {

        TheAnimDirectory->GetNameOfSceneHash(scene_hash, buffer);
    }
}

void ICE::FireEventTag(int n) {
    INIS *nis = INIS::Get();

    if (nis != nullptr) {

        char tagName[64];

        bSPrintf(tagName, "ICECut%d", n);
        nis->FireEventTag(tagName);
    }
}

ICEScene *ICE::FindAnimScene() {
    INIS *nis = INIS::Get();

    return nis ? nis->GetScene() : 0;
}
