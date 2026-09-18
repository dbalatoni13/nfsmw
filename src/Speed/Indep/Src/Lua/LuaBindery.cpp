#include "Speed/Indep/Src/Lua/LuaBindery.h"

#include "Speed/Indep/Src/Interfaces/SimEntities/IOnlinePlayer.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Lua/LuaGameHooks.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern "C" {
#include "Speed/Indep/Src/Lua/source/lauxlib.h"
}

template <typename T> struct Parameter {
};

template <> struct Parameter<bool> {
    static bool Translate(lua_State *luaState, unsigned int stackIndex) {
        return lua_toboolean(luaState, stackIndex) != 0;
    }
};

template <> struct Parameter<int> {
    static int Translate(lua_State *luaState, unsigned int stackIndex) {
        return static_cast<int>(lua_tonumber(luaState, stackIndex));
    }
};

template <> struct Parameter<float> {
    static float Translate(lua_State *luaState, unsigned int stackIndex) {
        return lua_tonumber(luaState, stackIndex);
    }
};

template <> struct Parameter<const char *> {
    static const char *Translate(lua_State *luaState, unsigned int stackIndex) {
        return lua_tostring(luaState, stackIndex);
    }
};

template <> struct Parameter<GRuntimeInstance *> {
    static GRuntimeInstance *Translate(lua_State *luaState, unsigned int stackIndex) {
        void *userdata = lua_touserdata(luaState, stackIndex);

        if (userdata != NULL) {
            return *static_cast<GRuntimeInstance **>(userdata);
        }
        return NULL;
    }
};

template <> struct Parameter<GRaceStatus *> {
    static GRaceStatus *Translate(lua_State *luaState, unsigned int stackIndex) {
        void *userdata = lua_touserdata(luaState, stackIndex);

        if (userdata != NULL) {
            return *static_cast<GRaceStatus **>(userdata);
        }
        return NULL;
    }
};

template <> struct Parameter<ISimable *> {
    static ISimable *Translate(lua_State *luaState, unsigned int stackIndex) {
        HSIMABLE *userdata = static_cast<HSIMABLE *>(lua_touserdata(luaState, stackIndex));

        if (userdata != NULL) {
            return UTL::Collections::Instanceable<HSIMABLE, ISimable, 160>::FindInstance(*userdata);
        }
        return NULL;
    }
};

template <typename T> struct ReturnValue {
};

template <> struct ReturnValue<bool> {
    static int Translate(lua_State *luaState, bool value) {
        lua_pushboolean(luaState, value);
        return 1;
    }
};

template <> struct ReturnValue<float> {
    static int Translate(lua_State *luaState, float value) {
        lua_pushnumber(luaState, value);
        return 1;
    }
};

template <> struct ReturnValue<double> {
    static int Translate(lua_State *luaState, double value) {
        lua_pushnumber(luaState, value);
        return 1;
    }
};

template <> struct ReturnValue<int> {
    static int Translate(lua_State *luaState, int value) {
        lua_pushnumber(luaState, value);
        return 1;
    }
};

template <> struct ReturnValue<unsigned int> {
    static int Translate(lua_State *luaState, unsigned int value) {
        lua_pushnumber(luaState, value);
        return 1;
    }
};

template <> struct ReturnValue<const char *> {
    static int Translate(lua_State *luaState, const char *value) {
        lua_pushstring(luaState, value);
        return 1;
    }
};

template <> struct ReturnValue<GRuntimeInstance *> {
    static int Translate(lua_State *luaState, GRuntimeInstance *value) {
        if (value == NULL) {
            lua_pushnil(luaState);
        } else {
            GRuntimeInstance **userData = static_cast<GRuntimeInstance **>(lua_newuserdata(luaState, sizeof(GRuntimeInstance *)));

            *userData = value;
            luaL_getmetatable(luaState, "GRuntimeInstance");
            lua_setmetatable(luaState, -2);
        }
        return 1;
    }
};

template <> struct ReturnValue<ISimable *> {
    static int Translate(lua_State *luaState, ISimable *value) {
        if (value == NULL) {
            lua_pushnil(luaState);
        } else {
            HSIMABLE *userData = static_cast<HSIMABLE *>(lua_newuserdata(luaState, sizeof(HSIMABLE)));

            *userData = value->GetInstanceHandle();
            luaL_getmetatable(luaState, "ISimable");
            lua_setmetatable(luaState, -2);
        }
        return 1;
    }
};


