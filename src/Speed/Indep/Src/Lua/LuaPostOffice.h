#ifndef LUA_LUAPOSTOFFICE_H
#define LUA_LUAPOSTOFFICE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Gameplay/GActivity.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

struct _type_ID_LuaActivityList {
    const char *name() {
        return "ID_LuaActivityList";
    };
};

struct _type_ID_LuaMessageSubscriberMap {
    const char *name() {
        return "ID_LuaMessageSubscriberMap";
    };
};

// total size: 0x20
class LuaPostOffice {
  public:
    static void Init();
    static void Shutdown();

    LuaPostOffice &Get() {
        return *fObj;
    }

  private:
    static LuaPostOffice *fObj;

    UTL::Std::map<unsigned int, UTL::Std::vector<GActivity *, _type_ID_LuaActivityList>, _type_ID_LuaMessageSubscriberMap>
        mSubscribers;                                                                          // offset 0x0, size 0x10
    UTL::Std::vector<Hermes::HHANDLER, Hermes::_type_ID_HermesHandlerVector> mBindingHandlers; // offset 0x10, size 0x10
};

#endif
