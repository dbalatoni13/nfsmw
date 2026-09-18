#ifndef LUA_LUARUNTIME_H
#define LUA_LUARUNTIME_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

extern "C" {
#include "Speed/Indep/Src/Lua/source/lmem.h"
}

DECLARE_CONTAINER_TYPE(LuaLargeFreeBlock);
DECLARE_CONTAINER_TYPE(LuaEmergencyAlloc);

// total size: 0x7C
class LuaRuntime {
  public:
    // total size: 0x1
    struct TableVar {
        TableVar() {
            *(unsigned char *)this = 0;
        }

        char mVarType : 4;            // offset 0x0, size 0x1
        unsigned char mBoolValue : 1; // offset 0x0, size 0x1
        unsigned char mPacked : 1;    // offset 0x0, size 0x1
        unsigned char mIsKey : 1;     // offset 0x0, size 0x1
        unsigned char mValid : 1;     // offset 0x0, size 0x1
    };

  public:
    typedef UTL::Std::list<void *, _type_LuaEmergencyAlloc> EmergencyAllocList;
    typedef UTL::Std::map<unsigned int, void *, _type_LuaLargeFreeBlock> LargeFreeBlockMap;

  public:
    LuaRuntime(unsigned int memPoolSize);
    ~LuaRuntime();

    static void Init(std::size_t memPoolSize);
    static void Shutdown();

    void CreateState();

    void *Alloc(unsigned int size);
    void Free(void *block, unsigned int size);

    void TakeResetSnapshot();
    void FreeResetSnapshot();
    void BeginDelivery();
    void EndDelivery();

    void *Realloc(lua_State *state, void *oldblock, unsigned int oldsize, unsigned int size, LuaAllocType type);
    void ResetHeap();
    void FreeEmergencyAllocations();

    static int PackIdentifier(const char *src, unsigned char *dest);
    static int UnpackIdentifier(const unsigned char *src, char *dest);

    static unsigned int SerializeTable(lua_State *state, unsigned char *buffer, bool allowUserData);
    static unsigned int DeserializeTable(lua_State *luaState, unsigned char *src, bool purgeOnLoad);

    static void DumpStack(lua_State *luaState);
    static int HandleGlobalIndex(lua_State *luaState);

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
    LargeFreeBlockMap mLargeFreeBlocks;                                            // offset 0x50, size 0x10
    EmergencyAllocList mEmergencyBlocks;                                           // offset 0x60, size 0x8
    unsigned char *mHeapSnapshot;                                                  // offset 0x68, size 0x4
    unsigned int mHeapSnapshotSize;                                                // offset 0x6C, size 0x4
    bool mHeapSnapshotCompressed;                                                  // offset 0x70, size 0x1
    bool mNeedToResetHeap;                                                         // offset 0x74, size 0x1
    int mMessagesBeingDelivered;                                                   // offset 0x78, size 0x4
};

#endif
