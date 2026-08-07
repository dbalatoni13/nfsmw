#include "Speed/Indep/Libs/snd/9/extern/aemsdef.h"
#include "Speed/Indep/Libs/snd/9/source/library/cmn/saemsi.h"
#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include <cstddef>

AemsDef::SNDAEMS sndaems;

#ifdef __cplusplus
extern "C" {
#endif

int SNDAEMS_addmodulebank(void *pBank, char *streamFileName, int streamFileNameOffset, void *(* mallocCb)(void *, int, int)) {
    AemsDef::ModuleBank *pModuleBank;
    void *pusermem;
    int residentSize;
    int sfxret = 0;
    int midiret = 0;
    int errorcode;
    int midiSize;
    int midiDstOffset;

    pModuleBank = reinterpret_cast<AemsDef::ModuleBank *>(pBank);
    if (sndaems.logaddmodulebank != NULL) {
        sndaems.logaddmodulebank();
    }

    pModuleBank->modulebankhandle = SNDAEMSI_createmodulebankhandle();

    if (pModuleBank->sfxbankoffset != 0) {
        sfxret = SNDbankadd(&pModuleBank->sfxbhandle, &pModuleBank->id[pModuleBank->sfxbankoffset]);
        if (sfxret < 0) {
            errorcode = sfxret;
            goto abort;
        }
    }
    if (pModuleBank->midibankoffset != 0) {
        midiret = SNDbankadd(
            &pModuleBank->midibhandle,
            &pModuleBank->id[pModuleBank->midibankoffset]);
        if (midiret < 0) {
            errorcode = midiret;
            goto abort;
        }
        errorcode = midiret;
    }

    residentSize = pModuleBank->residentsize;
    if (sfxret == 7) {
        residentSize += SNDbankheadersize(pModuleBank->sfxbhandle);
    } else {
        residentSize += pModuleBank->sfxbanksizepadded;
    }

    if (midiret == 7) {
        midiSize = SNDbankheadersize(pModuleBank->midibhandle);
    } else {
        midiSize = pModuleBank->midibanksizepadded;
    }
    midiDstOffset = residentSize;

    residentSize += midiSize;
    pusermem = mallocCb(pModuleBank, residentSize, pModuleBank->totalsize);
    if (pusermem == NULL) {
        errorcode = -6;
        goto abort;
    }

    if (pusermem != pBank) {
        Snd::Util::MemCpy(pusermem, pModuleBank, pModuleBank->residentsize);
        pModuleBank = reinterpret_cast<AemsDef::ModuleBank *>(pusermem);
        if (sfxret != 0) {
            SNDbankheadercopy(&pModuleBank->id[pModuleBank->residentsize], pModuleBank->sfxbhandle);
        }
    }
    if (midiret > 0) {
        if (pusermem != pBank || sfxret == 7) {
            SNDbankheadercopy(&reinterpret_cast<char *>(pusermem)[midiDstOffset], pModuleBank->midibhandle);
        }
    }

    SNDAEMSI_resolvemodulebank(
        pModuleBank,
        reinterpret_cast<AemsDef::FUNCFIXUPHEADER *>(&reinterpret_cast<char *>(pBank)[pModuleBank->funcfixupoffset]),
        streamFileName,
        streamFileNameOffset
    );

    return pModuleBank->modulebankhandle;

abort:
    if (sfxret > 0) {
        SNDbankremove(pModuleBank->sfxbhandle);
    }
    if (midiret > 0) {
        SNDbankremove(pModuleBank->midibhandle);
    }

    return errorcode;
}

#ifdef __cplusplus
}
#endif
