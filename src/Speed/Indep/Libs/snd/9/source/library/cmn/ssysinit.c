/* El orden del volcado final de `.lcomm` es el de la cadena del espacio de
   nombres, y esa la fija la PRIMERA declaracion de cada nombre, no la
   definicion.  `sndcmn.h` declara `gTotalOutputChannels` antes que
   `gSpeakerPositions`, y por eso nuestro `.bss` los sacaba al reves que el
   objetivo (canales en +1, altavoces en +4, contra +20 y +0).  Declarando los
   altavoces ANTES del include entran los primeros en la cadena.  */
namespace Snd {
extern float gSpeakerPositions[5];
};

#include "./sndcmn.h"

extern "C" {
void *memset(void *pdst, int val, unsigned int bytes);
}

// Declaraciones que en el original viven en cabeceras internas de la libreria
// que aqui no existen todavia. No se toca ninguna cabecera compartida.
enum StartMode {
    COLD = 0,
    WARM = 1,
};

extern char sndlibauthor;

int SNDPLATFORM_outputcaps();
int SNDPLATFORM_outputset();
int SNDPLATFORM_init(StartMode startMode);
int SNDPLATFORM_restore(StartMode startMode);

void SNDMEMI_init(void *pheap, int size);
int SNDMEMI_restore();
void SNDI_randomseed(unsigned int seed);
void SNDI_mutexalloc();
void SNDI_mutexfree();
void SNDI_spkrconfig();
void SNDI_precalcaztospkrvol();
void SNDI_freespkrtable();

int SNDSYS_getopts(SNDSYSOPTS *psso);
int SNDSYS_setopts(SNDSYSOPTS *psso);
int SNDSYSI_chanpubinit();

namespace Snd {

typedef struct VoicesNext {
    unsigned char value; // offset 0x0, size 0x1
    int changed;         // offset 0x4, size 0x4
} VoicesNext;

typedef struct OutputSampleRateNext {
    unsigned short value; // offset 0x0, size 0x2
    int changed;          // offset 0x4, size 0x4
} OutputSampleRateNext;

typedef struct OutputModeNext {
    OutputMode value; // offset 0x0, size 0x4
    int changed;      // offset 0x4, size 0x4
} OutputModeNext;

extern OutputMode outputmode;
extern VoicesNext gVoicesNext[4];
extern OutputSampleRateNext gOutputSampleRateNext[4];
extern OutputModeNext gOutputModeNext;

float gMasterVol = 1.0f;
int gMaxFxBuses = 1;
float gSpeakerPositions[5];
unsigned char gTotalOutputChannels;
// Original DWARF DIE 04EEC275: FoldDownTarget (4 B) at 804BED80.
// The following original object belongs to lbmpeg, not to this enum.
FoldDownTarget gFoldDownTarget;

} // namespace Snd

static int gotcaps = 0;

int SNDSYS_getopts(SNDSYSOPTS *psso) {
    static int ret = 0;

    if (gotcaps == 0) {
        sndgs.sso.set.maxbanks = 16;
        sndgs.sso.set.sndheapthreshold = 90;
        sndgs.sso.set.stealequalpriorityvoices = 1;
        sndgs.sso.set.compatibility = 0;
        sndgs.sso.set.updateperiod = -1.0f;
        sndgs.prevset = sndgs.sso.set;
        ret = SNDPLATFORM_outputcaps();
        gotcaps = 1;
    }

    *psso = sndgs.sso;
    return ret;
}

int SNDSYS_setopts(SNDSYSOPTS *psso) {
    sndgs.sso.set = psso->set;
    sndgs.sso.vec = psso->vec;
    SNDPLATFORM_outputset();
    sndgs.prevset = sndgs.sso.set;
    return 0;
}

