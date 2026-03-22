#include "Speed/Indep/Src/Gameplay/GHandler.h"

#include "Speed/Indep/Src/Gameplay/LuaMessageDeliveryInfo.h"
#include "Speed/Indep/Src/Lua/source/lauxlib.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Lua/LuaRuntime.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Misc/LZCompress.hpp"

void LZByteSwapHeader(LZHeader *header);

typedef void (*QueryFunc)(IMessageFilterContext *, int, const void *, int, bool *);

struct MessageFilterHeader {
    unsigned char mSourceOffset;
    unsigned char mDestOffset;
    unsigned char mSize;
    unsigned char mInitialized;
    QueryFunc mQuery;
    unsigned char mQueryStaticData[1];
};

void ByteSwapStaticData(const void *data) __asm__("ByteSwapStaticData__5QueryPCv");
QueryFunc LookupQueryFunc(unsigned int key) __asm__("LookupQueryFunc__5QueryUi");

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
    struct BlobView {
        unsigned int mSize;
        const void *mData;
    };

    if (!mAttached) {
        Attrib::Blob blob;
        bool hasBytecode;

        reinterpret_cast<BlobView &>(blob).mSize = 0;
        reinterpret_cast<BlobView &>(blob).mData = nullptr;
        hasBytecode = false;

        if (const Attrib::Blob *blobPtr = reinterpret_cast<const Attrib::Blob *>(GetAttributePointer(0x9a4a020a, 0))) {
            blob = *blobPtr;
            hasBytecode = true;
        }

        if (hasBytecode && reinterpret_cast<const BlobView &>(blob).mSize != 0) {
            int dobuffer_status;
            unsigned char *compressedBlock =
                reinterpret_cast<unsigned char *>(const_cast<void *>(reinterpret_cast<const BlobView &>(blob).mData));
            LZHeader *lzHeader = reinterpret_cast<LZHeader *>(compressedBlock);

            LZByteSwapHeader(lzHeader);
            if (!LZValidHeader(lzHeader)) {
                dobuffer_status =
                    lua_dobuffer(luaState, reinterpret_cast<const char *>(compressedBlock), reinterpret_cast<const BlobView &>(blob).mSize, "Handler");
            } else {
                const unsigned int kStackBufferSize = 0x1000;
                unsigned char stackBuffer[kStackBufferSize];
                unsigned char *decompressionBuffer = stackBuffer;
                unsigned char *heapBuffer = nullptr;

                if (lzHeader->UncompressedSize > kStackBufferSize) {
                    heapBuffer = new unsigned char[lzHeader->UncompressedSize];
                    decompressionBuffer = heapBuffer;
                }

                LZDecompress(reinterpret_cast<uint8 *>(lzHeader), decompressionBuffer);
                dobuffer_status = lua_dobuffer(
                    luaState,
                    reinterpret_cast<const char *>(decompressionBuffer),
                    lzHeader->UncompressedSize,
                    "Handler");
                if (heapBuffer) {
                    delete[] heapBuffer;
                }
            }
            LZByteSwapHeader(lzHeader);

            if (dobuffer_status == 0) {
                mAttached = true;
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
    IsScripted();
    ExecuteScriptedHandler(deliveryInfo);
}

bool GHandler::MessagePassesFilters(LuaMessageDeliveryInfo *deliveryInfo) {
    struct BlobView {
        unsigned int mSize;
        const void *mData;
    };

    for (unsigned int onFilter = 0;; onFilter++) {
        if (onFilter >= Num_FilterBlocks()) {
            return true;
        }

        Attrib::Blob blob;

        reinterpret_cast<BlobView &>(blob).mSize = 0;
        reinterpret_cast<BlobView &>(blob).mData = nullptr;

        const Attrib::Blob *blobPtr = reinterpret_cast<const Attrib::Blob *>(GetAttributePointer(0x56e1436d, onFilter));
        if (blobPtr) {
            blob = *blobPtr;
        }

        unsigned char *blockData =
            reinterpret_cast<unsigned char *>(const_cast<void *>(reinterpret_cast<const BlobView &>(blob).mData));

        if (blockData) {
            MessageFilterHeader *filter = reinterpret_cast<MessageFilterHeader *>(blockData);
            bool filterPassed = true;

            if (!filter->mInitialized) {
                ByteSwapStaticData(filter->mQueryStaticData);
                filter->mQuery = LookupQueryFunc(*reinterpret_cast<unsigned int *>(filter->mQueryStaticData));
                filter->mInitialized = 1;
            }

            IMessageFilterContext *filterContext = deliveryInfo;
            const unsigned char *source = reinterpret_cast<const unsigned char *>(filterContext->GetMessage()) + sizeof(Hermes::Message) + filter->mSourceOffset;
            unsigned char *dest = blockData + filter->mDestOffset + 0xC;

            bMemCpy(dest, source, filter->mSize);

            if (filter->mQuery) {
                filter->mQuery(filterContext, 0, filter->mQueryStaticData, 1, &filterPassed);
            }

            if (FilterModePassAll(0)) {
                if (!filterPassed) {
                    return false;
                }
            } else if (filterPassed) {
                return true;
            }
        }
    }
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
