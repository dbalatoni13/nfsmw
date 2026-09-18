#include "Speed/Indep/Src/Lua/LuaRuntime.h"

#include <stdio.h>
#include <string.h>

#include "Speed/Indep/Src/Gameplay/GActivity.h"
#include "Speed/Indep/Src/Gameplay/GHandler.h"
#include "Speed/Indep/Src/Gameplay/GRuntimeInstance.h"
#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Misc/LZCompress.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern "C" {
#include "Speed/Indep/Src/Lua/source/lauxlib.h"
}

extern "C" void *LuaRealloc(lua_State *L, void *oldblock, unsigned int oldsize, unsigned int size, LuaAllocType type) {
    return LuaRuntime::Get().Realloc(L, oldblock, oldsize, size, type);
}

extern "C" void LuaFree(lua_State *L, void *oldblock, unsigned int oldsize, LuaAllocType type) {
    LuaRuntime::Get().Realloc(L, oldblock, oldsize, 0, type);
}

// MEDIDO r36b: `_GLOBAL_.I.<X>` toma el nombre del PRIMER simbolo global que
// emite la unidad. Con mObj arriba, GCC bautiza la funcion de inicializacion
// `_GLOBAL_.I._10LuaRuntime.mObj` y el objetivo la llama
// `_GLOBAL_.I.LuaRealloc`: mismo cuerpo byte a byte, distinto nombre, y por eso
// contaba como funcion ausente. Definiendo mObj DETRAS de LuaRealloc casa.
LuaRuntime *LuaRuntime::mObj = nullptr;

extern "C" int LuaPanic(lua_State *state) {
    LuaRuntime::DumpStack(state);
    return 0;
}

LuaRuntime::LuaRuntime(unsigned int memPoolSize) : mState(NULL), mMemPoolSize(0), mMemPoolUsed(0), mMemPoolBlock(NULL) {
    mHeapSnapshot = NULL;
    mHeapSnapshotSize = 0;
    mHeapSnapshotCompressed = false;
    mNeedToResetHeap = false;
    mMessagesBeingDelivered = 0;

    mMemPoolSize = memPoolSize;
    mMemPoolBlock = (unsigned char *) bMalloc(memPoolSize, "LuaRuntime memory pool", 0, 0);

    bMemSet(mMemPoolBlock, 0, mMemPoolSize);
    bMemSet(mSmallFreeBlockChain, 0, sizeof(mSmallFreeBlockChain));

    luaM_setallocator(LuaRealloc, LuaFree);
}

LuaRuntime::~LuaRuntime() {
    if (mState != NULL) {
        lua_close(mState);
    }

    if (mMemPoolBlock != NULL) {
        bFree(mMemPoolBlock);
    }

    mMemPoolBlock = NULL;
    mMemPoolSize = 0;

    FreeResetSnapshot();
    FreeEmergencyAllocations();
    mLargeFreeBlocks.clear();
}

void LuaRuntime::Init(std::size_t memPoolSize) {
    mObj = new ("LuaRuntime", 0) LuaRuntime(memPoolSize);
    mObj->CreateState();
}

void LuaRuntime::Shutdown() {
    delete mObj;
    mObj = NULL;
}

void LuaRuntime::CreateState() {
    mState = lua_open();

    lua_atpanic(mState, LuaPanic);

    lua_newtable(mState);
    lua_pushstring(mState, "__index");
    lua_pushcclosure(mState, HandleGlobalIndex, 0);
    lua_settable(mState, -3);
    lua_setmetatable(mState, LUA_GLOBALSINDEX);

    lua_setgcthreshold(mState, 1024 * 1024);
}

void *LuaRuntime::Alloc(unsigned int size) {
    unsigned int blockIdx = (size + 7) >> 3;

    if (blockIdx != 0) {
        if (blockIdx <= 15) {
            void *smallBlockPtr = mSmallFreeBlockChain[blockIdx];

            if (smallBlockPtr != NULL) {
                mSmallFreeBlockChain[blockIdx] = *(void **) smallBlockPtr;
                return smallBlockPtr;
            }
        } else {
            LargeFreeBlockMap::iterator iter = mLargeFreeBlocks.find(blockIdx);

            if (iter != mLargeFreeBlocks.end()) {
                void *largeBlockPtr = (*iter).second;
                void *nextLargeBlock = *(void **) largeBlockPtr;

                if (nextLargeBlock != NULL) {
                    mLargeFreeBlocks[blockIdx] = nextLargeBlock;
                } else {
                    mLargeFreeBlocks.erase(iter);
                }

                return largeBlockPtr;
            }
        }
    }

    if (mMemPoolUsed + size < mMemPoolSize) {
        unsigned char *block = mMemPoolBlock + mMemPoolUsed;

        mMemPoolUsed = (mMemPoolUsed + size + 7) & ~7;

        if (mMemPoolUsed + 12288 > mMemPoolSize) {
            mNeedToResetHeap = true;
        }

        return block;
    }

    mNeedToResetHeap = true;

    void *emergencyBlock = bMalloc(size, "Lua emergency allocation", 0, 64);

    mEmergencyBlocks.push_back(emergencyBlock);

    return emergencyBlock;
}

