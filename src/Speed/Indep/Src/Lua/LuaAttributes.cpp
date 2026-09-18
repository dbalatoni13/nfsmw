#include "Speed/Indep/Src/Lua/LuaAttributes.h"

#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaRuntime.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

extern "C" {
#include "Speed/Indep/Src/Lua/source/lauxlib.h"
}

static void PushAttribValueFloat(const LuaAttribAccessInfo &info) {
    lua_pushnumber(info.mLuaState, info.mAttribute->Get<float>(info.mAttribIndex));
}

static void PushAttribValueDouble(const LuaAttribAccessInfo &info) {
    lua_pushnumber(info.mLuaState, info.mAttribute->Get<double>(info.mAttribIndex));
}

static void PushAttribValueInt64(const LuaAttribAccessInfo &info) {
    lua_pushnumber(info.mLuaState, info.mAttribute->Get<int64_t>(info.mAttribIndex));
}

static void PushAttribValueInt32(const LuaAttribAccessInfo &info) {
    lua_pushnumber(info.mLuaState, info.mAttribute->Get<int>(info.mAttribIndex));
}

static void PushAttribValueInt16(const LuaAttribAccessInfo &info) {
    lua_pushnumber(info.mLuaState, info.mAttribute->Get<short>(info.mAttribIndex));
}

static void PushAttribValueInt8(const LuaAttribAccessInfo &info) {
    lua_pushnumber(info.mLuaState, info.mAttribute->Get<signed char>(info.mAttribIndex));
}

static void PushAttribValueUInt64(const LuaAttribAccessInfo &info) {
    lua_pushnumber(info.mLuaState, info.mAttribute->Get<uint64_t>(info.mAttribIndex));
}

static void PushAttribValueUInt32(const LuaAttribAccessInfo &info) {
    lua_pushnumber(info.mLuaState, info.mAttribute->Get<unsigned int>(info.mAttribIndex));
}

static void PushAttribValueUInt16(const LuaAttribAccessInfo &info) {
    lua_pushnumber(info.mLuaState, info.mAttribute->Get<unsigned short>(info.mAttribIndex));
}

static void PushAttribValueUInt8(const LuaAttribAccessInfo &info) {
    lua_pushnumber(info.mLuaState, info.mAttribute->Get<unsigned char>(info.mAttribIndex));
}

static void PushAttribValueBool(const LuaAttribAccessInfo &info) {
    lua_pushboolean(info.mLuaState, info.mAttribute->Get<bool>(info.mAttribIndex) ? 1 : 0);
}

static void PushAttribValueStringKey(const LuaAttribAccessInfo &info) {
    lua_pushstring(info.mLuaState, info.mAttribute->Get<Attrib::StringKey>(info.mAttribIndex).GetString());
}

static void PushAttribValueText(const LuaAttribAccessInfo &info) {
    lua_pushstring(info.mLuaState, info.mAttribute->Get<const char *>(info.mAttribIndex));
}

static void PushAttribValueChar(const LuaAttribAccessInfo &info) {
    char string[2];

    string[0] = info.mAttribute->Get<char>(info.mAttribIndex);
    string[1] = '\0';
    lua_pushstring(info.mLuaState, string);
}

static void PushAttribValueRefSpec(const LuaAttribAccessInfo &info) {
    GRuntimeInstance *target = info.mRuntimeInstance->GetConnectedInstance(info.mAttribute->GetKey(), info.mAttribIndex);

    if (target != NULL) {
        *static_cast<GRuntimeInstance **>(lua_newuserdata(info.mLuaState, sizeof(GRuntimeInstance *))) = target;
        LuaBindery::AttachMetatable(info.mLuaState, "GRuntimeInstance");
    } else {
        lua_pushnil(info.mLuaState);
    }
}

