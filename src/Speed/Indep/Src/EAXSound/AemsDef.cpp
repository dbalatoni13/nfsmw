#include "Speed/Indep/bWare/Inc/Strings.hpp"

struct AemsClass1Entry {
    float unk0;
    int unk4;
    float unk8;
    char name[64];
};

struct AemsClass1Table {
    AemsClass1Entry entries[4];

    AemsClass1Table();
};

AemsClass1Table g_CLASS1;

AemsClass1Table::AemsClass1Table() {
    entries[0].unk0 = 42.0f;
    entries[0].unk4 = 0x20;
    entries[0].unk8 = 23.0f;
    bStrCpy(entries[0].name, "Test.gin3");

    entries[1].unk0 = 42.0f;
    entries[1].unk4 = 0x20;
    entries[1].unk8 = 23.0f;
    bStrCpy(entries[1].name, "Test.gin34");

    entries[2].unk0 = 42.0f;
    entries[2].unk4 = 0x20;
    entries[2].unk8 = 23.0f;
    bStrCpy(entries[2].name, "Test.gin");

    entries[3].unk0 = 42.0f;
    entries[3].unk4 = 0x20;
    entries[3].unk8 = 23.0f;
    bStrCpy(entries[3].name, "Test.gin");
}
