//
//
//
//
//
//
#ifndef SND_REVERBFXPARAMS_HPP
#define SND_REVERBFXPARAMS_HPP

// total size: 0x18
// Decl: 52
struct stREVERB_PARAMS {
    stREVERB_PARAMS() {} // Decl: 53

    stREVERB_PARAMS(int _GinsuWet, int _GinsuDry, int _AemsWet, int _AemsDry, int _FadeOut, int _FadeIn)
        : GinsuWet(_GinsuWet), //
          GinsuDry(_GinsuDry), //
          AemsWet(_AemsWet),   //
          AemsDry(_AemsDry),   //
          FadeOut(_FadeOut),   //
          FadeIn(_FadeIn) {}   // Decl: 63

    int GinsuWet; // offset 0x0, size 0x4, Decl: 73
    int GinsuDry; // offset 0x4, size 0x4, Decl: 74
    int AemsWet;  // offset 0x8, size 0x4, Decl: 75
    int AemsDry;  // offset 0xC, size 0x4, Decl: 76
    int FadeOut;  // offset 0x10, size 0x4, Decl: 77
    int FadeIn;   // offset 0x14, size 0x4, Decl: 78
};

#endif
