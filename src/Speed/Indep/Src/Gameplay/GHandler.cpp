#include "Speed/Indep/Src/Gameplay/GHandler.h"

#include "Speed/Indep/Src/Lua/LuaPostOffice.h"
#include "Speed/Indep/Src/Lua/LuaRuntime.h"
#include "Speed/Indep/Src/Lua/source/lauxlib.h"
#include "Speed/Indep/Src/Misc/LZCompress.hpp"

GHandler::GHandler(const Attrib::Key &handlerKey) : GRuntimeInstance(handlerKey, kGameplayObjType_Handler), mAttached(false) {
}

GHandler::~GHandler() {
    Detach(LuaRuntime::Get().GetState());
}

void GHandler::Attach(lua_State *luaState) {
    if (mAttached) {
        return;
    }

    Attrib::Blob blob;

    if (!bytecode(blob)) {
        return;
    }

    if (blob.GetSize() != 0) {
        unsigned char *compressedBlock = (unsigned char *) blob.GetData();
        int dobuffer_status;
        LZHeader *lzHeader = (LZHeader *) compressedBlock;

        LZByteSwapHeader(lzHeader);

        if (LZValidHeader(lzHeader)) {
            const unsigned int kStackBufferSize = 4096;
            unsigned char stackBuffer[kStackBufferSize];

            unsigned char *decompressionBuffer = stackBuffer;
            unsigned char *heapBuffer = NULL;

            if (lzHeader->UncompressedSize > kStackBufferSize) {
                heapBuffer = new ("Lua bytecode decompression buffer", 0) unsigned char[lzHeader->UncompressedSize];

                decompressionBuffer = heapBuffer;
            }

            LZDecompress(compressedBlock, decompressionBuffer);
            dobuffer_status = lua_dobuffer(luaState, (const char *) decompressionBuffer, lzHeader->UncompressedSize, "Handler");

            if (heapBuffer != NULL) {
                delete[] heapBuffer;
            }
        } else {
            dobuffer_status = lua_dobuffer(luaState, (const char *) compressedBlock, blob.GetSize(), "Handler");
        }

        LZByteSwapHeader(lzHeader);

        if (dobuffer_status == 0) {
            mAttached = true;
        }
    }
}

void GHandler::Detach(lua_State *luaState) {
    if (mAttached) {
        const char *name = CollectionName();

        lua_pushstring(luaState, name);
        lua_pushnil(luaState);
        lua_settable(luaState, LUA_GLOBALSINDEX);

        mAttached = false;
    }
}

void GHandler::NotifyBytecodeFlushed() {
    mAttached = false;
}

namespace Query {

// total size: 0x4
struct StaticData {
    unsigned int fQueryID; // offset 0x0, size 0x4
};

typedef bool (*QueryFunc)(const UTL::COM::IUnknown *, const UTL::COM::IUnknown *, const void *, unsigned int, void *);

QueryFunc LookupQueryFunc(unsigned int funcName);
void ByteSwapStaticData(const void *staticData);

} // namespace Query

// total size: 0xC
struct MessageFilterHeader {
    unsigned char mFieldMessageOffset; // offset 0x0, size 0x1
    unsigned char mFieldQueryOffset;   // offset 0x1, size 0x1
    unsigned char mFieldSize;          // offset 0x2, size 0x1
    unsigned char mInitialized;        // offset 0x3, size 0x1
    Query::QueryFunc mQueryFunc;       // offset 0x4, size 0x4
    Query::StaticData mQueryData;      // offset 0x8, size 0x4
};

bool GHandler::MessagePassesFilters(LuaMessageDeliveryInfo *deliveryInfo) {
    for (unsigned int onFilter = 0; onFilter < Num_FilterBlocks(); onFilter++) {
        Attrib::Blob blob;

        if (FilterBlocks(blob, onFilter)) {
            unsigned char *blockData = (unsigned char *) blob.GetData();
            MessageFilterHeader *filter = (MessageFilterHeader *) blockData;

            if (!filter->mInitialized) {
                Query::ByteSwapStaticData(&filter->mQueryData);

                filter->mQueryFunc = Query::LookupQueryFunc(filter->mQueryData.fQueryID);
                filter->mInitialized = true;
            }

            const unsigned char *source = (const unsigned char *) deliveryInfo->GetMessage() + sizeof(Hermes::Message);
            source += filter->mFieldMessageOffset;

            unsigned char *dest = blockData + sizeof(MessageFilterHeader);
            dest += filter->mFieldQueryOffset;

            bMemCpy(dest, source, filter->mFieldSize);

            bool filterPassed = true;

            Query::StaticData *queryData = &filter->mQueryData;

            if (filter->mQueryFunc != NULL) {
                filter->mQueryFunc(deliveryInfo, NULL, queryData, 1, &filterPassed);
            }

            if (FilterModePassAll()) {
                if (!filterPassed) {
                    return false;
                }
            } else {
                if (filterPassed) {
                    return true;
                }
            }
        }
    }

    return true;
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