static bool IsSameInstance(const GRuntimeInstance *a, const GRuntimeInstance *b) {
    if (a == b) {
        return true;
    }

    if ((a != NULL && b == NULL) || (b != NULL && a == NULL)) {
        return false;
    }

    if (a->IsDynamic() && a->GetParent() == b->GetCollection()) {
        return true;
    }

    if (b->IsDynamic() && b->GetParent() == a->GetCollection()) {
        return true;
    }

    return false;
}

static int GRuntimeInstanceMeta__index(lua_State *luaState) {
    void *attribPtrAddr = luaL_checkudata(luaState, 1, "GRuntimeInstance");

    if (attribPtrAddr == NULL) {
        luaL_argerror(luaState, 1, "GRuntimeInstance expected");
    }

    GRuntimeInstance *instance = *static_cast<GRuntimeInstance **>(attribPtrAddr);
    const char *fieldName = lua_tostring(luaState, 2);
    unsigned int attribKey = Attrib::StringToKey(fieldName);
    Attrib::Attribute attrib = instance->Get(attribKey);

    if (!attrib.IsValid()) {
        GRuntimeInstance *target = instance->GetConnectedInstance(attribKey, 0);

        if (target != NULL) {
            GRuntimeInstance **targetPtr = static_cast<GRuntimeInstance **>(lua_newuserdata(luaState, sizeof(GRuntimeInstance *)));

            *targetPtr = target;
            LuaBindery::AttachMetatable(luaState, "GRuntimeInstance");
            return 1;
        }
    }

    LuaAttribAccessInfo accessInfo;

    accessInfo.mLuaState = luaState;
    accessInfo.mAttribIndex = 0;
    accessInfo.mRuntimeInstance = instance;
    accessInfo.mAttribute = &attrib;
    LuaAttributes::Get().PushAttributeValue(accessInfo, false);
    return 1;
}

static int GRuntimeInstanceMeta__newindex(lua_State *luaState) {
    void *attribPtrAddr = luaL_checkudata(luaState, 1, "GRuntimeInstance");

    if (attribPtrAddr == NULL) {
        luaL_argerror(luaState, 1, "GRuntimeInstance expected");
    }

    GRuntimeInstance *instance = *static_cast<GRuntimeInstance **>(attribPtrAddr);
    const char *fieldName = lua_tostring(luaState, 2);
    unsigned int attribKey = Attrib::StringToKey(fieldName);
    Attrib::Attribute attrib = instance->Get(attribKey);
    LuaAttribAccessInfo accessInfo;

    accessInfo.mLuaState = luaState;
    accessInfo.mAttribIndex = 0;
    accessInfo.mRuntimeInstance = instance;
    accessInfo.mSrcStackIndex = 3;
    accessInfo.mAttribute = &attrib;
    LuaAttributes::Get().SetAttributeValue(accessInfo);
    return 0;
}

static int GRuntimeInstanceMeta__eq(lua_State *luaState) {
    void *attribPtrAddr = luaL_checkudata(luaState, 1, "GRuntimeInstance");
    void *otherPtrAddr = luaL_checkudata(luaState, 2, "GRuntimeInstance");

    if (attribPtrAddr == NULL) {
        luaL_argerror(luaState, 1, "GRuntimeInstance expected");
    }

    if (otherPtrAddr == NULL) {
        luaL_argerror(luaState, 2, "GRuntimeInstance expected");
    }

    GRuntimeInstance *instance = *static_cast<GRuntimeInstance **>(attribPtrAddr);
    GRuntimeInstance *other = *static_cast<GRuntimeInstance **>(otherPtrAddr);

    lua_pushboolean(luaState, IsSameInstance(instance, other));
    return 1;
}

