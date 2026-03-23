#ifndef BWARE_ESPRESSO_H
#define BWARE_ESPRESSO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

inline void espEmptyLayer(const char *layername) {}

inline int espGetLayerState(const char *layername) {
    return 0;
}

#endif
