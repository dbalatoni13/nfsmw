#ifndef LUA_LUARUNTIME_H
#define LUA_LUARUNTIME_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

struct _type_LuaLargeFreeBlock {
    const char *name() {
        return "LuaLargeFreeBlock";
    };
};

struct _type_LuaEmergencyAlloc {
    const char *name() {
        return "LuaEmergencyAlloc";
    };
};

// total size: 0x7C
class LuaRuntime {
  public:
    // total size: 0x1
    struct TableVar {
        char mVarType : 4;            // offset 0x0, size 0x1
        unsigned char mBoolValue : 1; // offset 0x0, size 0x1
        unsigned char mPacked : 1;    // offset 0x0, size 0x1
        unsigned char mIsKey : 1;     // offset 0x0, size 0x1
        unsigned char mValid : 1;     // offset 0x0, size 0x1
    };

  public:
    static void Init(std::size_t memPoolSize);
    static void Shutdown();

    void TakeResetSnapshot();

    static LuaRuntime &Get() {
        return *mObj;
    }

    lua_State *GetState() {
        return mState;
    }

  private:
    static LuaRuntime *mObj;

    lua_State *mState;                                                             // offset 0x0, size 0x4
    unsigned int mMemPoolSize;                                                     // offset 0x4, size 0x4
    unsigned int mMemPoolUsed;                                                     // offset 0x8, size 0x4
    unsigned char *mMemPoolBlock;                                                  // offset 0xC, size 0x4
    void *mSmallFreeBlockChain[16];                                                // offset 0x10, size 0x40
    UTL::Std::map<unsigned int, void *, _type_LuaLargeFreeBlock> mLargeFreeBlocks; // offset 0x50, size 0x10
    UTL::Std::list<void *, _type_LuaEmergencyAlloc> mEmergencyBlocks;              // offset 0x60, size 0x8
    unsigned char *mHeapSnapshot;                                                  // offset 0x68, size 0x4
    unsigned int mHeapSnapshotSize;                                                // offset 0x6C, size 0x4
    bool mHeapSnapshotCompressed;                                                  // offset 0x70, size 0x1
    bool mNeedToResetHeap;                                                         // offset 0x74, size 0x1
    int mMessagesBeingDelivered;                                                   // offset 0x78, size 0x4
};

#endif