void LuaRuntime::Free(void *block, unsigned int size) {
    if (!mEmergencyBlocks.empty()) {
        for (EmergencyAllocList::iterator iter = mEmergencyBlocks.begin(); iter != mEmergencyBlocks.end(); ++iter) {
            if (*iter == block) {
                bFree(block);
                mEmergencyBlocks.erase(iter);
                return;
            }
        }
    }

    unsigned int blockIdx = (size + 7) >> 3;

    if (blockIdx != 0) {
        if (blockIdx <= 15) {
            *(void **) block = mSmallFreeBlockChain[blockIdx];
            mSmallFreeBlockChain[blockIdx] = block;
        } else {
            *(void **) block = mLargeFreeBlocks[blockIdx];
            mLargeFreeBlocks[blockIdx] = block;
        }
    }
}

void LuaRuntime::TakeResetSnapshot() {
    FreeResetSnapshot();

    unsigned int spaceInLocalHeap = mMemPoolSize - mMemPoolUsed;
    unsigned int spaceInMainHeap = bLargestMalloc(0);
    unsigned int bufferSpaceNeeded = LZGetMaxCompressedSize(mMemPoolUsed);
    unsigned char *mainHeapBuffer = NULL;
    unsigned char *compressionBuffer = NULL;

    if (bufferSpaceNeeded <= spaceInLocalHeap) {
        compressionBuffer = mMemPoolBlock + mMemPoolUsed;
    } else if (bufferSpaceNeeded <= spaceInMainHeap) {
        mainHeapBuffer = (unsigned char *) bMalloc(bufferSpaceNeeded, "LuaRuntime temp compression buffer", 0, 64);
        compressionBuffer = mainHeapBuffer;
    }

    unsigned char *snapshotSource = mMemPoolBlock;
    unsigned int snapshotAllocSize = mMemPoolUsed;

    if (compressionBuffer != NULL) {
        mHeapSnapshotCompressed = true;
        snapshotSource = compressionBuffer;
        snapshotAllocSize = LZCompress(mMemPoolBlock, mMemPoolUsed, compressionBuffer);
    } else {
        mHeapSnapshotCompressed = false;
    }

    mHeapSnapshotSize = mMemPoolUsed;
    mHeapSnapshot = (unsigned char *) bMalloc(snapshotAllocSize, "LuaRuntime VM reset snapshot", 0, 0);

    bMemCpy(mHeapSnapshot, snapshotSource, snapshotAllocSize);

    if (mainHeapBuffer != NULL) {
        bFree(mainHeapBuffer);
    }
}

void LuaRuntime::FreeResetSnapshot() {
    if (mHeapSnapshot != NULL) {
        bFree(mHeapSnapshot);
    }

    mHeapSnapshot = NULL;
    mHeapSnapshotSize = 0;
    mHeapSnapshotCompressed = false;
}

void LuaRuntime::BeginDelivery() {
    mMessagesBeingDelivered++;
}

void LuaRuntime::EndDelivery() {
    mMessagesBeingDelivered--;

    if (mNeedToResetHeap && mMessagesBeingDelivered <= 0) {
        ResetHeap();
    }
}

void *LuaRuntime::Realloc(lua_State *state, void *oldblock, unsigned int oldsize, unsigned int size, LuaAllocType type) {
    void *newblock = NULL;

    if (size != 0) {
        newblock = Alloc(size);

        if (oldblock != NULL) {
            bMemCpy(newblock, oldblock, size > oldsize ? oldsize : size);
        }
    }

    if (oldblock != NULL) {
        Free(oldblock, oldsize);
    }

    return newblock;
}

