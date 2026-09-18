#ifndef LUA_LUABINDERY_H
#define LUA_LUABINDERY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Lua/source/lua.h"

// total size: 0x1
class LuaBindery {
  public:
    LuaBindery();
    ~LuaBindery();

    static void Init();
    static void Shutdown();

    void BindToGameCode(lua_State *luaState);

    static void LoadMetatable(lua_State *luaState, const char *metatableName);
    static void AttachMetatable(lua_State *luaState, const char *metatableName);
    static void GetGlobalTable(lua_State *luaState, const char *tableName);
    static void SetInGlobalTable(lua_State *luaState, const char *tableName);

    static LuaBindery &Get() {
        return *fObj;
    }

  private:
    static LuaBindery *fObj;
};

#endif
