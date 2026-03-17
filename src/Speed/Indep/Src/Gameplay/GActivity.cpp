#include "GActivity.h"

#include "GManager.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void GActivity::SerializeVars(bool abandonLuaTable) {
    SerializedHeader header;
    ObjectStateBlockHeader *block;
    const bool *persistent;
    const char *stateName;

    if (!mVarsInLuaVM) {
        return;
    }

    stateName = nullptr;
    if (mCurrentState) {
        stateName = mCurrentState->CollectionName();
    }

    header.mStateNameHash = stateName && *stateName ? Attrib::StringHash32(stateName) : 0;
    header.mFlags = mRunning ? 1 : 0;
    header.mTableBytes = 0;

    persistent = reinterpret_cast<const bool *>(GetAttributePointer(0xE4542E9B, 0) ?
                                                    GetAttributePointer(0xE4542E9B, 0) :
                                                    Attrib::DefaultDataArea(sizeof(bool)));
    block = GManager::Get().AllocObjectStateBlock(GetCollection(), sizeof(header), *persistent);
    if (block) {
        bMemCpy(block, &header, sizeof(header));
    }

    if (abandonLuaTable) {
        mVarsInLuaVM = false;
    }
}
