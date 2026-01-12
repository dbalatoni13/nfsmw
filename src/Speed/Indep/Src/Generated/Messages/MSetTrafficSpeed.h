#ifndef GENERATED_MESSAGES_MSETTRAFFICSPEED_H
#define GENERATED_MESSAGES_MSETTRAFFICSPEED_H

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x1C
class MSetTrafficSpeed : public Hermes::Message {
  public:
    static unsigned int _GetSize() {
        return sizeof(MSetTrafficSpeed);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MSetTrafficSpeed");
        return k;
    }

    MSetTrafficSpeed(float _SpeedDefault, float _SpeedHighway, unsigned int _FixSpeed) : Hermes::Message(_GetKind(), _GetSize(), 0) {
        fSpeedDefault = _SpeedDefault;
        fSpeedHighway = _SpeedHighway;
        fFixSpeed = _FixSpeed;
    }

    ~MSetTrafficSpeed() {}

    // float GetSpeedDefault() const {}

    // void SetSpeedDefault(float _SpeedDefault) {}

    // float GetSpeedHighway() const {}

    // void SetSpeedHighway(float _SpeedHighway) {}

    // unsigned int GetFixSpeed() const {}

    // void SetFixSpeed(unsigned int _FixSpeed) {}

    // int PostMessage_LuaBinding(struct lua_State *luaState) {
    //     // Local variables
    //     int numArgs;
    // }

    // void BindToLua(struct lua_State *luaState) {}

    // void HandleMessage_LuaBinding(const MSetTrafficSpeed &message) {
    //     // Local variables
    //     // struct LuaMessageDeliveryInfo deliveryInfo;
    // }

    // void BuildMessageTable(struct lua_State *luaState, const Message *messageBase) {
    //     // Local variables
    //     // const struct MSetTrafficSpeed &message;
    // }

  private:
    float fSpeedDefault;    // offset 0x10, size 0x4
    float fSpeedHighway;    // offset 0x14, size 0x4
    unsigned int fFixSpeed; // offset 0x18, size 0x4
};

#endif
