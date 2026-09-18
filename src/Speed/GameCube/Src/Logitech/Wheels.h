#ifndef LOGITECH_WHEELS_H
#define LOGITECH_WHEELS_H

// total size: 0xA
struct LGPosition {
    unsigned short button;      // offset 0x0, size 0x2
    unsigned char misc;         // offset 0x2, size 0x1
    char wheel;                 // offset 0x3, size 0x1
    unsigned char accelerator;  // offset 0x4, size 0x1
    unsigned char brake;        // offset 0x5, size 0x1
    char combined;              // offset 0x6, size 0x1
    unsigned char triggerLeft;  // offset 0x7, size 0x1
    unsigned char triggerRight; // offset 0x8, size 0x1
    char err;                   // offset 0x9, size 0x1
};

// total size: 0x880
struct Wheels {
    Wheels();
    ~Wheels() {}

    void InitLGDevLibrary();
    short ReadAll();
    int FirstConnectedPort();

    bool ButtonIsPressed(long channel, unsigned long buttonMask);
    bool ButtonTriggered(long channel, unsigned long buttonMask);
    bool ButtonReleased(long channel, unsigned long buttonMask);
    bool IsConnected(long channel);
    bool PedalsConnected(long channel);
    bool PowerConnected(long channel);

    LGPosition Position[4];         // offset 0x0, size 0x28
    short NonLinearWheel[256][4];   // offset 0x28, size 0x800
    unsigned long WheelHandles[4];  // offset 0x828, size 0x10
    unsigned long type[4];          // offset 0x838, size 0x10
    unsigned long WheelHandle[4];   // offset 0x848, size 0x10
    LGPosition PositionLast[4];     // offset 0x858, size 0x28
};

#endif