template <typename R> struct Binder {
    typedef R (*FuncType)();

    static int Thunk(lua_State *luaState) {
        FuncType funcPtr = (FuncType)lua_touserdata(luaState, lua_upvalueindex(1));

        return ReturnValue<R>::Translate(luaState, funcPtr());
    }
};

template <typename R, typename A1> static int FunctionArg1Thunk(lua_State *luaState) {
    typedef R (*FuncType)(A1);

    FuncType funcPtr = (FuncType)lua_touserdata(luaState, lua_upvalueindex(1));

    return ReturnValue<R>::Translate(luaState, funcPtr(Parameter<A1>::Translate(luaState, 1)));
}

template <typename R, typename A1, typename A2> static int FunctionArg2Thunk(lua_State *luaState) {
    typedef R (*FuncType)(A1, A2);

    FuncType funcPtr = (FuncType)lua_touserdata(luaState, lua_upvalueindex(1));

    return ReturnValue<R>::Translate(luaState, funcPtr(Parameter<A1>::Translate(luaState, 1), Parameter<A2>::Translate(luaState, 2)));
}

struct VoidBinder {
    typedef void (*FuncType)();

    static int Thunk(lua_State *luaState) {
        FuncType funcPtr = (FuncType)lua_touserdata(luaState, lua_upvalueindex(1));

        funcPtr();
        return 0;
    }

    template <typename T1> struct Arg1 {
        typedef void (*FuncType)(T1);

        static int Thunk(lua_State *luaState) {
            FuncType funcPtr = (FuncType)lua_touserdata(luaState, lua_upvalueindex(1));

            funcPtr(Parameter<T1>::Translate(luaState, 1));
            return 0;
        }
    };
};

template <typename T1, typename T2> static int VoidFunctionArg2Thunk(lua_State *luaState) {
    typedef void (*FuncType)(T1, T2);

    FuncType funcPtr = (FuncType)lua_touserdata(luaState, lua_upvalueindex(1));

    funcPtr(Parameter<T1>::Translate(luaState, 1), Parameter<T2>::Translate(luaState, 2));
    return 0;
}

template <typename T1, typename T2, typename T3> static int VoidFunctionArg3Thunk(lua_State *luaState) {
    typedef void (*FuncType)(T1, T2, T3);

    FuncType funcPtr = (FuncType)lua_touserdata(luaState, lua_upvalueindex(1));

    funcPtr(Parameter<T1>::Translate(luaState, 1), Parameter<T2>::Translate(luaState, 2), Parameter<T3>::Translate(luaState, 3));
    return 0;
}

