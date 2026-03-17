#include "Speed/Indep/Src/Gameplay/GHandler.h"

#include "Speed/Indep/Src/Gameplay/LuaMessageDeliveryInfo.h"
#include "Speed/Indep/Src/Lua/source/lauxlib.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Lua/LuaRuntime.h"
#include "Speed/Indep/Src/Misc/LZCompress.hpp"

void LZByteSwapHeader(LZHeader *header);

GHandler::GHandler(const Attrib::Key &handlerKey)
    : GRuntimeInstance(handlerKey, kGameplayObjType_Handler), //
      mAttached(false) {}

GHandler::~GHandler() {
    Detach(LuaRuntime::Get().GetState());
}

void GHandler::NotifyBytecodeFlushed() {
    mAttached = false;
}

void GHandler::Attach(lua_State *luaState) {
    if (!mAttached) {
        const unsigned int *bytecode = reinterpret_cast<const unsigned int *>(GetAttributePointer(0x9a4a020a, 0));

        if (bytecode) {
            unsigned int size = bytecode[0];
            LZHeader *header = reinterpret_cast<LZHeader *>(bytecode[1]);

            if (size != 0) {
                int result;

                LZByteSwapHeader(header);
                if (!LZValidHeader(header)) {
                    result = lua_dobuffer(luaState, reinterpret_cast<const char *>(header), size, "Handler");
                } else {
                    unsigned char stackBuffer[0x1000];
                    unsigned char *buffer = stackBuffer;
                    unsigned char *allocated = nullptr;

                    if (header->UncompressedSize > sizeof(stackBuffer)) {
                        allocated = new unsigned char[header->UncompressedSize];
                        buffer = allocated;
                    }

                    LZDecompress(reinterpret_cast<uint8 *>(header), buffer);
                    result = lua_dobuffer(luaState, reinterpret_cast<const char *>(buffer), header->UncompressedSize, "Handler");
                    delete[] allocated;
                }
                LZByteSwapHeader(header);

                if (result == 0) {
                    mAttached = true;
                }
            }
        }
    }
}

void GHandler::Detach(lua_State *luaState) {
    if (mAttached) {
        lua_pushstring(luaState, CollectionName());
        lua_pushnil(luaState);
        lua_settable(luaState, LUA_GLOBALSINDEX);
        mAttached = false;
    }
}

void GHandler::HandleMessage(LuaMessageDeliveryInfo *deliveryInfo) {
    ExecuteScriptedHandler(deliveryInfo);
}

void GHandler::ExecuteScriptedHandler(LuaMessageDeliveryInfo *deliveryInfo) {
    lua_State *luaState = deliveryInfo->GetLuaState();

    Attach(luaState);
    lua_pushstring(luaState, CollectionName());
    lua_gettable(luaState, LUA_GLOBALSINDEX);
    lua_pushvalue(luaState, -3);
    lua_pushvalue(luaState, -5);
    lua_pushvalue(luaState, -4);
    lua_call(luaState, 3, 0);
}
