#include "Speed/Indep/Src/EAXSound/Data/SND_DopplerParams.hpp"

struct DopplerParam {
    float unk0;
    int unk4;
};

struct DopplerParamTable {
    DopplerParam entries[3];

    DopplerParamTable();
};

DopplerParamTable g_DOPPLER_PARAMS;

DopplerParamTable::DopplerParamTable() {
    entries[0].unk0 = 0.35f;
    entries[0].unk4 = 1;
    entries[1].unk0 = 0.3f;
    entries[1].unk4 = 1;
    entries[2].unk0 = 0.3f;
    entries[2].unk4 = -300;
}
