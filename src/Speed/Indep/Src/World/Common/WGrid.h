#ifndef WORLD_COMMON_WGRID_H
#define WORLD_COMMON_WGRID_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct UGroup;

struct WGrid {
    static void Init(const UGroup *mapGroup);
    static void Shutdown();
};

#endif