void LuaRuntime::ResetHeap() {
    if (mMessagesBeingDelivered > 0) {
        return;
    }

    for (GObjectIterator<GActivity> iterActivity(-1); iterActivity.IsValid(); iterActivity.Advance()) {
        iterActivity.GetInstance()->SerializeVars(true);
    }

    for (GObjectIterator<GHandler> iterHandler(-1); iterHandler.IsValid(); iterHandler.Advance()) {
        iterHandler.GetInstance()->NotifyBytecodeFlushed();
    }

    FreeEmergencyAllocations();

    bMemSet(mSmallFreeBlockChain, 0, sizeof(mSmallFreeBlockChain));
    mLargeFreeBlocks.clear();

    if (mHeapSnapshotCompressed) {
        LZDecompress(mHeapSnapshot, mMemPoolBlock);
    } else {
        bMemCpy(mMemPoolBlock, mHeapSnapshot, mHeapSnapshotSize);
    }

    mNeedToResetHeap = false;
    mMemPoolUsed = mHeapSnapshotSize;
}

void LuaRuntime::FreeEmergencyAllocations() {
    for (EmergencyAllocList::iterator iter = mEmergencyBlocks.begin(); iter != mEmergencyBlocks.end(); ++iter) {
        bFree(*iter);
    }

    mEmergencyBlocks.clear();
}

int LuaRuntime::PackIdentifier(const char *src, unsigned char *dest) {
    int bytes = 0;
    int acc = 0;
    int accBits = 0;
    char c;

    do {
        int code = 0;
        bool upperCase = false;

        c = *src++;

        if (c != 0) {
            if (c >= 'a' && c <= 'z') {
                code = c - 'a' + 1;
            } else if (c >= 'A' && c <= 'Z') {
                upperCase = true;
                code = c - 'A' + 1;
            } else if (c == '_') {
                code = 27;
            } else {
                return 0;
            }
        }

        if (upperCase) {
            acc |= 28 << accBits;
            accBits += 5;
        }

        acc |= code << accBits;
        accBits += 5;

        int maxBitsToKeep = c != 0 ? 7 : 0;

        while (accBits > maxBitsToKeep) {
            if (dest != NULL) {
                *dest = (unsigned char) acc;
                dest++;
            }

            accBits -= 8;
            acc >>= 8;
            bytes++;
        }
    } while (c != 0);

    return bytes;
}

int LuaRuntime::UnpackIdentifier(const unsigned char *src, char *dest) {
    int acc = 0;
    int accBits = 0;
    bool upperCase = false;
    const unsigned char *srcStart = src;

    for (;;) {
        if (accBits < 5) {
            acc |= *src++ << accBits;
            accBits += 8;
        }

        int code = acc & 31;
        accBits -= 5;
        acc >>= 5;

        if (code == 0) {
            *dest = 0;
            break;
        } else if (code <= 26) {
            if (upperCase) {
                *dest++ = 'A' - 1 + code;
                upperCase = false;
            } else {
                *dest++ = 'a' - 1 + code;
            }
        } else if (code == 27) {
            *dest++ = '_';
        } else if (code == 28) {
            upperCase = true;
        }
    }

    return src - srcStart;
}

static inline void CopyUnalignedWord32(void *dest, void *src) {
    const unsigned char *s = (const unsigned char *)src;
    unsigned char *d = (unsigned char *)dest;
    d[0] = s[0];
    d[1] = s[1];
    d[2] = s[2];
    d[3] = s[3];
}

