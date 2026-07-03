#ifndef FETYPES_H_
#define FETYPES_H_

#ifdef EA_PLATFORM_GAMECUBE
typedef unsigned long u32;
typedef long i32;
typedef unsigned short u16;
typedef short i16;
typedef unsigned char u8;
#else
typedef unsigned int u32;   // Decl: speed/indep/src/feng/FETypes.h:26
typedef int i32;            // Decl: speed/indep/src/feng/FETypes.h:27
typedef unsigned short u16; // Decl: speed/indep/src/feng/FETypes.h:28
typedef short i16;          // Decl: speed/indep/src/feng/FETypes.h:29
typedef unsigned char u8;   // Decl: speed/indep/src/feng/FETypes.h:30
#endif

#define FENG_BIG_ENDIAN // :32 // TODO: probably set conditionally by build platform

static const u32 FEngLibVersion = 78339; // size: 0x4, Decl: speed/indep/src/feng/FETypes.h:67

#define FEMAX(a, b) ((a) > (b) ? (a) : (b))                    // :76
#define FEMIN(a, b) ((a) < (b) ? (a) : (b))                    // :77
#define FECLAMP(a) ((a) < 0 ? 0 : ((a) > 255 ? 255 : (u8)(a))) // :78

// File: speed/indep/src/feng/FETypes.h
// total size: 0x10
// Decl: speed/indep/src/feng/FETypes.h:85
class FEColor {
  public:
    i32 b, g, r, a; // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FETypes.h:88

    FEColor() {} // Decl: speed/indep/src/feng/FETypes.h:90
    FEColor(u32 Col);

    operator u32() const;

    FEColor &operator=(const FEColor &rhs); // Decl: speed/indep/src/feng/FETypes.h:94

    bool operator==(const FEColor &rhs) {} // Decl: speed/indep/src/feng/FETypes.h:97

    FEColor operator+(const FEColor &rhs) const; // Decl: speed/indep/src/feng/FETypes.h:99
    FEColor &operator+=(const FEColor &rhs);     // Decl: speed/indep/src/feng/FETypes.h:100
    FEColor operator-(const FEColor &rhs) const; // Decl: speed/indep/src/feng/FETypes.h:101
    FEColor &operator-=(const FEColor &rhs);     // Decl: speed/indep/src/feng/FETypes.h:102

    FEColor Modulate(const FEColor &rhs) const; // Decl: speed/indep/src/feng/FETypes.h:104
};

// total size: 0x8
// Decl: speed/indep/src/feng/FETypes.h:108
class FEPoint {
  public:
    float h, v; // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FETypes.h:110

    FEPoint() : h(0.0f), v(0.0f) {}              // Decl: speed/indep/src/feng/FETypes.h:112
    FEPoint(float Value) : h(Value), v(Value) {} // Decl: speed/indep/src/feng/FETypes.h:113
    FEPoint(float H, float V) : h(H), v(V) {}    // Decl: speed/indep/src/feng/FETypes.h:114

    FEPoint &operator=(const FEPoint &p) { // Decl: speed/indep/src/feng/FETypes.h:116
        h = p.h;
        v = p.v;
        return *this;
    }
    FEPoint &operator-=(const FEPoint &p) { // Decl: speed/indep/src/feng/FETypes.h:117
        h -= p.h;
        v -= p.v;
        return *this;
    }
    FEPoint &operator+=(const FEPoint &p) { // Decl: speed/indep/src/feng/FETypes.h:118
        h += p.h;
        v += p.v;
        return *this;
    }
};

// total size: 0x10
// Decl: speed/indep/src/feng/FETypes.h:122
class FERect {
  public:
    float left, top, right, bottom; // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FETypes.h:124

    FERect() : left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {}                       // Decl: speed/indep/src/feng/FETypes.h:126
    FERect(float Value) : left(Value), top(Value), right(Value), bottom(Value) {}        // Decl: speed/indep/src/feng/FETypes.h:127
    FERect(float l, float t, float r, float b) : left(l), top(t), right(r), bottom(b) {} // Decl: speed/indep/src/feng/FETypes.h:128

    FERect &operator=(const FERect &r) { // Decl: speed/indep/src/feng/FETypes.h:136
        left = r.left;
        top = r.top;
        right = r.right;
        bottom = r.bottom;
        return *this;
    }

    FERect operator+(const FEPoint &p) const {} // Decl: speed/indep/src/feng/FETypes.h:138
    FERect &operator+=(const FEPoint &p) {}     // Decl: speed/indep/src/feng/FETypes.h:139

    FERect operator-(const FEPoint &p) const {} // Decl: speed/indep/src/feng/FETypes.h:141
    FERect &operator-=(const FEPoint &p) {}     // Decl: speed/indep/src/feng/FETypes.h:142

    FERect operator*(const FEPoint &p) const {} // Decl: speed/indep/src/feng/FETypes.h:144
    FERect &operator*=(const FEPoint &p) {}     // Decl: speed/indep/src/feng/FETypes.h:145

    void operator()(float l, float t, float r, float b) { // Decl: speed/indep/src/feng/FETypes.h:147
        left = l;
        top = t;
        right = r;
        bottom = b;
    }
};

// Decl: speed/indep/src/feng/FETypes.h:152-154 // TODO: probably checks FENG_BIG_ENDIAN for byte swaps
inline u32 FEngGetu32(u32 Val) {
    return (Val >> 24) | (Val << 24) | ((Val & 0xFF00) << 8) | ((Val >> 8) & 0xFF00);
}

inline i32 FEngGeti32(i32 Val) {
    return (Val >> 24) | (Val << 24) | ((Val & 0xFF00) << 8) | ((Val >> 8) & 0xFF00);
}

inline u16 FEngGetu16(u16 Val) {
    return static_cast<u16>((Val >> 8) | (Val << 8));
}

inline u16 FEngGeti16(i16 Val) {
    return static_cast<i16>((Val >> 8) | (Val << 8));
}

inline float FEngGetf32(float &Val) {
    u32 Temp = FEngGetu32(*reinterpret_cast<u32 *>(&Val));
    return *reinterpret_cast<float *>(&Temp);
}

#endif
