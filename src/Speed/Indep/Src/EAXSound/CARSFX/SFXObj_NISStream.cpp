#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_NISStream.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

struct NISStringHashMapEntry {
    unsigned int Hash;
    unsigned int Flags;
    const char *Name;
};

NISStringHashMapEntry uNIS_STRINGHASHMAP[68];

#define INIT_NIS_ENTRY(index, flags, name) \
    uNIS_STRINGHASHMAP[index].Flags = flags; \
    uNIS_STRINGHASHMAP[index].Name = name; \
    uNIS_STRINGHASHMAP[index].Hash = bStringHash(name)

void GenerateNISAnimHashMap() {
    INIT_NIS_ENTRY(0x00, 0x40000, "GenericStart");
    INIT_NIS_ENTRY(0x01, 1, "IntroNis01");
    INIT_NIS_ENTRY(0x02, 2, "IntroNis02");
    INIT_NIS_ENTRY(0x03, 4, "IntroNis03");
    INIT_NIS_ENTRY(0x04, 8, "IntroNis04");
    INIT_NIS_ENTRY(0x05, 0x10, "IntroNis05");
    INIT_NIS_ENTRY(0x06, 0x20, "IntroNis06");
    INIT_NIS_ENTRY(0x07, 0x40, "IntroNis07");
    INIT_NIS_ENTRY(0x08, 0x80, "IntroNis08");
    INIT_NIS_ENTRY(0x09, 0x100, "IntroNis09");
    INIT_NIS_ENTRY(0x0A, 0x200, "IntroNis10");
    INIT_NIS_ENTRY(0x0B, 0x1000, "Flythrough");
    INIT_NIS_ENTRY(0x0C, 0x4000, "IntroNisDD");
    INIT_NIS_ENTRY(0x0D, 0x80000, "IntroNisDDEnd");
    INIT_NIS_ENTRY(0x0E, 0x20000, "IntroNisToll01");
    INIT_NIS_ENTRY(0x0F, 0x2000, "GarageEnter");
    INIT_NIS_ENTRY(0x10, 0x2000, "GarageExit");
    INIT_NIS_ENTRY(0x11, 0x8000, "OPMRivalIntro");
    INIT_NIS_ENTRY(0x12, 1, "IntroNisBL01");
    INIT_NIS_ENTRY(0x13, 2, "IntroNisBL02");
    INIT_NIS_ENTRY(0x14, 4, "IntroNisBL03");
    INIT_NIS_ENTRY(0x15, 8, "IntroNisBL04");
    INIT_NIS_ENTRY(0x16, 0x10, "IntroNisBL05");
    INIT_NIS_ENTRY(0x17, 0x20, "IntroNisBL06");
    INIT_NIS_ENTRY(0x18, 0x20, "IntroNisBL06End");
    INIT_NIS_ENTRY(0x19, 0x40, "IntroNisBL07");
    INIT_NIS_ENTRY(0x1A, 0x40, "IntroNisBL07End");
    INIT_NIS_ENTRY(0x1B, 0x80, "IntroNisBL08");
    INIT_NIS_ENTRY(0x1C, 0x100, "IntroNisBL09");
    INIT_NIS_ENTRY(0x1D, 0x200, "IntroNisBL10");
    INIT_NIS_ENTRY(0x1E, 0x400, "IntroNisBL11");
    INIT_NIS_ENTRY(0x1F, 0x800, "IntroNisBL12");
    INIT_NIS_ENTRY(0x20, 0x1000, "IntroNisBL13");
    INIT_NIS_ENTRY(0x21, 0x2000, "IntroNisBL14");
    INIT_NIS_ENTRY(0x22, 0x4000, "IntroNisBL15");
    INIT_NIS_ENTRY(0x23, 0x8000, "IntroNisBL16");
    INIT_NIS_ENTRY(0x24, 0x80, "ArrestF02");
    INIT_NIS_ENTRY(0x25, 0x100, "ArrestF06");
    INIT_NIS_ENTRY(0x26, 0x200, "ArrestF07");
    INIT_NIS_ENTRY(0x27, 0x400, "ArrestF14");
    INIT_NIS_ENTRY(0x28, 0x4000, "ArrestF18");
    INIT_NIS_ENTRY(0x29, 0x800, "ArrestF23");
    INIT_NIS_ENTRY(0x2A, 1, "ArrestM01");
    INIT_NIS_ENTRY(0x2B, 2, "ArrestM04");
    INIT_NIS_ENTRY(0x2C, 4, "ArrestM06");
    INIT_NIS_ENTRY(0x2D, 0x8000, "ArrestM07");
    INIT_NIS_ENTRY(0x2E, 8, "ArrestM14");
    INIT_NIS_ENTRY(0x2F, 0x10, "ArrestM16");
    INIT_NIS_ENTRY(0x30, 0x20, "ArrestM19");
    INIT_NIS_ENTRY(0x31, 0x40, "ArrestM23");
    INIT_NIS_ENTRY(0x32, 0x80, "ArrestF02b");
    INIT_NIS_ENTRY(0x33, 0x100, "ArrestF06b");
    INIT_NIS_ENTRY(0x34, 0x200, "ArrestF07b");
    INIT_NIS_ENTRY(0x35, 0x400, "ArrestF14b");
    INIT_NIS_ENTRY(0x36, 0x4000, "ArrestF18b");
    INIT_NIS_ENTRY(0x37, 0x800, "ArrestF23b");
    INIT_NIS_ENTRY(0x38, 1, "ArrestM01b");
    INIT_NIS_ENTRY(0x39, 2, "ArrestM04b");
    INIT_NIS_ENTRY(0x3A, 4, "ArrestM06b");
    INIT_NIS_ENTRY(0x3B, 0x8000, "ArrestM07b");
    INIT_NIS_ENTRY(0x3C, 8, "ArrestM14b");
    INIT_NIS_ENTRY(0x3D, 0x10, "ArrestM16b");
    INIT_NIS_ENTRY(0x3E, 0x20, "ArrestM19b");
    INIT_NIS_ENTRY(0x3F, 0x40, "ArrestM23b");
    INIT_NIS_ENTRY(0x40, 0x20000, "EndingNis01");
    INIT_NIS_ENTRY(0x41, 0x40000, "EndingNis02");
    INIT_NIS_ENTRY(0x42, 0x80000, "EndingNis03");
    INIT_NIS_ENTRY(0x43, 0x100000, "EndingNis04");
}

#undef INIT_NIS_ENTRY