unsigned int LuaRuntime::SerializeTable(lua_State *state, unsigned char *buffer, bool allowUserData) {
    unsigned int size = 0;




    bool packKeyStrings = (allowUserData == false);


    if (lua_type(state, -1) == LUA_TTABLE) {



        for (lua_pushnil(state); lua_next(state, -2); lua_settop(state, -2)) {



            int keyType = lua_type(state, -2);
            int valueType = lua_type(state, -1);



            switch (keyType) {
            case LUA_TNIL:
            case LUA_TLIGHTUSERDATA:
            case LUA_TTABLE:
            case LUA_TFUNCTION:
            case LUA_TUSERDATA:
            case LUA_TTHREAD:
                continue;
            }


            switch (valueType) {
            case LUA_TLIGHTUSERDATA:
            case LUA_TUSERDATA:
                if (!allowUserData) {
                    continue;
                }
                break;

            case LUA_TNIL:
            case LUA_TFUNCTION:
            case LUA_TTHREAD:
                continue;
            }






            for (int stackPos = -2; stackPos < 0; stackPos++) {
                TableVar header;
                TableVar *headerPos;

                header.mValid = 1;
                header.mIsKey = (stackPos == -2);
                header.mVarType = lua_type(state, stackPos);



                headerPos = &header;

                if (buffer != NULL) {

                    headerPos = (TableVar *)&buffer[size];
                    *headerPos = header;
                }

                size++;

                switch (header.mVarType) {




                case LUA_TBOOLEAN:
                    headerPos->mBoolValue = (lua_toboolean(state, stackPos) != 0);
                    break;





                case LUA_TNUMBER: {

                    float number = lua_tonumber(state, stackPos);
                    char packed = (char)number;
                    float unpacked = (float)packed;

                    if (number == unpacked) {
                        headerPos->mPacked = 1;
                    }
                    if (headerPos->mPacked) {



                        if (buffer != NULL) {
                            buffer[size] = packed;
                        }
                        size++;
                    } else {




                        if (buffer != NULL) {
                            CopyUnalignedWord32(&buffer[size], &number);
                        }
                        size += 4;
                    }
                    break;
                }




                case LUA_TSTRING: {
                    const char *str = lua_tostring(state, stackPos);
                    unsigned int strLength = bStrLen(str);
                    int packedLen = 0;

                    if (headerPos->mIsKey && packKeyStrings) {
                        packedLen = PackIdentifier(str, NULL);
                    }
                    if (packedLen > 0 && packedLen < (int)(strLength + 1) && packedLen <= 128) {

                        if (buffer != NULL) {
                            PackIdentifier(str, &buffer[size]);
                        }
                        headerPos->mPacked = 1;
                        size += packedLen;
                    } else {


                        if (buffer != NULL) {
                            bStrCpy((char *)&buffer[size], str);
                        }
                        size += strLength + 1;
                    }
                    break;
                }




                case LUA_TLIGHTUSERDATA: {

                    void *data = lua_touserdata(state, stackPos);

                    if (buffer != NULL) {


                        CopyUnalignedWord32(&buffer[size], data);
                    }
                    size += 4;
                    break;
                }




                case LUA_TUSERDATA: {
                    unsigned int dataSize = lua_userdatalen(state, stackPos);


                    if (buffer != NULL) {
                        buffer[size] = dataSize;
                    }
                    size++;



                    void *data = lua_touserdata(state, stackPos);

                    if (buffer != NULL) {
                        bMemCpy(&buffer[size], data, dataSize);
                    }
                    size += dataSize;



                    unsigned int metatableHash = 0;

                    lua_getmetatable(state, stackPos);

                    if (lua_type(state, -1) == LUA_TTABLE) {

                        lua_rawget(state, LUA_REGISTRYINDEX);

                        if (lua_isstring(state, -1)) {
                            metatableHash = stringhash32(lua_tostring(state, -1));
                        }
                    }

                    lua_settop(state, -2);

                    if (buffer != NULL) {
                        CopyUnalignedWord32(&buffer[size], &metatableHash);
                    }
                    size += 4;
                    break;
                }






                case LUA_TTABLE:
                    size += SerializeTable(state, buffer != NULL ? &buffer[size] : NULL, allowUserData);
                    break;
                }
            }
        }
    }


    TableVar terminator;

    terminator.mValid = 0;

    if (buffer != NULL) {
        buffer[size] = *(unsigned char *)&terminator;
    }



    return size + 1;
}

