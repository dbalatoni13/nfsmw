#ifndef LOGITECH_LGDEV_H
#define LOGITECH_LGDEV_H

struct LGPosition;

// Un eje de un efecto de condicion. Condition::DownloadForce escribe uno en
// 0xC y acto seguido lo relee y lo copia a 0x14, o sea que el efecto tiene dos.
// total size: 0x8
struct LGConditionAxis {
    char offset;             // offset 0x0, size 0x1
    unsigned char deadband;  // offset 0x1, size 0x1
    unsigned char satNeg;    // offset 0x2, size 0x1
    unsigned char satPos;    // offset 0x3, size 0x1
    short coeffNeg;          // offset 0x4, size 0x2
    short coeffPos;          // offset 0x6, size 0x2
};

// Descriptor de efecto que las tres clases derivadas de Force rellenan en pila y
// pasan a LGDownloadForceEffect / LGUpdateForceEffect. Mide 0x24: los tres lo
// limpian con memset(&params, 0, 0x24) antes de escribir sus campos.
//
// De 0xC en adelante cada tipo de efecto usa un layout distinto, deducido de los
// desplazamientos de las tiendas de cada DownloadForce.
// total size: 0x24
struct LGForceParams {
    unsigned char type;       // offset 0x0, size 0x1
    unsigned long duration;   // offset 0x4, size 0x4
    unsigned long startDelay; // offset 0x8, size 0x4

    union {
        // offset 0xC
        struct {
            short magnitude;           // offset 0xC, size 0x2
            unsigned short direction;  // offset 0xE, size 0x2
            unsigned long attackTime;  // offset 0x10, size 0x4
            unsigned long fadeTime;    // offset 0x14, size 0x4
            unsigned char attackLevel; // offset 0x18, size 0x1
            unsigned char fadeLevel;   // offset 0x19, size 0x1
        } constant;

        // offset 0xC y 0x14
        LGConditionAxis condition[2];

        // offset 0xC
        struct {
            unsigned char magnitude;   // offset 0xC, size 0x1
            unsigned short direction;  // offset 0xE, size 0x2
            unsigned short period;     // offset 0x10, size 0x2
            unsigned short phase;      // offset 0x12, size 0x2
            short offset;              // offset 0x14, size 0x2
            unsigned long attackTime;  // offset 0x18, size 0x4
            unsigned long fadeTime;    // offset 0x1C, size 0x4
            unsigned char attackLevel; // offset 0x20, size 0x1
            unsigned char fadeLevel;   // offset 0x21, size 0x1
        } periodic;
    };
};

// Los simbolos estan sin decorar en el binario original (LGOpen = 0x8031FDDC,
// LGStartForceEffect = 0x80320240...), asi que la libreria del volante es C.
#ifdef __cplusplus
extern "C" {
#endif

long LGInit();
long LGOpen(long channel, unsigned long *handle);
long LGClose(unsigned long handle);
long LGRead(void *wheels);

long LGDownloadForceEffect(unsigned long handle, unsigned long *effectID, struct LGForceParams *params);
long LGUpdateForceEffect(unsigned long effectID, struct LGForceParams *params);
long LGStartForceEffect(unsigned long effectID);
long LGStopForceEffect(unsigned long effectID);
long LGDestroyForceEffect(unsigned long effectID);

#ifdef __cplusplus
}
#endif

#endif
