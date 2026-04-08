#include "Speed/Indep/Src/EAXSound/Data/SND_DopplerParams.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"

// TODO: Dwarf/PS2 do not currently surface better member names for this local parameter carrier.
struct DopplerParam {
    float unk0;
    int unk4;
};

struct DopplerParamTable {
    DopplerParam entries[3];

    DopplerParamTable();
};

DopplerParamTable g_DOPPLER_PARAMS;
extern stREVERB_PARAMS g_REVERBFXMODULES[];

DopplerParamTable::DopplerParamTable() {
    entries[0].unk0 = 0.35f;
    entries[0].unk4 = 1;
    entries[1].unk0 = 0.3f;
    entries[1].unk4 = 1;
    entries[2].unk0 = 0.3f;
    entries[2].unk4 = -300;

    g_REVERBFXMODULES[0].GinsuWet = -300;
    g_REVERBFXMODULES[0].GinsuDry = 0;
    g_REVERBFXMODULES[0].AemsWet = -300;
    g_REVERBFXMODULES[0].AemsDry = 0;
    g_REVERBFXMODULES[0].FadeOut = 120;
    g_REVERBFXMODULES[0].FadeIn = 120;

    g_REVERBFXMODULES[1].GinsuWet = -300;
    g_REVERBFXMODULES[1].GinsuDry = 0;
    g_REVERBFXMODULES[1].AemsWet = -300;
    g_REVERBFXMODULES[1].AemsDry = 0;
    g_REVERBFXMODULES[1].FadeOut = 120;
    g_REVERBFXMODULES[1].FadeIn = 120;

    g_REVERBFXMODULES[2].GinsuWet = -300;
    g_REVERBFXMODULES[2].GinsuDry = 0;
    g_REVERBFXMODULES[2].AemsWet = -300;
    g_REVERBFXMODULES[2].AemsDry = 0;
    g_REVERBFXMODULES[2].FadeOut = 120;
    g_REVERBFXMODULES[2].FadeIn = 120;

    g_REVERBFXMODULES[3].GinsuWet = -300;
    g_REVERBFXMODULES[3].GinsuDry = 0;
    g_REVERBFXMODULES[3].AemsWet = -300;
    g_REVERBFXMODULES[3].AemsDry = 0;
    g_REVERBFXMODULES[3].FadeOut = 120;
    g_REVERBFXMODULES[3].FadeIn = 120;

    g_REVERBFXMODULES[4].GinsuWet = -300;
    g_REVERBFXMODULES[4].GinsuDry = 0;
    g_REVERBFXMODULES[4].AemsWet = -300;
    g_REVERBFXMODULES[4].AemsDry = 0;
    g_REVERBFXMODULES[4].FadeOut = 120;
    g_REVERBFXMODULES[4].FadeIn = 120;

    g_REVERBFXMODULES[5].GinsuWet = -100;
    g_REVERBFXMODULES[5].GinsuDry = 0;
    g_REVERBFXMODULES[5].AemsWet = -300;
    g_REVERBFXMODULES[5].AemsDry = 0;
    g_REVERBFXMODULES[5].FadeOut = 300;
    g_REVERBFXMODULES[5].FadeIn = 120;

    g_REVERBFXMODULES[6].GinsuWet = -300;
    g_REVERBFXMODULES[6].GinsuDry = 0;
    g_REVERBFXMODULES[6].AemsWet = -300;
    g_REVERBFXMODULES[6].AemsDry = 0;
    g_REVERBFXMODULES[6].FadeOut = 120;
    g_REVERBFXMODULES[6].FadeIn = 120;

    g_REVERBFXMODULES[7].GinsuWet = -1200;
    g_REVERBFXMODULES[7].GinsuDry = 0;
    g_REVERBFXMODULES[7].AemsWet = -1200;
    g_REVERBFXMODULES[7].AemsDry = 0;
    g_REVERBFXMODULES[7].FadeOut = 120;
    g_REVERBFXMODULES[7].FadeIn = 120;

    g_REVERBFXMODULES[8].GinsuWet = -1200;
    g_REVERBFXMODULES[8].GinsuDry = 0;
    g_REVERBFXMODULES[8].AemsWet = -1200;
    g_REVERBFXMODULES[8].AemsDry = 0;
    g_REVERBFXMODULES[8].FadeOut = 120;
    g_REVERBFXMODULES[8].FadeIn = 120;

    g_REVERBFXMODULES[9].GinsuWet = -1200;
    g_REVERBFXMODULES[9].GinsuDry = 0;
    g_REVERBFXMODULES[9].AemsWet = -1200;
    g_REVERBFXMODULES[9].AemsDry = 0;
    g_REVERBFXMODULES[9].FadeOut = 120;
    g_REVERBFXMODULES[9].FadeIn = 120;

    g_REVERBFXMODULES[10].GinsuWet = -1200;
    g_REVERBFXMODULES[10].GinsuDry = 0;
    g_REVERBFXMODULES[10].AemsWet = -1200;
    g_REVERBFXMODULES[10].AemsDry = 0;
    g_REVERBFXMODULES[10].FadeOut = 120;
    g_REVERBFXMODULES[10].FadeIn = 120;

    g_REVERBFXMODULES[11].GinsuWet = -300;
    g_REVERBFXMODULES[11].GinsuDry = 0;
    g_REVERBFXMODULES[11].AemsWet = -300;
    g_REVERBFXMODULES[11].AemsDry = 0;
    g_REVERBFXMODULES[11].FadeOut = 120;
    g_REVERBFXMODULES[11].FadeIn = 120;
}
