#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// El TU original solo aporta datos: ninguna de estas tablas se lee desde
// zCamera (solo aparecen en __static_initialization_and_destruction_0), asi que
// tTable<bVector3>::Blend nunca se instancia.

static bVector3 vCopViewPivot(1.0f, 0.0f, 2.0f); // .bss: 0x8045AE04

static bVector3 vCopViewDistanceFovBand[2] = {
    bVector3(1.5f, 12000.0f, 0.5f), //
    bVector3(1.2f, 6000.0f, 0.5f),  //
}; // .bss: 0x8045AE14

static tTable<bVector3> tCopViewDistanceFovBand(vCopViewDistanceFovBand, 2, 0.0f, 1.0f); // .bss: 0x8045AE34

static bVector3 vCopViewPoints[5] = {
    bVector3(-2.0f, -3.0f, 1.2f),  //
    bVector3(-2.0f, -2.5f, 1.3f),  //
    bVector3(-2.0f, 0.0f, 1.5f),   //
    bVector3(-2.0f, 2.5f, 1.3f),   //
    bVector3(-2.0f, 3.0f, 1.2f),   //
}; // .bss: 0x8045AE48

static bVector2 vCopViewDistanceFov[2] = {
    bVector2(2.0f, 12000.0f), //
    bVector2(6.0f, 6000.0f),  //
}; // .bss: 0x8045AE98

static tTable<bVector3> tCopViewPosition(vCopViewPoints, 5, 0.0f, 1.0f); // .bss: 0x8045AEA8

static tTable<bVector2> tCopViewDistanceFov(vCopViewDistanceFov, 2, 0.0f, 1.0f); // .bss: 0x8045AEBC
