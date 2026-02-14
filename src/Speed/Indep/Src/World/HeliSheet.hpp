#ifndef WORLD_HELISHEET_H
#define WORLD_HELISHEET_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x38
class HeliSheetCoordinate {
  public:
    HeliSheetCoordinate() {
        this->PreviousElevation = 1000.0f;
        this->VertexValid = false;
    }

    void SetPreviousElevation(float elevation) {}

    // const bVector3 &GetVertex(unsigned int n) {}

  private:
    float PreviousElevation; // offset 0x0, size 0x4
    bool VertexValid;        // offset 0x4, size 0x1
    bVector3 Vertex[3];      // offset 0x8, size 0x30
};

#endif
