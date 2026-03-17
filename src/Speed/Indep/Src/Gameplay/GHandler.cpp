#include "Speed/Indep/Src/Gameplay/GHandler.h"

#include "Speed/Indep/Src/Lua/LuaRuntime.h"

GHandler::GHandler(const Attrib::Key &handlerKey)
    : GRuntimeInstance(handlerKey, kGameplayObjType_Handler), //
      mAttached(false) {}

GHandler::~GHandler() {
    Detach(LuaRuntime::Get().GetState());
}

void GHandler::NotifyBytecodeFlushed() {
    mAttached = false;
}
