#include "AnimScene.hpp"

CarAnimationState::CarAnimationState() {
    HaveLastCarPosition = 0;
    CarIndex = -1;
    AnimCtrl = nullptr;
}

bTList<CAnimSceneData> g_loadedAnimSceneDataList;
CarAnimationState gCarAnimationStates[16];