int SNDSYSI_init(void *psndheap, int heapsize, int headerversion) {
    int rc;
    StartMode startMode = COLD;

    sndlibauthor = 'S';
    if (sndgs.installed != 0) {
        return 0;
    }

    memset(psndheap, 0, heapsize);
    SNDMEMI_init(psndheap, heapsize);

    if (sndgs.voicestotal == 0) {
        rc = SNDSYS_getopts(&sndgs.sso);
        if (rc < 0) {
            return rc;
        }
        SNDSYS_setopts(&sndgs.sso);
    }

    SNDI_randomseed(sndgs.sso.set.randomseed);
    SNDI_mutexalloc();
    sndgs.installed = 1;

    SNDSYS_entercritical();
    SNDSYSI_chanpubinit();
    sndgs.banklist = (BANKLIST *)SNDMEMI_allocz(sndgs.sso.set.maxbanks * 12);
    SNDSYS_leavecritical();

    Snd::gMasterVol = 1.0f;
    sndgs.audiotick = 0;
    sndgs.numuserdataclients = 0;
    sndgs.numserverclients100hz = 0;
    sndgs.numserverclients = 0;

    rc = SNDPLATFORM_init(startMode);
    if (rc < 0) {
        SNDPLATFORM_restore(startMode);
        SNDI_mutexfree();
        sndgs.installed = 0;
        return rc;
    }

    Snd::gTotalOutputChannels = sndgs.sso.set.outputchannels + sndgs.sso.set.outputlfe;
    SNDI_spkrconfig();
    SNDI_precalcaztospkrvol();
    return 0;
}

int SNDSYS_restore() {
    int rc;
    StartMode startMode = COLD;

    if (!Snd::System::IsInited()) {
        return SNDERR_NOT_INITED;
    }

    if (sndgs.aemsrestore != NULL) {
        sndgs.aemsrestore();
    }
    if (sndgs.aemsstreamrestore != NULL) {
        sndgs.aemsstreamrestore();
    }
    if (sndgs.eventrestore != NULL) {
        sndgs.eventrestore();
    }
    if (sndgs.streamrestore != NULL) {
        sndgs.streamrestore();
    }
    if (sndgs.midirestore != NULL) {
        sndgs.midirestore();
    }

    SNDI_freespkrtable();

    if (sndgs.bankremove != NULL) {
        sndgs.bankremove(SND_ALL_BANKS);
    }

    SNDPLATFORM_restore(startMode);

    SNDSYS_entercritical();
    SNDMEMI_free(sndgs.chan);
    SNDMEMI_free(sndgs.banklist);
    SNDSYS_leavecritical();

    rc = SNDMEMI_restore();
    sndgs.installed = 0;
    SNDI_mutexfree();
    return rc;
}

int SNDSYSI_chanpubinit() {
    int i;
    int j;
    int chanAllocSize;
    char *ptemp;

    chanAllocSize = sndgs.voicestotal * sizeof(CHANPUB) + Snd::gMaxFxBuses * (sndgs.voicestotal << 2);
    sndgs.chan = (CHANPUB *)SNDMEMI_allocz(chanAllocSize);
    memset(sndgs.chan, 0, chanAllocSize);

    ptemp = (char *)&sndgs.chan[sndgs.voicestotal];
    for (i = 0; i < sndgs.voicestotal; i++) {
        sndgs.chan[i].pFxVolume = (FXVOLUME *)ptemp;
        for (j = 0; j < Snd::gMaxFxBuses; j++) {
            sndgs.chan[i].pFxVolume[j].fxLevel = 1.0f;
        }
        ptemp += Snd::gMaxFxBuses * 4;
    }

    return 0;
}

int SNDSYSI_updatesso(SNDSYSOPTS *pSndSysOpts) {
    int device;

    if (Snd::gOutputModeNext.changed == 1) {
        Snd::outputmode = Snd::gOutputModeNext.value;
        Snd::gOutputModeNext.changed = 0;
    }

    for (device = Snd::DEVICE_MAIN; device < Snd::DEVICE_MAX; device++) {
        switch (device) {
        case Snd::DEVICE_MAIN:
            if (Snd::gVoicesNext[device].changed != 0) {
                pSndSysOpts->set.voicesmaincpu = Snd::gVoicesNext[device].value;
                Snd::gVoicesNext[device].changed = 0;
            }
            if (Snd::gOutputSampleRateNext[device].changed != 0) {
                pSndSysOpts->set.outputrate = Snd::gOutputSampleRateNext[device].value;
                Snd::gOutputSampleRateNext[device].changed = 0;
            }
            break;

        case Snd::DEVICE_HARDWARE:
            if (Snd::gVoicesNext[device].changed != 0) {
                pSndSysOpts->set.voicesdsp = Snd::gVoicesNext[device].value;
                Snd::gVoicesNext[device].changed = 0;
            }
            break;

        default:
            return -5;
        }
    }

    return 0;
}
