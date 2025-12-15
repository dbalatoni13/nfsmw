#ifndef LUA_LUAATTRIBUTES_H
#define LUA_LUAATTRIBUTES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Gameplay/GRuntimeInstance.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// total size: 0x14
struct LuaAttribAccessInfo {
    lua_State *mLuaState;               // offset 0x0, size 0x4
    GRuntimeInstance *mRuntimeInstance; // offset 0x4, size 0x4
    Attrib::Attribute *mAttribute;      // offset 0x8, size 0x4
    int mAttribIndex;                   // offset 0xC, size 0x4
    int mSrcStackIndex;                 // offset 0x10, size 0x4
};

// total size: 0x8
struct LuaAttribAccessors {
    void (*mPushAttrib)(const LuaAttribAccessInfo &); // offset 0x0, size 0x4
    void (*mSetAttrib)(const LuaAttribAccessInfo &);  // offset 0x4, size 0x4
};

// total size: 0x14
class LuaAttributes {
  public:
    static void Init();
    static void Shutdown();

    void BindAccessors(lua_State *luaState);

    static LuaAttributes &Get() {
        return *fObj;
    }

  private:
    static LuaAttributes *fObj;

    Attrib::Class *fGameplayClass;                                             // offset 0x0, size 0x4
    UTL::Std::map<unsigned int, LuaAttribAccessors, _type_map> fTypeAccessors; // offset 0x4, size 0x10
};

#endif
