#ifndef LOGITECH_LGWHEELS_H
#define LOGITECH_LGWHEELS_H

#include "Speed/GameCube/Src/Logitech/Force.h"
#include "Speed/GameCube/Src/Logitech/Wheels.h"

// Indice de efecto con el que StopForce, IsPlaying y los arrays de Force trabajan.
// Los valores salen del "li r5, N" de cada LGWheels::StopXxx; el 3 y el 4 son los
// dos de colision, que no tienen Stop propio. Son diez, y cuadra con la primera
// dimension de wasPlayingBeforeAirborne[10][4].
enum LGForceType {
    LGFORCE_SPRING = 0,
    LGFORCE_CONSTANT = 1,
    LGFORCE_DAMPER = 2,
    LGFORCE_SIDE_COLLISION = 3,
    LGFORCE_FRONTAL_COLLISION = 4,
    LGFORCE_DIRT_ROAD = 5,
    LGFORCE_BUMPY_ROAD = 6,
    LGFORCE_SLIPPERY_ROAD = 7,
    LGFORCE_SURFACE = 8,
    LGFORCE_CAR_AIRBORNE = 9,
};

// total size: 0x16E4
//
// Ojo con los bool: aqui ocupan 4 bytes, no 1. Los desplazamientos del volcado
// DWARF solo cuadran asi (damperWasPlaying[4] son 16 bytes, y de ahi que
// springWasPlaying caiga en 0x15BC), aunque el dump anote esos arrays como
// "size 0x4". Sin eso, los *Params se van 156 bytes mas alla de su sitio.
struct LGWheels {
    LGWheels();

    void InitVars(long channel);
    void ReadAll();
    void StopForce(long channel, long forceType);
    bool IsPlaying(long channel, long forceType);

    bool IsConnected(long channel);
    bool PedalsConnected(long channel);
    bool ButtonIsPressed(long channel, unsigned long buttonMask);

    void PlayAutoCalibAndSpringForce(long channel);
    void PlaySpringForce(long channel, signed char offset, unsigned char saturation, short coefficient);
    void PlayConstantForce(long channel, short magnitude, unsigned short direction);
    void PlayDamperForce(long channel, short coefficient);
    void PlayFrontalCollisionForce(long channel, unsigned char magnitude);
    void PlaySlipperyRoadEffect(long channel, short magnitude);
    void PlaySurfaceEffect(long channel, unsigned char type, unsigned char magnitude, unsigned short period);
    void PlayCarAirborne(long channel);
    void PlayDirtRoadEffect(long channel, unsigned char magnitude);
    void PlayBumpyRoadEffect(long channel, unsigned char magnitude);

    void StopSpringForce(long channel);
    void StopConstantForce(long channel);
    void StopDamperForce(long channel);
    void StopDirtRoadEffect(long channel);
    void StopBumpyRoadEffect(long channel);
    void StopSlipperyRoadEffect(long channel);
    void StopSurfaceEffect(long channel);
    void StopCarAirborne(long channel);

    bool SameSpringForceParams(long channel, signed char offset, unsigned char saturation, short coefficient);
    bool SameConstantForceParams(long channel, short magnitude, unsigned short direction);
    bool SameDamperForceParams(long channel, short coefficient);
    bool SameFrontalCollisionForceParams(long channel, short magnitude);
    bool SameDirtRoadEffectParams(long channel, short magnitude);
    bool SameBumpyRoadEffectParams(long channel, short magnitude);
    bool SameSlipperyRoadEffectParams(long channel, short magnitude);
    bool SameSurfaceEffectParams(long channel, unsigned char type, unsigned char magnitude, unsigned short period);

    LGPosition Position[4];              // offset 0x0, size 0x28
    short NonLinearWheel[256][4];        // offset 0x28, size 0x800
    Wheels wheels;                       // offset 0x828, size 0x880
    Force force;                         // offset 0x10A8, size 0x100
    Condition condition;                 // offset 0x11A8, size 0x100
    Constant constant;                   // offset 0x12A8, size 0x100
    Periodic periodic;                   // offset 0x13A8, size 0x100
    Ramp ramp;                           // offset 0x14A8, size 0x100
    unsigned char OverallGain;           // offset 0x15A8, size 0x1
    bool damperWasPlaying[4];            // offset 0x15AC, size 0x4
    bool springWasPlaying[4];            // offset 0x15BC, size 0x4
    bool wasPlayingBeforeAirborne[4][10]; // offset 0x15CC, indexado [canal][tipo]
    bool IsAirborne[4];                  // offset 0x166C, size 0x4

    // total size: 0x4
    struct {
        char offset;              // offset 0x0, size 0x1
        unsigned char saturation; // offset 0x1, size 0x1
        short coefficient;        // offset 0x2, size 0x2
    } SpringForceParams[4];       // offset 0x167C, size 0x10

    // total size: 0x4
    struct {
        short magnitude;          // offset 0x0, size 0x2
        unsigned short direction; // offset 0x2, size 0x2
    } ConstantForceParams[4];     // offset 0x168C, size 0x10

    // total size: 0x2
    struct {
        short coefficient; // offset 0x0, size 0x2
    } DamperForceParams[4];       // offset 0x169C, size 0x8

    // total size: 0x4
    struct {
        short magnitude;          // offset 0x0, size 0x2
        unsigned short direction; // offset 0x2, size 0x2
    } SideCollisionParams[4];     // offset 0x16A4, size 0x10

    // total size: 0x2
    struct {
        short magnitude; // offset 0x0, size 0x2
    } FrontalCollisionParams[4];  // offset 0x16B4, size 0x8

    // total size: 0x2
    struct {
        short magnitude; // offset 0x0, size 0x2
    } DirtRoadParams[4];          // offset 0x16BC, size 0x8

    // total size: 0x2
    struct {
        short magnitude; // offset 0x0, size 0x2
    } BumpyRoadParams[4];         // offset 0x16C4, size 0x8

    // total size: 0x2
    struct {
        short magnitude; // offset 0x0, size 0x2
    } SlipperyRoadParams[4];      // offset 0x16CC, size 0x8

    // total size: 0x4
    struct {
        unsigned char type;      // offset 0x0, size 0x1
        unsigned char magnitude; // offset 0x1, size 0x1
        unsigned short period;   // offset 0x2, size 0x2
    } SurfaceEffectParams[4];    // offset 0x16D4, size 0x10
};

#endif
