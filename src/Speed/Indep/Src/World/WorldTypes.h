#ifndef WORLD_WORLDTYPES_H
#define WORLD_WORLDTYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

typedef unsigned int WUID;

inline bool WUID_IsValid(WUID id) {
    return id != 0;
}

#endif
