#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/file/filesys.h"
#include "Speed/Indep/Libs/snd/9/extern/aemsdef.h"
#include "Speed/Indep/Libs/snd/9/source/library/cmn/saemsi.h"
#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include "snd/sndo.h"
#include <cstddef>
#include <cstring>

int SNDAEMSalmbcurrentpriority = 0x64;

int SNDAEMSalmbpriority;
char *SNDAEMSalmbfilename;
int SNDAEMSalmbfileoffset;
char *SNDAEMSalmbstreamfilename;
int SNDAEMSalmbstreamfileoffset;
char *SNDAEMSalmbploadbuf;
int SNDAEMSalmbloadbufsize;
void *(* SNDAEMSalmbmalloccb)(int);
int SNDAEMSalmbfhandle;
AemsDef::ModuleBank *SNDAEMSalmbpmb;
int SNDAEMSalmbmodulebankhandle;
int SNDAEMSalmbpatchbankhandle;
int SNDAEMSalmbmidibankhandle;
int SNDAEMSalmbfileop;
unsigned char SNDAEMSalmbstage;
signed char SNDAEMSalmblasterror;

void SNDAEMSI_almbservice();

void SNDAEMSI_almbabortload() {
    SNDSYS_entercritical();

    SNDMEMI_free(SNDAEMSalmbfilename);
    if (SNDAEMSalmbstreamfilename != NULL) {
        SNDMEMI_free(SNDAEMSalmbstreamfilename);
    }

    SNDSYS_leavecritical();

    iSNDserverremoveclient(SNDAEMSI_almbservice);
    sndaems.asyncloading = false;
}

void SNDAEMSI_almbservice() {
    int opstatus;

    if (SNDAEMSalmbstage == 0) {
        opstatus = FILESYS_opstatus(SNDAEMSalmbfileop);
        if (opstatus != 1) {
            if (opstatus == 0) return;

            FILESYS_completeop(SNDAEMSalmbfileop);
            SNDAEMSalmblasterror = -19;
            SNDAEMSI_almbabortload();
            return;
        } else {
            SNDAEMSalmbfhandle = FILESYS_completeop(SNDAEMSalmbfileop);
            SNDAEMSalmbfileop = FILESYS_read(SNDAEMSalmbfhandle, SNDAEMSalmbfileoffset, SNDAEMSalmbploadbuf, 0x5C, SNDAEMSalmbpriority, NULL);
            SNDAEMSalmbstage = opstatus;
        }
    }

    if (SNDAEMSalmbstage == 1) {
        if (FILESYS_opstatus(SNDAEMSalmbfileop) != 1) return;

        FILESYS_completeop(SNDAEMSalmbfileop);

        SNDAEMSalmbpmb = reinterpret_cast<AemsDef::ModuleBank *>(SNDAEMSalmbploadbuf);

        char *pusermem = (char *)SNDAEMSalmbmalloccb(SNDAEMSalmbpmb->residentsize);
        SNDAEMSalmbfileop = FILESYS_read(SNDAEMSalmbfhandle, SNDAEMSalmbfileoffset, pusermem, SNDAEMSalmbpmb->residentsize, SNDAEMSalmbpriority, NULL);
        SNDAEMSalmbpmb = reinterpret_cast<AemsDef::ModuleBank *>(pusermem);
        SNDAEMSalmbstage = 2;
    }

    if (SNDAEMSalmbstage == 2) {
        if (FILESYS_opstatus(SNDAEMSalmbfileop) != 1) return;
        FILESYS_completeop(SNDAEMSalmbfileop);
        SNDAEMSalmbstage = 3;
    }

    if (SNDAEMSalmbstage == 3) {
        int throwawaysize;
        if (SNDAEMSalmbpmb->sfxbankoffset != 0) {
            SNDAEMSalmbpatchbankhandle = SNDBANK_asyncload(SNDAEMSalmbfilename, SNDAEMSalmbfileoffset + SNDAEMSalmbpmb->sfxbankoffset, SNDAEMSalmbploadbuf, SNDAEMSalmbloadbufsize, SNDAEMSalmbmalloccb);
            SNDAEMSalmbstage = 4;
        } else {
            SNDAEMSalmbstage = 5;
        }
    }

    if (SNDAEMSalmbstage == 4) {
        if (SNDBANK_asyncdone() <= 0) return;
        SNDAEMSalmbstage = 5;
    }

    if (SNDAEMSalmbstage == 5) {
        if (SNDAEMSalmbpmb->midibankoffset != 0) {
            SNDAEMSalmbmidibankhandle = SNDBANK_asyncload(SNDAEMSalmbfilename, SNDAEMSalmbfileoffset + SNDAEMSalmbpmb->midibankoffset, SNDAEMSalmbploadbuf, SNDAEMSalmbloadbufsize, SNDAEMSalmbmalloccb);
            SNDAEMSalmbstage = 6;
        } else {
            SNDAEMSalmbstage = 7;
        }
    }

    if (SNDAEMSalmbstage == 6) {
        if (SNDBANK_asyncdone() <= 0) return;
        SNDAEMSalmbstage = 7;
    }

    if (SNDAEMSalmbstage == 7) {
        SNDAEMSalmbfileop = FILESYS_read(SNDAEMSalmbfhandle, SNDAEMSalmbfileoffset + SNDAEMSalmbpmb->funcfixupoffset, SNDAEMSalmbploadbuf, SNDAEMSalmbpmb->totalsize - SNDAEMSalmbpmb->funcfixupoffset, SNDAEMSalmbpriority, NULL);
        SNDAEMSalmbstage = 8;
    }

    if (SNDAEMSalmbstage == 8) {
        if (FILESYS_opstatus(SNDAEMSalmbfileop) != 1) return;

        FILESYS_completeop(SNDAEMSalmbfileop);
        SNDAEMSalmbfileop = FILESYS_close(SNDAEMSalmbfhandle, SNDAEMSalmbpriority, NULL);

        SNDSYS_entercritical();
        SNDAEMSI_resolvemodulebank(SNDAEMSalmbpmb, reinterpret_cast<AemsDef::FUNCFIXUPHEADER *>(SNDAEMSalmbploadbuf), SNDAEMSalmbstreamfilename, SNDAEMSalmbstreamfileoffset);

        SNDAEMSalmbpmb->sfxbhandle = SNDAEMSalmbpatchbankhandle;
        SNDAEMSalmbpmb->midibhandle = SNDAEMSalmbmidibankhandle;
        SNDAEMSalmbpmb->modulebankhandle = SNDAEMSalmbmodulebankhandle;
        SNDMEMI_free(SNDAEMSalmbfilename);
        if (SNDAEMSalmbstreamfilename != NULL) {
            SNDMEMI_free(SNDAEMSalmbstreamfilename);
        }
        SNDSYS_leavecritical();

        SNDAEMSalmbstage = 9;
    }

    if (SNDAEMSalmbstage == 9) {
        if (FILESYS_opstatus(SNDAEMSalmbfileop) != 1) return;

        FILESYS_completeop(SNDAEMSalmbfileop);

        iSNDserverremoveclient(SNDAEMSI_almbservice);
        sndaems.asyncloading = false;
    }

}

