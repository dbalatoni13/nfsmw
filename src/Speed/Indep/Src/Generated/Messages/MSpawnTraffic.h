#ifndef GENERATED_MESSAGES_MSPAWNTRAFFIC_H
#define GENERATED_MESSAGES_MSPAWNTRAFFIC_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x2c
class MSpawnTraffic : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MSpawnTraffic);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MSpawnTraffic");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MSpawnTraffic &message);

    MSpawnTraffic(UMath::Vector3 _InitialPos, UMath::Vector3 _InitialVec, const char *_VehicleName)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fInitialPos(_InitialPos), fInitialVec(_InitialVec), fVehicleName(_VehicleName) {}

    ~MSpawnTraffic() {}

    UMath::Vector3 GetInitialPos() const {
        return fInitialPos;
    }

    void SetInitialPos(UMath::Vector3 _InitialPos) {
        fInitialPos = _InitialPos;
    }

    UMath::Vector3 GetInitialVec() const {
        return fInitialVec;
    }

    void SetInitialVec(UMath::Vector3 _InitialVec) {
        fInitialVec = _InitialVec;
    }

    const char *GetVehicleName() const {
        return fVehicleName;
    }

    void SetVehicleName(const char *_VehicleName) {
        fVehicleName = _VehicleName;
    }

  private:
    UMath::Vector3 fInitialPos; // offset 0x10, size 0xc
    UMath::Vector3 fInitialVec; // offset 0x1c, size 0xc
    const char *fVehicleName;   // offset 0x28, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MSpawnTraffic::HandleMessage_LuaBinding(const MSpawnTraffic &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MSpawnTraffic::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MSpawnTraffic *message = static_cast<const MSpawnTraffic *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "InitialPos");
    lua_pushnil(luaState);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "InitialVec");
    lua_pushnil(luaState);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "VehicleName");
    lua_pushstring(luaState, message->fVehicleName);
    lua_settable(luaState, -3);
}

#endif