static int LuaAttribArrayInfoMeta__index(lua_State *luaState) {
    void *arrayInfoAddr = luaL_checkudata(luaState, 1, "LuaAttribArrayInfo");

    if (arrayInfoAddr == NULL) {
        luaL_argerror(luaState, 1, "LuaAttribArrayInfo expected");
    }

    LuaAttribArrayInfo &info = *static_cast<LuaAttribArrayInfo *>(arrayInfoAddr);
    Attrib::Attribute attribute = info.mRuntimeInstance->Get(info.mAttributeKey);

    if (lua_isnumber(luaState, 2)) {
        int index = static_cast<int>(lua_tonumber(luaState, 2)) - 1;

        if (index < 0) {
            lua_pushnil(luaState);
            return 1;
        }

        LuaAttribAccessInfo accessInfo;

        accessInfo.mLuaState = luaState;
        accessInfo.mAttribute = &attribute;
        accessInfo.mAttribIndex = index;
        accessInfo.mRuntimeInstance = info.mRuntimeInstance;

        LuaAttributes::Get().PushAttributeValue(accessInfo, true);
        return 1;
    }

    if (lua_isstring(luaState, 2)) {
        const char *propertyName = lua_tostring(luaState, 2);

        if (bStrCmp(propertyName, "Count") == 0) {
            lua_pushnumber(luaState, attribute.GetLength());
            return 1;
        }
    }

    return 0;
}

static int LuaAttribArrayInfoMeta__newindex(lua_State *luaState) {
    return 0;
}

static int LuaAttribArrayInfoMeta__eq(lua_State *luaState) {
    void *thisInfoAddr = luaL_checkudata(luaState, 1, "LuaAttribArrayInfo");
    void *otherInfoAddr = luaL_checkudata(luaState, 2, "LuaAttribArrayInfo");

    if (thisInfoAddr == NULL) {
        luaL_argerror(luaState, 1, "LuaAttribArrayInfo expected");
    }

    if (otherInfoAddr == NULL) {
        luaL_argerror(luaState, 2, "LuaAttribArrayInfo expected");
    }

    LuaAttribArrayInfo &thisInfo = *static_cast<LuaAttribArrayInfo *>(thisInfoAddr);
    LuaAttribArrayInfo &otherInfo = *static_cast<LuaAttribArrayInfo *>(otherInfoAddr);
    bool sameRef = false;

    if (IsSameInstance(thisInfo.mRuntimeInstance, otherInfo.mRuntimeInstance)) {
        sameRef = (thisInfo.mAttributeKey == otherInfo.mAttributeKey);
    }

    lua_pushboolean(luaState, sameRef);
    return 1;
}

LuaAttributes *LuaAttributes::fObj = nullptr;

LuaAttributes::LuaAttributes() {
    fGameplayClass = Attrib::Database::Get().GetClass(Attrib::ClassName::gameplay);
    BuildAttributeTypeTable();
}

LuaAttributes::~LuaAttributes() {}

void LuaAttributes::Init() {
    fObj = new ("LuaAttributes", 0) LuaAttributes();
}

void LuaAttributes::Shutdown() {
    delete fObj;
    fObj = NULL;
}

void LuaAttributes::BuildAttributeTypeTable() {
    static struct {
        const char *mName;
        LuaAttribAccessors mAccessors;
    } accessorTable[] = {
        {"EA::Reflection::Float", {PushAttribValueFloat, NULL}},
        {"EA::Reflection::Double", {PushAttribValueDouble, NULL}},
        {"EA::Reflection::Int64", {PushAttribValueInt64, NULL}},
        {"EA::Reflection::Int32", {PushAttribValueInt32, NULL}},
        {"EA::Reflection::Int16", {PushAttribValueInt16, NULL}},
        {"EA::Reflection::Int8", {PushAttribValueInt8, NULL}},
        {"EA::Reflection::UInt64", {PushAttribValueUInt64, NULL}},
        {"EA::Reflection::UInt32", {PushAttribValueUInt32, NULL}},
        {"EA::Reflection::UInt16", {PushAttribValueUInt16, NULL}},
        {"EA::Reflection::UInt8", {PushAttribValueUInt8, NULL}},
        {"EA::Reflection::Char", {PushAttribValueChar, NULL}},
        {"EA::Reflection::Bool", {PushAttribValueBool, NULL}},
        {"EA::Reflection::Text", {PushAttribValueText, NULL}},
        {"GCollectionKey", {PushAttribValueRefSpec, NULL}},
        {"Attrib::RefSpec", {PushAttribValueRefSpec, NULL}},
        {"Attrib::StringKey", {PushAttribValueStringKey, NULL}},
    };
    int tableSize = sizeof(accessorTable) / sizeof(accessorTable[0]);

    for (unsigned int onType = 0; onType < Attrib::Database::Get().GetNumIndexedTypes(); onType++) {
        const Attrib::TypeDesc &typeDesc = Attrib::Database::Get().GetIndexedTypeDesc((unsigned short)onType);
        LuaAttribAccessors &accessors = fTypeAccessors[typeDesc.GetType()];

        accessors.mPushAttrib = NULL;
        accessors.mSetAttrib = NULL;

        for (int i = 0; i < tableSize; i++) {
            if (bStrCmp(accessorTable[i].mName, typeDesc.GetName()) == 0) {
                accessors = accessorTable[i].mAccessors;
                break;
            }
        }
    }
}

