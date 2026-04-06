#ifndef WORLD_PARAMETERMAPS_H
#define WORLD_PARAMETERMAPS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"

struct ParameterMapLayer;

// total size: 0x1C
class ParameterAccessor : public bTNode<ParameterAccessor> {
  public:
    // TODO: Dwarf/PS2 show a larger virtual interface here; keep this owner-header
    // declaration source-compatible with zEAXSound's original call shape until the
    // broader ParameterMaps header can be restored without regressions.
    ParameterMapLayer *Layer;         // offset 0x8, size 0x4
    unsigned int AutoAttachLayerNamehash; // offset 0xC, size 0x4
    const char *DebugName;            // offset 0x10, size 0x4
    void *CurrentParameterData;       // offset 0x14, size 0x4

    bool IsValid() {
        return Layer != nullptr;
    }

    void CaptureData(float x, float y);
    int GetDataInt(int field_index);
};

#endif
