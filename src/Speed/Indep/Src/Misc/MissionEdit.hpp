#ifndef MISC_MISSIONEDIT_H
#define MISC_MISSIONEDIT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"

// Debug/editor hooks shared across the gameplay code. The zMission TU
// provides the implementations; every one of them was dead-stripped from
// the shipped GOWE69 build.

void DrawMissionComponents();

void AIEntityCreated(unsigned int entity);
void AIEntityDestroyed(unsigned int entity);

void SendLuaOutput(char *key, char *value);
void SendLuaOutput(char *value);
bool IsLuaOutputEnabled();

void SendToolIMMessage(char *messageID, char *channel, char *key, char *value);
void SendToolIMMessage(char *messageID, char *channel, UTL::Std::list<char *, _type_list> &keys,
                       UTL::Std::list<char *, _type_list> &values);

#endif
