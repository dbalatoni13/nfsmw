#include "Speed/Indep/Libs/snd/9/extern/aemsdef.h"
#include "Speed/Indep/Libs/snd/9/source/library/cmn/saemsi.h"
#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include "snd/sndo.h"
#include <cstddef>
#include <cstring>

char *SNDAEMSalmbmpmodulebank;
char *SNDAEMSalmbmstreamfilename;
int SNDAEMSalmbmstreamfileoffset;
void *(* SNDAEMSalmbmmalloccb)(int);
AemsDef::ModuleBank *SNDAEMSalmbmpmb;
int SNDAEMSalmbmpatchbankhandle;
int SNDAEMSalmbmmidibankhandle;
signed char SNDAEMSalmbmpatchloaded;
signed char SNDAEMSalmbmmidiloaded;

void SNDAEMSI_almbmservice() {
    if (SNDAEMSalmbmpatchloaded == 0 && SNDAEMSalmbmpatchbankhandle < 0) {
        if (SNDAEMSalmbmpmb->sfxbankoffset != 0) {
            SNDBANK_asyncloadmem(&SNDAEMSalmbmpatchbankhandle, SNDAEMSalmbmpmodulebank + SNDAEMSalmbmpmb->sfxbankoffset);
        } else {
            SNDAEMSalmbmpatchloaded = 1;
        }
    }

    if (SNDAEMSalmbmpatchbankhandle >= 0 && SNDAEMSalmbmpatchloaded == 0) {
        if (SNDBANK_asyncloadmemdone() <= 0) return;

        int headersize = SNDbankheadersize(SNDAEMSalmbmpatchbankhandle);
        void *phdr = SNDAEMSalmbmmalloccb(headersize);
        SNDbankheadercopy(phdr, SNDAEMSalmbmpatchbankhandle);
        SNDAEMSalmbmpatchloaded = 1;
    }

    if (SNDAEMSalmbmmidiloaded == 0 && SNDAEMSalmbmmidibankhandle < 0) {
        if (SNDAEMSalmbmpmb->midibankoffset != 0) {
            SNDBANK_asyncloadmem(&SNDAEMSalmbmmidibankhandle, SNDAEMSalmbmpmodulebank + SNDAEMSalmbmpmb->midibankoffset);
        } else {
            SNDAEMSalmbmmidiloaded = 1;
        }
    }

    if (SNDAEMSalmbmmidibankhandle >= 0 && SNDAEMSalmbmmidiloaded == 0) {
        if (SNDBANK_asyncloadmemdone() <= 0) return;

        int headersize = SNDbankheadersize(SNDAEMSalmbmmidibankhandle);
        void *phdr = SNDAEMSalmbmmalloccb(headersize);
        SNDbankheadercopy(phdr, SNDAEMSalmbmmidibankhandle);
        SNDAEMSalmbmmidiloaded = 1;
    }

    if (SNDAEMSalmbmmidiloaded == 0) return;

    iSNDserverremoveclient(SNDAEMSI_almbmservice);

    SNDSYS_entercritical();
    SNDAEMSI_resolvemodulebank(SNDAEMSalmbmpmb, reinterpret_cast<AemsDef::FUNCFIXUPHEADER *>(SNDAEMSalmbmpmodulebank + SNDAEMSalmbmpmb->funcfixupoffset), SNDAEMSalmbmstreamfilename, SNDAEMSalmbmstreamfileoffset);

    SNDAEMSalmbmpmb->sfxbhandle = SNDAEMSalmbmpatchbankhandle;
    SNDAEMSalmbmpmb->midibhandle = SNDAEMSalmbmmidibankhandle;
    if (SNDAEMSalmbmstreamfilename != NULL) {
        SNDMEMI_free(SNDAEMSalmbmstreamfilename);
    }
    sndaems.asyncloading = false;
    SNDSYS_leavecritical();
}

#ifdef __cplusplus
extern "C" {
#endif


int SNDAEMS_asyncloadmodulebankmem(void *pModuleBank, char *streamFileName, int streamFileOffset, void *(* mallocCb)(int)) {
    sndaems.asyncloading = true;
    SNDAEMSalmbmpmodulebank = (char *)pModuleBank;
    SNDAEMSalmbmmalloccb = mallocCb;

    SNDSYS_entercritical();
    if (streamFileName != NULL && streamFileName[0] != 0) {
        SNDAEMSalmbmstreamfilename = (char *)SNDMEMI_allocz(strlen(streamFileName) + 1);
        strcpy(SNDAEMSalmbmstreamfilename, streamFileName);
    } else {
        SNDAEMSalmbmstreamfilename = 0;
    }

    SNDAEMSalmbmstreamfileoffset = streamFileOffset;
    SNDSYS_leavecritical();

    SNDAEMSalmbmpatchloaded = 0;
    SNDAEMSalmbmmidiloaded = 0;

    SNDAEMSalmbmpmb = reinterpret_cast<AemsDef::ModuleBank *>(pModuleBank);
    int residentsize = SNDAEMSalmbmpmb->residentsize;

    SNDAEMSalmbmpatchbankhandle = -1;
    SNDAEMSalmbmmidibankhandle = -1;
    SNDAEMSalmbmpmb = reinterpret_cast<AemsDef::ModuleBank *>(SNDAEMSalmbmmalloccb(residentsize));

    Snd::Util::MemCpy(SNDAEMSalmbmpmb, pModuleBank, residentsize);
    SNDAEMSalmbmpmb->modulebankhandle = SNDAEMSI_createmodulebankhandle();
    iSNDserveraddclient(SNDAEMSI_almbmservice);
    SNDAEMSI_almbmservice();

    return SNDAEMSalmbmpmb->modulebankhandle;
}

int SNDAEMS_asyncloadmodulebankmemdone() {
    return (int)(sndaems.asyncloading == false);
}

#ifdef __cplusplus
}
#endif