template <typename T1, typename T2, typename T3, typename T4> static int VoidFunctionArg4Thunk(lua_State *luaState) {
    typedef void (*FuncType)(T1, T2, T3, T4);

    FuncType funcPtr = (FuncType)lua_touserdata(luaState, lua_upvalueindex(1));

    funcPtr(Parameter<T1>::Translate(luaState, 1), Parameter<T2>::Translate(luaState, 2), Parameter<T3>::Translate(luaState, 3), Parameter<T4>::Translate(luaState, 4));
    return 0;
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
static int VoidFunctionArg6Thunk(lua_State *luaState) {
    typedef void (*FuncType)(T1, T2, T3, T4, T5, T6);

    FuncType funcPtr = (FuncType)lua_touserdata(luaState, lua_upvalueindex(1));

    funcPtr(Parameter<T1>::Translate(luaState, 1), Parameter<T2>::Translate(luaState, 2), Parameter<T3>::Translate(luaState, 3), Parameter<T4>::Translate(luaState, 4), Parameter<T5>::Translate(luaState, 5), Parameter<T6>::Translate(luaState, 6));
    return 0;
}

template <typename C> struct VoidMethodBinder {
    typedef void (C::*MethodPtrType)();

    static int ThunkMethod(lua_State *luaState) {
        MethodPtrType *methodPtrAddr = (MethodPtrType *)lua_touserdata(luaState, lua_upvalueindex(1));

        (Parameter<C *>::Translate(luaState, 1)->**methodPtrAddr)();
        return 0;
    }

    template <typename T1> struct Arg1 {
        typedef void (C::*MethodPtrType)(T1);

        static int ThunkMethod(lua_State *luaState) {
            MethodPtrType *methodPtrAddr = (MethodPtrType *)lua_touserdata(luaState, lua_upvalueindex(1));
            C *const objPtr = Parameter<C *>::Translate(luaState, 1);

            T1 arg1 = Parameter<T1>::Translate(luaState, 2);

            (objPtr->**methodPtrAddr)(arg1);
            return 0;
        }
    };
};

template <typename C, typename T1, typename T2> static int VoidMethodArg2Thunk(lua_State *luaState) {
    typedef void (C::*MethodPtrType)(T1, T2);

    MethodPtrType *methodPtrAddr = (MethodPtrType *)lua_touserdata(luaState, lua_upvalueindex(1));
    C *const objPtr = Parameter<C *>::Translate(luaState, 1);

    T1 arg1 = Parameter<T1>::Translate(luaState, 2);
    T2 arg2 = Parameter<T2>::Translate(luaState, 3);

    (objPtr->**methodPtrAddr)(arg1, arg2);
    return 0;
}

template <typename R, typename C> struct MethodBinder {
    typedef R (C::*MethodPtrType)() const;

    static int ThunkMethod(lua_State *luaState) {
        MethodPtrType *methodPtrAddr = (MethodPtrType *)lua_touserdata(luaState, lua_upvalueindex(1));
        C *objPtr = Parameter<C *>::Translate(luaState, 1);

        return ReturnValue<R>::Translate(luaState, (objPtr->**methodPtrAddr)());
    }
};

template <typename C> static void BindSingleton(lua_State *luaState, const char *variableName, C *singleton, const char *className) {
    lua_pushstring(luaState, variableName);
    *static_cast<C **>(lua_newuserdata(luaState, sizeof(C *))) = singleton;
    LuaBindery::LoadMetatable(luaState, className);
    lua_setmetatable(luaState, -2);
    lua_settable(luaState, LUA_GLOBALSINDEX);
}

static void BindRawFunction(lua_State *luaState, const char *funcName, int (*funcptr)(lua_State *), const char *tableName) {
    LuaBindery::GetGlobalTable(luaState, tableName);
    lua_pushstring(luaState, funcName);
    lua_pushcclosure(luaState, funcptr, 0);
    LuaBindery::SetInGlobalTable(luaState, tableName);
}

template <typename R> static void BindFunction(lua_State *luaState, const char *funcName, R (*funcptr)(), const char *tableName) {
    LuaBindery::GetGlobalTable(luaState, tableName);
    lua_pushstring(luaState, funcName);
    lua_pushlightuserdata(luaState, (void *)funcptr);
    lua_pushcclosure(luaState, Binder<R>::Thunk, 1);
    LuaBindery::SetInGlobalTable(luaState, tableName);
}

template <typename R, typename A1> static void BindFunction(lua_State *luaState, const char *funcName, R (*funcptr)(A1), const char *tableName) {
    LuaBindery::GetGlobalTable(luaState, tableName);
    lua_pushstring(luaState, funcName);
    lua_pushlightuserdata(luaState, (void *)funcptr);
    lua_pushcclosure(luaState, FunctionArg1Thunk<R, A1>, 1);
    LuaBindery::SetInGlobalTable(luaState, tableName);
}

template <typename R, typename A1, typename A2> static void BindFunction(lua_State *luaState, const char *funcName, R (*funcptr)(A1, A2), const char *tableName) {
    LuaBindery::GetGlobalTable(luaState, tableName);
    lua_pushstring(luaState, funcName);
    lua_pushlightuserdata(luaState, (void *)funcptr);
    lua_pushcclosure(luaState, FunctionArg2Thunk<R, A1, A2>, 1);
    LuaBindery::SetInGlobalTable(luaState, tableName);
}

static void BindVoidFunction(lua_State *luaState, const char *funcName, void (*funcptr)(), const char *tableName) {
    LuaBindery::GetGlobalTable(luaState, tableName);
    lua_pushstring(luaState, funcName);
    lua_pushlightuserdata(luaState, (void *)funcptr);
    lua_pushcclosure(luaState, VoidBinder::Thunk, 1);
    LuaBindery::SetInGlobalTable(luaState, tableName);
}

template <typename T1> static void BindVoidFunction(lua_State *luaState, const char *funcName, void (*funcptr)(T1), const char *tableName) {
    LuaBindery::GetGlobalTable(luaState, tableName);
    lua_pushstring(luaState, funcName);
    lua_pushlightuserdata(luaState, (void *)funcptr);
    lua_pushcclosure(luaState, VoidBinder::Arg1<T1>::Thunk, 1);
    LuaBindery::SetInGlobalTable(luaState, tableName);
}

template <typename T1, typename T2> static void BindVoidFunction(lua_State *luaState, const char *funcName, void (*funcptr)(T1, T2), const char *tableName) {
    LuaBindery::GetGlobalTable(luaState, tableName);
    lua_pushstring(luaState, funcName);
    lua_pushlightuserdata(luaState, (void *)funcptr);
    lua_pushcclosure(luaState, VoidFunctionArg2Thunk<T1, T2>, 1);
    LuaBindery::SetInGlobalTable(luaState, tableName);
}

template <typename T1, typename T2, typename T3> static void BindVoidFunction(lua_State *luaState, const char *funcName, void (*funcptr)(T1, T2, T3), const char *tableName) {
    LuaBindery::GetGlobalTable(luaState, tableName);
    lua_pushstring(luaState, funcName);
    lua_pushlightuserdata(luaState, (void *)funcptr);
    lua_pushcclosure(luaState, VoidFunctionArg3Thunk<T1, T2, T3>, 1);
    LuaBindery::SetInGlobalTable(luaState, tableName);
}

template <typename T1, typename T2, typename T3, typename T4> static void BindVoidFunction(lua_State *luaState, const char *funcName, void (*funcptr)(T1, T2, T3, T4), const char *tableName) {
    LuaBindery::GetGlobalTable(luaState, tableName);
    lua_pushstring(luaState, funcName);
    lua_pushlightuserdata(luaState, (void *)funcptr);
    lua_pushcclosure(luaState, VoidFunctionArg4Thunk<T1, T2, T3, T4>, 1);
    LuaBindery::SetInGlobalTable(luaState, tableName);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
static void BindVoidFunction(lua_State *luaState, const char *funcName, void (*funcptr)(T1, T2, T3, T4, T5, T6), const char *tableName) {
    LuaBindery::GetGlobalTable(luaState, tableName);
    lua_pushstring(luaState, funcName);
    lua_pushlightuserdata(luaState, (void *)funcptr);
    lua_pushcclosure(luaState, VoidFunctionArg6Thunk<T1, T2, T3, T4, T5, T6>, 1);
    LuaBindery::SetInGlobalTable(luaState, tableName);
}

template <typename C> static void BindVoidMethod(lua_State *luaState, const char *className, const char *methodName, void (C::*methodPtr)()) {
    typedef void (C::*MethodPtrType)();

    LuaBindery::LoadMetatable(luaState, className);
    lua_pushstring(luaState, methodName);
    MethodPtrType *methodUserData = static_cast<MethodPtrType *>(lua_newuserdata(luaState, sizeof(MethodPtrType)));

    *methodUserData = methodPtr;
    lua_pushcclosure(luaState, VoidMethodBinder<C>::ThunkMethod, 1);
    lua_settable(luaState, -3);
    lua_settop(luaState, -2);
}

template <typename C, typename T1> static void BindVoidMethod(lua_State *luaState, const char *className, const char *methodName, void (C::*methodPtr)(T1)) {
    typedef void (C::*MethodPtrType)(T1);

    LuaBindery::LoadMetatable(luaState, className);
    lua_pushstring(luaState, methodName);
    MethodPtrType *methodUserData = static_cast<MethodPtrType *>(lua_newuserdata(luaState, sizeof(MethodPtrType)));

    *methodUserData = methodPtr;
    lua_pushcclosure(luaState, VoidMethodBinder<C>::Arg1<T1>::ThunkMethod, 1);
    lua_settable(luaState, -3);
    lua_settop(luaState, -2);
}

template <typename C, typename T1, typename T2> static void BindVoidMethod(lua_State *luaState, const char *className, const char *methodName, void (C::*methodPtr)(T1, T2)) {
    typedef void (C::*MethodPtrType)(T1, T2);

    LuaBindery::LoadMetatable(luaState, className);
    lua_pushstring(luaState, methodName);
    MethodPtrType *methodUserData = static_cast<MethodPtrType *>(lua_newuserdata(luaState, sizeof(MethodPtrType)));

    *methodUserData = methodPtr;
    lua_pushcclosure(luaState, VoidMethodArg2Thunk<C, T1, T2>, 1);
    lua_settable(luaState, -3);
    lua_settop(luaState, -2);
}

template <typename R, typename C> static void BindMethod(lua_State *luaState, const char *className, const char *methodName, R (C::*methodPtr)() const) {
    typedef R (C::*MethodPtrType)() const;

    LuaBindery::LoadMetatable(luaState, className);
    lua_pushstring(luaState, methodName);
    MethodPtrType *methodUserData = static_cast<MethodPtrType *>(lua_newuserdata(luaState, sizeof(MethodPtrType)));

    *methodUserData = methodPtr;
    lua_pushcclosure(luaState, MethodBinder<R, C>::ThunkMethod, 1);
    lua_settable(luaState, -3);
    lua_settop(luaState, -2);
}

LuaBindery *LuaBindery::fObj = nullptr;

LuaBindery::LuaBindery() {}

LuaBindery::~LuaBindery() {}

void LuaBindery::Init() {
    fObj = new ("LuaBindery", 0) LuaBindery();
}

void LuaBindery::Shutdown() {
    if (fObj != NULL) {
        delete fObj;
    }
    fObj = NULL;
}

void LuaBindery::LoadMetatable(lua_State *luaState, const char *className) {
    if (luaL_newmetatable(luaState, className) != 0) {
        lua_pushstring(luaState, "__index");
        lua_pushvalue(luaState, -2);
        lua_settable(luaState, -3);
    }
}

void LuaBindery::AttachMetatable(lua_State *luaState, const char *className) {
    luaL_getmetatable(luaState, className);
    lua_setmetatable(luaState, -2);
}

void LuaBindery::GetGlobalTable(lua_State *luaState, const char *tableName) {
    if (tableName != NULL) {
        lua_pushstring(luaState, tableName);
        lua_gettable(luaState, LUA_GLOBALSINDEX);

        if (lua_type(luaState, -1) == LUA_TNIL) {
            lua_settop(luaState, -2);
            lua_newtable(luaState);
            lua_pushstring(luaState, tableName);
            lua_pushvalue(luaState, -2);
            lua_settable(luaState, LUA_GLOBALSINDEX);
        }
    }
}

void LuaBindery::SetInGlobalTable(lua_State *luaState, const char *tableName) {
    if (tableName != NULL) {
        lua_settable(luaState, -3);
        lua_settop(luaState, -2);
    } else {
        lua_settable(luaState, LUA_GLOBALSINDEX);
    }
}

static int Table_Randomize(lua_State *luaState) {
    if (lua_type(luaState, 1) == LUA_TTABLE) {
        int numElements = luaL_getn(luaState, 1);

        for (int onPass = 0; onPass < numElements; onPass++) {
            for (int onElem = 1; onElem < numElements; onElem++) {
                int swapWith = bRandom(numElements) + 1;

                if (swapWith != onElem) {
                    lua_rawgeti(luaState, 1, onElem);
                    lua_rawgeti(luaState, 1, swapWith);
                    lua_rawseti(luaState, 1, onElem);
                    lua_rawseti(luaState, 1, swapWith);
                }
            }
        }
    }
    return 0;
}

void LuaBindery::BindToGameCode(lua_State *luaState) {
    BindRawFunction(luaState, "Randomize", Table_Randomize, "Table");

    BindVoidFunction(luaState, "Run", Activity_Run, "Activity");
    BindVoidFunction(luaState, "Suspend", Activity_Suspend, "Activity");

    BindVoidFunction(luaState, "SetFlag", Audio_SetFlag, "Audio");
    BindFunction(luaState, "IsCopSpeechPlaying", Audio_IsCopSpeechPlaying, "Audio");

    BindVoidFunction(luaState, "AddEngagedRace", MiniMap_AddEngagedRace, "MiniMap");

    BindFunction(luaState, "GetNumChallengesPassed", Bin_GetNumChallengesPassed, "Bin");
    BindFunction(luaState, "GetNumRacesWon", Bin_GetNumRacesWon, "Bin");

    BindFunction(luaState, "RandomInt", Math_RandomInt, "Math");

    BindVoidFunction(luaState, "SetGenericCamera", Camera_SetGenericCamera, "Camera");

    BindVoidFunction(luaState, "Play", NIS_Play, "NIS");

    BindVoidFunction(luaState, "PlayHackE3FMV", Movie_PlayHackE3FMV, "Movie");

    BindVoidFunction(luaState, "Print", Debug_Print, "Debug");
    BindVoidFunction(luaState, "PrintInstance", Debug_PrintInstance, "Debug");
    BindVoidFunction(luaState, "ShowScreenMessage", Debug_ShowScreenMessage, "Debug");

    BindVoidFunction(luaState, "SetRaceCompleteForFE", Demo_SetRaceCompleteForFE, "Demo");
    BindVoidFunction(luaState, "StorePursuitRepForFE", Demo_StorePursuitRepForFE, "Demo");

    BindVoidFunction(luaState, "ShowMessage", HUD_ShowMessage, "HUD");
    BindVoidFunction(luaState, "ShowTimeExtension", HUD_ShowTimeExtension, "HUD");

    BindVoidFunction(luaState, "SabotageEngine", Game_SabotageEngine, "Game");
    BindVoidFunction(luaState, "BlowEngine", Game_BlowEngine, "Game");
    BindVoidFunction(luaState, "DetachCameraFromRacer", Game_DetachCameraFromRacer, "Game");
    BindVoidFunction(luaState, "ChallengeComplete", Game_ChallengeComplete, "Game");
    BindVoidFunction(luaState, "ClearAIControl", Game_ClearAIControl, "Game");
    BindVoidFunction(luaState, "ForceAIControl", Game_ForceAIControl, "Game");
    BindVoidFunction(luaState, "InitRacers", Game_InitRacers, "Game");
    BindVoidFunction(luaState, "WarpPlayerToTrigger", Game_WarpPlayerToTrigger, "Game");
    BindVoidFunction(luaState, "ResetTrigger", Game_ResetTrigger, "Game");
    BindVoidFunction(luaState, "SetPlayerStartPosition", Game_SetPlayerStartPosition, "Game");
    BindVoidFunction(luaState, "SetRacerGoal", Game_SetRacerGoal, "Game");
    BindVoidFunction(luaState, "SetRacerLapsLeft", Game_SetRacerLapsLeft, "Game");
    BindVoidFunction(luaState, "KnockoutRacer", Game_KnockoutRacer, "Game");
    BindVoidFunction(luaState, "TotalRacer", Game_KnockoutRacer, "Game");
    BindVoidFunction(luaState, "NotifyRacePlacement", Game_NotifyRacePlacement, "Game");
    BindVoidFunction(luaState, "NotifySpeedTrapTriggered", Game_NotifySpeedTrapTriggered, "Game");
    BindVoidFunction(luaState, "NotifyCheckpointReached", Game_NotifyCheckpointReached, "Game");
    BindVoidFunction(luaState, "NotifyLapFinished", Game_NotifyLapFinished, "Game");
    BindVoidFunction(luaState, "NotifyRaceFinished", Game_NotifyRaceFinished, "Game");
    BindVoidFunction(luaState, "SetRaceActivity", Game_SetRaceActivity, "Game");
    BindVoidFunction(luaState, "StartRace", Game_StartRace, "Game");
    BindVoidFunction(luaState, "StartRaceTimers", Game_StartRaceTimers, "Game");
    BindVoidFunction(luaState, "SaveStartPositions", Game_SaveStartPositions, "Game");
    BindVoidFunction(luaState, "RestoreStartPositions", Game_RestoreStartPositions, "Game");
    BindVoidFunction(luaState, "EnterPostRaceFlow", Game_EnterPostRaceFlow, "Game");
    BindFunction(luaState, "ShowOnlinePostRaceScreen", FE_ShowOnlinePostRaceScreen, "Game");
    BindVoidFunction(luaState, "SetCopsEnabled", Game_SetCopsEnabled, "Game");
    BindVoidFunction(luaState, "NoNewPursuitsOrCops", Game_NoNewPursuitsOrCops, "Game");
    BindVoidFunction(luaState, "ForcePursuitStart", Game_ForcePursuitStart, "Game");
    BindVoidFunction(luaState, "EnterEngagableTrigger", Game_EnterEngagableTrigger, "Game");
    BindVoidFunction(luaState, "ExitEngagableTrigger", Game_ExitEngagableTrigger, "Game");
    BindVoidFunction(luaState, "EnterGateZone", Game_EnterGateZone, "Game");
    BindVoidFunction(luaState, "ExitGateZone", Game_ExitGateZone, "Game");
    BindVoidFunction(luaState, "ShowRaceOverSummary", Game_ShowRaceOverSummary, "Game");
    BindVoidFunction(luaState, "HideRaceOverSummary", Game_HideRaceOverSummary, "Game");
    BindVoidFunction(luaState, "SetTrafficSpeed", Game_SetTrafficSpeed, "Game");
    BindVoidFunction(luaState, "SpawnCop", Game_SpawnCop, "Game");
    BindVoidFunction(luaState, "SpawnCharacter", Game_SpawnCharacter, "Game");
    BindVoidFunction(luaState, "UnspawnCharacter", Game_UnspawnCharacter, "Game");
    BindVoidFunction(luaState, "SendCharacterStimulus", Game_SendCharacterStimulus, "Game");
    BindVoidFunction(luaState, "JackKnife", Game_JackKnife, "Game");
    BindVoidFunction(luaState, "SetAllStaging", Game_SetAllStaging, "Game");
    BindVoidFunction(luaState, "ShowPauseMenu", Game_ShowPauseMenu, "Game");
    BindVoidFunction(luaState, "AwardCash", Game_AwardCash, "Game");
    BindVoidFunction(luaState, "AwardPoints", Game_AwardPoints, "Game");
    BindVoidFunction(luaState, "ChallengeCompleted", Game_ChallengeCompleted, "Game");
    BindVoidFunction(luaState, "UnlockRace", Game_UnlockRace, "Game");
    BindVoidFunction(luaState, "NotifyCountdownDone", Game_NotifyCountdownDone, "Game");
    BindVoidFunction(luaState, "ResetCopsForRestart", Game_ResetCopsForRestart, "Game");
    BindVoidFunction(luaState, "ShowTriggerIcon", Game_ShowTriggerIcon, "Game");
    BindVoidFunction(luaState, "HideTriggerIcon", Game_HideTriggerIcon, "Game");
    BindVoidFunction(luaState, "WarpToMarkerWhenRoaming", Game_WarpToMarkerWhenRoaming, "Game");
    BindVoidFunction(luaState, "DoSpecialFinalization", Game_DoSpecialFinalization, "Game");
    BindFunction(luaState, "IsActiveSpeedTrap", Game_IsActiveSpeedTrap, "Game");
    BindFunction(luaState, "IsActiveMenuGate", Game_IsActiveMenuGate, "Game");
    BindFunction(luaState, "GetSimTime", Game_GetSimTime, "Game");
    BindFunction(luaState, "GetRacerIndex", Game_GetRacerIndex, "Game");
    BindFunction(luaState, "GetRacerElement", Game_GetRacerElement, "Game");
    BindFunction(luaState, "GetPlayerElement", Game_GetPlayerElement, "Game");
    BindFunction(luaState, "GetRacerCharacter", Game_GetRacerCharacter, "Game");
    BindFunction(luaState, "GetNumRacers", Game_GetNumRacers, "Game");
    BindFunction(luaState, "GetSimableSpeedKmh", Game_GetSimableSpeedKmh, "Game");
    BindFunction(luaState, "RacerIsHuman", Game_RacerIsHuman, "Game");
    BindFunction(luaState, "PlayerIsLocal", Game_PlayerIsLocal, "Game");
    BindFunction(luaState, "IsRaceCompleted", Game_IsRaceCompleted, "Game");
    BindFunction(luaState, "IsRaceUnlocked", Game_IsRaceUnlocked, "Game");
    BindFunction(luaState, "IsOnlineGame", Game_IsOnlineGame, "Game");
    BindFunction(luaState, "IsLANGame", Game_IsLANGame, "Game");
    BindFunction(luaState, "AllRacersDone", Game_AllRacersDone, "Game");
    BindFunction(luaState, "AllHumanPlayersDone", Game_AllHumanPlayersDone, "Game");
    BindFunction(luaState, "SimableDistance", Game_SimableDistance, "Game");
    BindFunction(luaState, "SimableAngle", Game_SimableAngle, "Game");
    BindFunction(luaState, "AllowEngageEvents", Game_AllowEngageEvents, "Game");
    BindFunction(luaState, "AllowMenuGates", Game_AllowMenuGates, "Game");
    BindFunction(luaState, "AllowEngageSafehouse", Game_AllowEngageSafehouse, "Game");
    BindFunction(luaState, "DoSpecialSetup", Game_DoSpecialSetup, "Game");

    BindFunction(luaState, "IsNextGen", Platform_IsNextGen, "Platform");

    BindSingleton(luaState, "RaceStatus", &GRaceStatus::Get(), "GRaceStatus");

    BindVoidMethod(luaState, "GRaceStatus", "ClearRacers", &GRaceStatus::ClearRacers);
    BindVoidMethod(luaState, "GRaceStatus", "AddRacer", &GRaceStatus::AddRacer);
    BindVoidMethod(luaState, "GRaceStatus", "AddAvailableEventToMap", &GRaceStatus::AddAvailableEventToMap);
    BindVoidMethod(luaState, "GRaceStatus", "AddSpeedTrapToMap", &GRaceStatus::AddSpeedTrapToMap);
    BindVoidMethod(luaState, "GRaceStatus", "ClearCheckpoints", &GRaceStatus::ClearCheckpoints);
    BindVoidMethod(luaState, "GRaceStatus", "AddCheckpoint", &GRaceStatus::AddCheckpoint);
    BindVoidMethod(luaState, "GRaceStatus", "SetNextCheckpointPos", &GRaceStatus::SetNextCheckpointPos);
    BindVoidMethod(luaState, "GRaceStatus", "SetRoaming", &GRaceStatus::SetRoaming);
    BindVoidMethod(luaState, "GRaceStatus", "SetIsLoading", &GRaceStatus::SetIsLoading);
    BindVoidMethod(luaState, "GRaceStatus", "EnterSuddenDeath", &GRaceStatus::EnterSuddenDeath);
    BindVoidMethod(luaState, "GRaceStatus", "AwardBonusTime", &GRaceStatus::AwardBonusTime);
    BindVoidMethod(luaState, "GRaceStatus", "SetActivelyRacing", &GRaceStatus::SetActivelyRacing);
    BindVoidMethod(luaState, "GRaceStatus", "SetTaskTime", &GRaceStatus::SetTaskTime);
    BindVoidMethod(luaState, "GRaceStatus", "DisableBarriers", &GRaceStatus::DisableBarriers);
    BindVoidMethod(luaState, "GRaceStatus", "EnableBinBarriers", &GRaceStatus::EnableBinBarriers);
    BindVoidMethod(luaState, "GRaceStatus", "NotifyScriptWhenLoaded", &GRaceStatus::NotifyScriptWhenLoaded);
    BindVoidMethod(luaState, "GRaceStatus", "SetHasBeenWon", &GRaceStatus::SetHasBeenWon);

    BindFunction(luaState, "GetPlayerBounty", Game_GetPlayerBounty, "Game");
    BindVoidFunction(luaState, "AwardPlayerBounty", Game_AwardPlayerBounty, "Game");
    BindVoidFunction(luaState, "JumpToCarLot", Game_JumpToCarLot, "Game");
    BindVoidFunction(luaState, "AbandonRace", Game_AbandonRace, "Game");
    BindVoidFunction(luaState, "JumpToSafeHouse", Game_JumpToSafeHouse, "Game");

    BindMethod(luaState, "ISimable", "IsPlayer", &ISimable::IsPlayer);

    BindVoidFunction(luaState, "SetTimer", Game_SetTimer, "Game");
    BindVoidFunction(luaState, "KillTimer", Game_KillTimer, "Game");
    BindVoidFunction(luaState, "ShowGPS", Game_ShowGPS, "Game");
    BindVoidFunction(luaState, "NavigatePlayerTo", Game_NavigatePlayerTo, "Game");

    BindVoidFunction(luaState, "Assert", Debug_Assert, "Debug");

    BindVoidFunction(luaState, "NotifyActivityFinsihed", Game_NotifyFinished, "Game");

    BindFunction(luaState, "SkipCareerIntro", Game_SkipCareerIntro, "Game");
    BindVoidFunction(luaState, "SetChanceOfRain", Game_SetChanceOfRain, "Game");
    BindVoidFunction(luaState, "Fade", Game_DoFade, "Game");
    BindVoidFunction(luaState, "IntroduceRival", Game_IntroduceRival, "Game");
    BindVoidFunction(luaState, "PlayTutorial", Game_PlayTutorial, "Game");
    BindVoidFunction(luaState, "DoSafeHouseIntro", Game_DoSafeHouseIntro, "Game");
    BindVoidFunction(luaState, "DoZoneMenuAction", Game_DoZoneMenuAction, "Game");
    BindFunction(luaState, "SetCurrentTimeOfDay", Game_SetTimeOfDay, "Game");
    BindVoidFunction(luaState, "ReloadWorld", Game_ReloadWorld, "Game");
    BindFunction(luaState, "IsCareerMode", Game_IsCareerMode, "Game");
    BindFunction(luaState, "IsSplitScreen", Game_IsSplitScreen, "Game");
    BindVoidFunction(luaState, "SetHasRapSheet", Game_SetHasRapSheet, "Game");
    BindVoidFunction(luaState, "SetWorldHeat", Game_SetWorldHeat, "Game");
    BindVoidFunction(luaState, "ShowWinningPostRaceScreen", FE_ShowWinningPostRaceScreen, "Game");
    BindVoidFunction(luaState, "ShowLosingPostRaceScreen", FE_ShowLosingPostRaceScreen, "Game");
    BindVoidFunction(luaState, "PreventPlayerBeingBusted", Game_PreventPlayerBeingBusted, "Game");
    BindFunction(luaState, "CalculateRanking", Game_CalculateRanking, "Game");
}
