#include "Speed/GameCube/Src/Ecstasy/eViewPlat.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

// 0x1FF8 / sizeof(eViewPlatInfo) = 22 entradas, segun symbols.txt
eViewPlatInfo ViewPlatInfoTable[22];

// El original emite stw, no stb: es int aunque el DWARF lo anote como bool.
int ForceFERenderStates;

void eViewPlatInterface::FEBeginBatchRender(int numPolys) {
    ForceFERenderStates = 1;
}

void eViewPlatInterface::FEEndBatchRender() {
    ForceFERenderStates = 0;
}

eViewPlatInfo *eViewPlatInterface::GimmeMyViewPlatInfo(int view_id) {
    return &ViewPlatInfoTable[view_id];
}