void LuaAttributes::PushAttributeValue(const LuaAttribAccessInfo &info, bool arrayAccess) {
    if (!info.mAttribute->IsValid()) {
        lua_pushnil(info.mLuaState);
        return;
    }

    bool isArray = fGameplayClass->GetDefinition(info.mAttribute->GetKey())->IsArray();

    if (isArray && !arrayAccess) {
        LuaAttribArrayInfo *arrayInfo =
            (LuaAttribArrayInfo *)lua_newuserdata(info.mLuaState, sizeof(LuaAttribArrayInfo));

        arrayInfo->mRuntimeInstance = info.mRuntimeInstance;
        arrayInfo->mAttributeKey = info.mAttribute->GetKey();

        LuaBindery::AttachMetatable(info.mLuaState, "LuaAttribArrayInfo");
    } else {
        UTL::Std::map<unsigned int, LuaAttribAccessors, _type_map>::iterator iter =
            fTypeAccessors.find(info.mAttribute->GetType());

        if (iter != fTypeAccessors.end()) {
            if ((*iter).second.mPushAttrib != NULL) {
                (*iter).second.mPushAttrib(info);
            }
        } else {
            lua_pushnil(info.mLuaState);
        }
    }
}

void LuaAttributes::SetAttributeValue(const LuaAttribAccessInfo &info) {
    UTL::Std::map<unsigned int, LuaAttribAccessors, _type_map>::iterator iter =
        fTypeAccessors.find(info.mAttribute->GetType());

    if (iter != fTypeAccessors.end()) {
        if ((*iter).second.mSetAttrib != NULL) {
            (*iter).second.mSetAttrib(info);
        }
    }
}

void LuaAttributes::BindAccessors(lua_State *luaState) {
    LuaBindery::LoadMetatable(luaState, "GRuntimeInstance");
    lua_pushstring(luaState, "__index");
    lua_pushcclosure(luaState, GRuntimeInstanceMeta__index, 0);
    lua_settable(luaState, -3);
    lua_pushstring(luaState, "__newindex");
    lua_pushcclosure(luaState, GRuntimeInstanceMeta__newindex, 0);
    lua_settable(luaState, -3);
    lua_pushstring(luaState, "__eq");
    lua_pushcclosure(luaState, GRuntimeInstanceMeta__eq, 0);
    lua_settable(luaState, -3);
    lua_settop(luaState, -2);

    LuaBindery::LoadMetatable(luaState, "LuaAttribArrayInfo");
    lua_pushstring(luaState, "__index");
    lua_pushcclosure(luaState, LuaAttribArrayInfoMeta__index, 0);
    lua_settable(luaState, -3);
    lua_pushstring(luaState, "__newindex");
    lua_pushcclosure(luaState, LuaAttribArrayInfoMeta__newindex, 0);
    lua_settable(luaState, -3);
    lua_pushstring(luaState, "__eq");
    lua_pushcclosure(luaState, LuaAttribArrayInfoMeta__eq, 0);
    lua_settable(luaState, -3);
    lua_settop(luaState, -2);
}
