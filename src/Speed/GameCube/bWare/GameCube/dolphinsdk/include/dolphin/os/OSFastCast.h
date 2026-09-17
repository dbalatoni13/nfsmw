#ifndef _DOLPHIN_OSFASTCAST
#define _DOLPHIN_OSFASTCAST

#ifdef __cplusplus
extern "C" {
#endif

#define OS_GQR_F32 0x0000
#define OS_GQR_U8 0x0004
#define OS_GQR_U16 0x0005
#define OS_GQR_S8 0x0006
#define OS_GQR_S16 0x0007

#define OS_FASTCAST_U8 2
#define OS_FASTCAST_U16 3
#define OS_FASTCAST_S8 4
#define OS_FASTCAST_S16 5
// clang-format off
static inline void OSInitFastCast(void) {
#ifdef __MWERKS__
  asm
  {
        li      r3, OS_GQR_U8
        oris    r3, r3, OS_GQR_U8
        mtspr   GQR2, r3

        li      r3, OS_GQR_U16
        oris    r3, r3, OS_GQR_U16
        mtspr   GQR3, r3

        li      r3, OS_GQR_S8
        oris    r3, r3, OS_GQR_S8
        mtspr   GQR4, r3

        li      r3, OS_GQR_S16
        oris    r3, r3, OS_GQR_S16
        mtspr   GQR5, r3
  }
#elif defined(__GNUC__) || defined(__SN__)
    __asm__("li 3, 4\n"
            "oris 3, 3, 4\n"
            "mtspr 914, 3\n"
            "li 3, 5\n"
            "oris 3, 3, 5\n"
            "mtspr 915, 3\n"
            "li 3, 6\n"
            "oris 3, 3, 6\n"
            "mtspr 916, 3\n"
            "li 3, 7\n"
            "oris 3, 3, 7\n"
            "mtspr 917, 3" : : : "r3");
#endif
}
// clang-format off

#if defined(__GNUC__) || defined(__SN__)
static inline void OSSetGQR6(unsigned int type, unsigned int scale) {
    unsigned int val;

    val = type | (scale << 8);
    val |= val << 16;
    __asm__("mtspr 918, %0" : : "b"(val));
}
#endif

static inline s16 __OSf32tos16(register f32 inF)
{
	u32 tmp;
	register u32* tmpPtr = &tmp;
	register s16 out;
	// clang-format off
#ifdef MWERKS
    asm {
        psq_st inF, 0(tmpPtr), 0x1, OS_FASTCAST_S16
        lha out, 0(tmpPtr)
    }
#endif
  // clang-format on

  return out;
}

static inline void OSf32tos16(f32 *f, s16 *out) { *out = __OSf32tos16(*f); }

static inline u8 __OSf32tou8(register f32 inF) {
  u32 tmp;
  register u32 *tmpPtr = &tmp;
  register u8 out;
  // clang-format off
#ifdef MWERKS
    asm {
        psq_st inF, 0(tmpPtr), 0x1, OS_FASTCAST_U8
        lbz out, 0(tmpPtr)
    }
#endif
  // clang-format on

  return out;
}

static inline void OSf32tou8(f32 *f, u8 *out) { *out = __OSf32tou8(*f); }

static inline s8 __OSf32tos8(register f32 inF) {
  u32 tmp;
  register u32 *tmpPtr = &tmp;
  register s8 out;
  // clang-format off
#ifdef MWERKS
    asm {
        psq_st inF, 0(tmpPtr), 0x1, OS_FASTCAST_S8
        lbz out, 0(tmpPtr)
		extsb out, out //HACK to match fast cast inlines
    }
#endif
  // clang-format on

  return out;
}

static inline void OSf32tos8(f32 *f, s8 *out) { *out = __OSf32tos8(*f); }

static inline u16 __OSf32tou16(register f32 inF) {
  u32 tmp;
  register u32 *tmpPtr = &tmp;
  register u16 out;
  // clang-format off
#ifdef MWERKS
    asm {
        psq_st inF, 0(tmpPtr), 0x1, OS_FASTCAST_U16
        lbz out, 0(tmpPtr)
    }
#endif
  // clang-format on

  return out;
}

static inline void OSf32tou16(f32 *f, u16 *out) { *out = __OSf32tou16(*f); }

static inline float __OSs8tof32(register const s8 *arg) {
  register float ret;
#ifdef MWERKS
  asm {
        psq_l ret, 0(arg), 1, OS_FASTCAST_S8
  }
#endif
  return ret;
}

static inline void OSs8tof32(const s8 *in, float *out) {
  *out = __OSs8tof32(in);
}

static inline float __OSs16tof32(register const s16 *arg) {
  register float ret;
#ifdef MWERKS
  asm {
        psq_l ret, 0(arg), 1, OS_FASTCAST_S16
  }
#endif
  return ret;
}

static inline void OSs16tof32(const s16 *in, float *out) {
  *out = __OSs16tof32(in);
}

static inline float __OSu8tof32(register const u8 *arg) {
  register float ret;
#ifdef MWERKS
  asm {
        psq_l ret, 0(arg), 1, OS_FASTCAST_U8
  }
#endif
  return ret;
}

static inline void OSu8tof32(const u8 *in, float *out) {
  *out = __OSu8tof32(in);
}

static inline float __OSu16tof32(register const u16 *arg) {
  register float ret;
#ifdef MWERKS
  asm {
        psq_l ret, 0(arg), 1, OS_FASTCAST_U16
  }
#endif
  return ret;
}

static inline void OSu16tof32(const u16 *in, float *out) {
  *out = __OSu16tof32(in);
}

#ifdef __cplusplus
}
#endif

#endif // _DOLPHIN_OSFASTCAST
