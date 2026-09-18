#ifndef _DOLPHIN_MTX_SHIM_H_
#define _DOLPHIN_MTX_SHIM_H_
// Shim portable de la API de matrices del SDK Dolphin para el port Android.
// Mtx/Mtx44 son float[3][4] / float[4][4]; el juego las usa via bWare.
typedef float Mtx[3][4];
typedef float Mtx44[4][4];
typedef float Quaternion[4];
typedef struct { float x, y, z; } Vec;
#endif
