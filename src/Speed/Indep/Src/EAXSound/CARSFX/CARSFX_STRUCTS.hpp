//
//
//
//
//
//
#ifndef CARSFX_STRUCTS_HPP
#define CARSFX_STRUCTS_HPP

// total size: 0x4C
// Decl: 11
#include "Speed/Indep/bWare/Inc/Strings.hpp"
struct sCLASS1 {
    sCLASS1() {} // Decl: 12

    // Decl: 20
    sCLASS1(float _param1, int _param2, float _param3, char *_param4)
        : param1(_param1), //
          param2(_param2), //
          param3(_param3) {
        bStrCpy(this->param4, _param4);
    }

    float param1;    // offset 0x0, size 0x4, Decl: 28
    int param2;      // offset 0x4, size 0x4, Decl: 29
    float param3;    // offset 0x8, size 0x4, Decl: 30
    char param4[64]; // offset 0xC, size 0x40, Decl: 31
};

#endif
