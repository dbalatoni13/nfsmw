#ifndef GENERATED_MESSAGES_MICECAMERAFINISHED_H
#define GENERATED_MESSAGES_MICECAMERAFINISHED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MICECameraFinished : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MICECameraFinished);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MICECameraFinished");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MICECameraFinished &message);

    MICECameraFinished() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MICECameraFinished() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MICECameraFinished::HandleMessage_LuaBinding(const MICECameraFinished &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MICECameraFinished::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