#ifdef __cplusplus
extern "C" {
#endif

int SNDAEMS_asyncloadmodulebank(char *moduleBankFileName, int moduleBankFileOffset, char *streamFileName, int streamFileOffset, void *pMem, int memSize, void *(* mallocCb)(int)) {
    sndaems.asyncloading = true;
    SNDAEMSalmbploadbuf = (char *)pMem;
    SNDAEMSalmbloadbufsize = memSize;
    SNDAEMSalmbmalloccb = mallocCb;
    SNDAEMSalmblasterror = 0;

    SNDSYS_entercritical();
    SNDAEMSalmbfilename = (char *)SNDMEMI_allocz(strlen(moduleBankFileName) + 1);
    strcpy(SNDAEMSalmbfilename, moduleBankFileName);
    SNDAEMSalmbfileoffset = moduleBankFileOffset;

    if (streamFileName != NULL && streamFileName[0] != 0) {
        SNDAEMSalmbstreamfilename = (char *)SNDMEMI_allocz(strlen(streamFileName) + 1);
        strcpy(SNDAEMSalmbstreamfilename, streamFileName);
    } else {
        SNDAEMSalmbstreamfilename = NULL;
    }

    SNDAEMSalmbstreamfileoffset = streamFileOffset;
    SNDSYS_leavecritical();

    SNDAEMSalmbpatchbankhandle = -1;
    SNDAEMSalmbmidibankhandle = -1;
    SNDAEMSalmbpriority = SNDAEMSalmbcurrentpriority;
    SNDAEMSalmbstage = 0;
    SNDAEMSalmbmodulebankhandle = SNDAEMSI_createmodulebankhandle();
    SNDAEMSalmbfileop = FILESYS_open(moduleBankFileName, 1, SNDAEMSalmbpriority, NULL);

    iSNDserveraddclient(SNDAEMSI_almbservice);
    SNDAEMSI_almbservice();

    if (!sndaems.asyncloading) {
        return SNDAEMSalmblasterror;
    }

    return SNDAEMSalmbmodulebankhandle;
}

int SNDAEMS_asyncloadmodulebankdone() {
    if (!sndaems.asyncloading) {
        if (SNDAEMSalmblasterror != 0) {
            return SNDAEMSalmblasterror;
        }
        return 1;
    }
    return 0;
}

#ifdef __cplusplus
}
#endif