unsigned int LuaRuntime::DeserializeTable(lua_State *state, unsigned char *buffer, bool allowUserData) {
    if (buffer == NULL) {
        return 0;
    }

    unsigned char *bufferStart = buffer;
    int prevStackTop = lua_gettop(state);

    lua_newtable(state);

    TableVar *header = (TableVar *)buffer;
    buffer++;

    bool onKey = true;

    while (header->mValid) {
        int type;

        if (onKey != header->mIsKey) {
            goto LabelDeserializeFailed;
        }




        type = header->mVarType;

        switch (type) {




        case LUA_TBOOLEAN:
            lua_pushboolean(state, header->mBoolValue);
            break;






        case LUA_TNUMBER: {
            float numValue;

            if (header->mPacked) {



                numValue = (float)(char)*buffer;
                buffer++;
            } else {






                CopyUnalignedWord32(&numValue, buffer);
                buffer += 4;
            }

            if (UMath::IsNaN(numValue)) {
                goto LabelDeserializeFailed;
            }




            lua_pushnumber(state, numValue);
            break;
        }





        case LUA_TSTRING:
            if (header->mPacked) {



                char unpacked[128];
                buffer += UnpackIdentifier(buffer, unpacked);
                lua_pushstring(state, unpacked);
            } else {



                lua_pushstring(state, (const char *)buffer);
                buffer += bStrLen((const char *)buffer) + 1;
            }
            break;





        case LUA_TLIGHTUSERDATA: {
            void *lightUserData;

            if (onKey) {
                goto LabelDeserializeFailed;
            }

            if (!allowUserData) {
                goto LabelDeserializeFailed;
            }











            CopyUnalignedWord32(&lightUserData, buffer);
            buffer += 4;

            lua_pushlightuserdata(state, lightUserData);
            break;
        }





        case LUA_TUSERDATA: {
            unsigned char dataSize;
            void *userData;
            unsigned int metatableHash;

            if (onKey) {
                goto LabelDeserializeFailed;
            }

            if (!allowUserData) {
                goto LabelDeserializeFailed;
            }







            dataSize = *buffer;
            buffer++;



            userData = lua_newuserdata(state, dataSize);
            bMemCpy(userData, buffer, dataSize);



            buffer += dataSize;




            CopyUnalignedWord32(&metatableHash, buffer);

            buffer += 4;




            if (metatableHash != 0) {




                bool found = false;

                lua_pushvalue(state, LUA_REGISTRYINDEX);

                for (lua_pushnil(state); lua_next(state, -2); lua_settop(state, -2)) {

                    if (lua_isstring(state, -2)) {

                        unsigned int checkHash = stringhash32(lua_tostring(state, -2));

                        if (checkHash == metatableHash) {









                            lua_setmetatable(state, -4);
                            found = true;

                            lua_settop(state, -2);
                            break;
                        }
                    }
                }





                lua_settop(state, -2);




                if (!found) {
                    goto LabelDeserializeFailed;
                }
            }





            break;
        }





        case LUA_TTABLE: {
            unsigned int tableSize;

            if (onKey) {
                goto LabelDeserializeFailed;
            }




            tableSize = DeserializeTable(state, buffer, allowUserData);

            if (tableSize == 0) {
                goto LabelDeserializeFailed;
            }

            buffer += tableSize;
            break;
        }

        default:
            goto LabelDeserializeFailed;
        }










        if (!onKey) {
            lua_settable(state, -3);
        }




        header = (TableVar *)buffer;
        buffer++;




        onKey = !onKey;
    }

    if (!onKey) {
        goto LabelDeserializeFailed;
    }







    return buffer - bufferStart;




LabelDeserializeFailed:
    lua_settop(state, prevStackTop);
    return 0;
}

void LuaRuntime::DumpStack(lua_State *luaState) {
    int count = lua_gettop(luaState);

    {
        char buf[80];

        for (int onIdx = 1; onIdx < count + 1; onIdx++) {
            int rev = onIdx - 1;
            sprintf(buf, "%d/-%d", onIdx, count - rev);
        }
    }

    for (int onIdx = 1; onIdx < count + 1; onIdx++) {
    }

    {
        char buf[1024];

        for (int onIdx = 1; onIdx < count + 1; onIdx++) {
            buf[0] = 0;

            int onType = lua_type(luaState, onIdx);

            switch (onType) {
            case LUA_TNIL:
                strcpy(buf, "nil");
                break;

            case LUA_TBOOLEAN:
                sprintf(buf, "%s", lua_toboolean(luaState, onIdx) ? "true" : "false");
                break;

            case LUA_TNUMBER:
                sprintf(buf, "%.2f", lua_tonumber(luaState, onIdx));
                break;

            case LUA_TSTRING:
                strcpy(buf, lua_tostring(luaState, onIdx));
                break;

            case LUA_TLIGHTUSERDATA:
            case LUA_TTABLE:
            case LUA_TFUNCTION:
            case LUA_TUSERDATA:
            case LUA_TTHREAD:
                sprintf(buf, "%08X", lua_topointer(luaState, onIdx));
                break;
            }

            if (strlen(buf) > 8) {
                buf[8] = 0;
            }
        }
    }
}

int LuaRuntime::HandleGlobalIndex(lua_State *luaState) {
    if (lua_isstring(luaState, -1)) {
        const char *keyName = lua_tostring(luaState, -1);

        if (keyName != NULL && keyName[0] == 'E') {
            int (*bindingFunc)(lua_State *) = RegisterEvent::GetLuaBinding(stringhash32(keyName));

            if (bindingFunc != NULL) {
                lua_pushcclosure(luaState, bindingFunc, 0);
                lua_pushvalue(luaState, -2);
                lua_pushvalue(luaState, -2);
                lua_settable(luaState, LUA_GLOBALSINDEX);

                return 1;
            }
        }
    }

    lua_pushnil(luaState);

    return 1;
}
