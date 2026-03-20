#include "Speed/Indep/Src/Gameplay/GState.h"

#include "Speed/Indep/Src/Generated/Messages/MStateEnter.h"
#include "Speed/Indep/Src/Generated/Messages/MStateExit.h"

UCrc32 MStateEnter::_GetKind() {
    static UCrc32 k("MStateEnter");

    return k;
}

UCrc32 MStateExit::_GetKind() {
    static UCrc32 k("MStateExit");

    return k;
}

GState::GState(const Attrib::Key &stateKey) : GRuntimeInstance(stateKey, kGameplayObjType_State) {}

GState::~GState() {}
