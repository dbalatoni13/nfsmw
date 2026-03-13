#include "Speed/Indep/Src/FEng/FEScript.h"

void FEScript::Init() {
    *reinterpret_cast<FEKeyTrack**>(reinterpret_cast<char*>(this) + 0x20) = nullptr;
    *reinterpret_cast<char**>(reinterpret_cast<char*>(this) + 0x2C) = nullptr;
    *reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(this) + 0x30) = 0;
    *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x0C) = 0;
    *reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(this) + 0x14) = 0;
    *reinterpret_cast<FEScript**>(reinterpret_cast<char*>(this) + 0x18) = nullptr;
    *reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(this) + 0x1C) = 0;
}
