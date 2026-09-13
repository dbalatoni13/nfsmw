#ifndef FETYPES_H_
#define FETYPES_H_

#ifdef EA_PLATFORM_GAMECUBE
typedef unsigned long u32;
typedef long i32;
typedef unsigned short u16;
typedef short i16;
typedef unsigned char u8;
#else
typedef unsigned int u32;   // Decl: 26
typedef int i32;            // Decl: 27
typedef unsigned short u16; // Decl: 28
typedef short i16;          // Decl: 29
typedef unsigned char u8;   // Decl: 30
#endif

#define FENG_BIG_ENDIAN // :32 // TODO: probably set conditionally by build platform

static const u32 FEngLibVersion = 78339; // size: 0x4, Decl: 67

#define FEMAX(a, b) ((a) > (b) ? (a) : (b))                    // :76
#define FEMIN(a, b) ((a) < (b) ? (a) : (b))                    // :77
#define FECLAMP(a) ((a) < 0 ? 0 : ((a) > 255 ? 255 : (u8)(a))) // :78

// total size: 0x10
// Decl: 85
class FEColor {
  public:
    i32 b, g, r, a; // offset 0x0, size 0x4, Decl: 88

    FEColor() {} // Decl: 90
    FEColor(u32 Col);

    operator u32() const;

    FEColor &operator=(const FEColor &rhs); // Decl: 94

    bool operator==(const FEColor &rhs) {} // Decl: 97

    FEColor operator+(const FEColor &rhs) const; // Decl: 99
    FEColor &operator+=(const FEColor &rhs);     // Decl: 100
    FEColor operator-(const FEColor &rhs) const; // Decl: 101
    FEColor &operator-=(const FEColor &rhs);     // Decl: 102

    FEColor Modulate(const FEColor &rhs) const; // Decl: 104
};

// total size: 0x8
// Decl: 108
class FEPoint {
  public:
    float h, v; // offset 0x0, size 0x4, Decl: 110

    FEPoint() : h(0.0f), v(0.0f) {}              // Decl: 112
    FEPoint(float Value) : h(Value), v(Value) {} // Decl: 113
    FEPoint(float H, float V) : h(H), v(V) {}    // Decl: 114

    FEPoint &operator=(const FEPoint &p) { // Decl: 116
        h = p.h;
        v = p.v;
        return *this;
    }
    FEPoint &operator-=(const FEPoint &p) { // Decl: 117
        h -= p.h;
        v -= p.v;
        return *this;
    }
    FEPoint &operator+=(const FEPoint &p) { // Decl: 118
        h += p.h;
        v += p.v;
        return *this;
    }
};

// total size: 0x10
// Decl: 122
class FERect {
  public:
    float left, top, right, bottom; // offset 0x0, size 0x4, Decl: 124

    FERect() : left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {}                       // Decl: 126
    FERect(float Value) : left(Value), top(Value), right(Value), bottom(Value) {}        // Decl: 127
    FERect(float l, float t, float r, float b) : left(l), top(t), right(r), bottom(b) {} // Decl: 128

    FERect &operator=(const FERect &r) { // Decl: 136
        left = r.left;
        top = r.top;
        right = r.right;
        bottom = r.bottom;
        return *this;
    }

    FERect operator+(const FEPoint &p) const {} // Decl: 138
    FERect &operator+=(const FEPoint &p) {}     // Decl: 139

    FERect operator-(const FEPoint &p) const {} // Decl: 141
    FERect &operator-=(const FEPoint &p) {}     // Decl: 142

    FERect operator*(const FEPoint &p) const {} // Decl: 144
    FERect &operator*=(const FEPoint &p) {}     // Decl: 145

    void operator()(float l, float t, float r, float b) { // Decl: 147
        left = l;
        top = t;
        right = r;
        bottom = b;
    }
};

// Decl: 152-154 // TODO: probably checks FENG_BIG_ENDIAN for byte swaps
inline u32 FEngGetu32(u32 Val) {
    return (Val >> 24) | (Val << 24) | ((Val & 0xFF00) << 8) | ((Val >> 8) & 0xFF00);
}

inline i32 FEngGeti32(i32 Val) {
    return (Val >> 24) | (Val << 24) | ((Val & 0xFF00) << 8) | ((Val >> 8) & 0xFF00);
}

inline u16 FEngGetu16(u16 Val) {
    return ((Val & 0xFF00) >> 8) | (Val << 8);
}

inline i16 FEngGeti16(i16 Val) {
    return ((Val & 0xFF00) >> 8) | (Val << 8);
}

inline float FEngGetf32(float &Val) {
    u32 Temp = FEngGetu32(reinterpret_cast<u32 &>(Val));
    return reinterpret_cast<float &>(Temp);
}

#endif
