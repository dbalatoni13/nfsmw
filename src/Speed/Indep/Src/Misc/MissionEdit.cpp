#include "Speed/Indep/Src/Misc/MissionEdit.hpp"
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"


// Mission component debug drawing --------------------------------------

void DrawMissionComponents() {}

// AI entity lifetime hooks ---------------------------------------------

void AIEntityCreated(unsigned int entity) {}

void AIEntityDestroyed(unsigned int entity) {}

// Lua output hooks ------------------------------------------------------

static bool sLuaOutputEnabled = false;

bool IsLuaOutputEnabled() {
    return sLuaOutputEnabled;
}

void SendLuaOutput(char *key, char *value) {}

void SendLuaOutput(char *value) {}

// Tool instant messaging hooks -----------------------------------------

void SendToolIMMessage(char *messageID, char *channel, char *key, char *value) {}

void SendToolIMMessage(char *messageID, char *channel, UTL::Std::list<char *, _type_list> &keys,
                       UTL::Std::list<char *, _type_list> &values) {}
