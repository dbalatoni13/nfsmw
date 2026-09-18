#include "Speed/Indep/bWare/Inc/bMath.hpp"

// TU de solo datos: los tweaks de la camara Still. Sus tres globales viven en
// .bss entre gPhoto_CarPosBias (Showcase.cpp) y vIceAccelLagMin (ICEMover.cpp).

static bVector3 StillEyeTweak(0.0f, -80.0f, 0.0f); // .bss: 0x8045AF78
static bVector3 StillLookTweak(0.0f, 0.0f, 0.0f);  // .bss: 0x8045AF88
static bVector3 StillUpTweak(0.0f, 0.0f, 1.0f);    // .bss: 0x8045AF98
