#ifndef LOGITECH_FORCE_H
#define LOGITECH_FORCE_H

// Las cuatro derivadas no anaden miembros ni metodos virtuales (el DWARF las da
// como "struct X : public Force {}" y sus constructores solo llaman al de Force),
// asi que viven aqui junto a la base en vez de en cuatro cabeceras de seis lineas.

// total size: 0x100
struct Force {
    Force();

    void InitVars();
    long Start(long channel, long effect);
    long Stop(long channel, long effect);
    long Destroy(long channel, long effect);

    // Los dos arrays son de 4 bytes por elemento e indexados [canal][efecto]: el
    // original calcula el desplazamiento como canal*32 + efecto*4 y usa stwx.
    // El volcado DWARF los anota como bool[8][4], que no cuadra con esas tiendas.
    unsigned long Playing[4][8];  // offset 0x0, size 0x80
    unsigned long EffectID[4][8]; // offset 0x80, size 0x80
};

// total size: 0x100
struct Condition : public Force {
    Condition();

    long DownloadForce(long channel, long forceNumber, unsigned long &handle, unsigned char type, unsigned long duration, unsigned long startDelay,
                       signed char offset, unsigned char deadband, unsigned char satNeg, unsigned char satPos, short coeffNeg, short coeffPos);
    long UpdateForce(long channel, long forceNumber, unsigned char type, unsigned long duration, unsigned long startDelay, signed char offset,
                     unsigned char deadband, unsigned char satNeg, unsigned char satPos, short coeffNeg, short coeffPos);
};

// total size: 0x100
struct Constant : public Force {
    Constant();

    long DownloadForce(long channel, long forceNumber, unsigned long &handle, unsigned long duration, unsigned long startDelay, short magnitude,
                       unsigned short direction, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel,
                       unsigned char fadeLevel);
    long UpdateForce(long channel, long forceNumber, unsigned long duration, unsigned long startDelay, short magnitude, unsigned short direction,
                     unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);
};

// total size: 0x100
struct Periodic : public Force {
    Periodic();

    long DownloadForce(long channel, long forceNumber, unsigned long &handle, unsigned char type, unsigned long duration, unsigned long startDelay,
                       unsigned char magnitude, unsigned short direction, unsigned short period, unsigned short phase, short offset,
                       unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);
    long UpdateForce(long channel, long forceNumber, unsigned char type, unsigned long duration, unsigned long startDelay, unsigned char magnitude,
                     unsigned short direction, unsigned short period, unsigned short phase, short offset, unsigned long attackTime,
                     unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);
};

// total size: 0x100
struct Ramp : public Force {
    Ramp();
};

#endif
