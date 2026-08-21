#include "pathi.h"
#include "path/IPathToReal.h"
#include <stdio.h>
#include <string.h>

int PATHI_loadbank(PATHTRACK *track, int subbanknum) {
    int result;
    int i;
    int lowesti;
    unsigned int lowest;
    unsigned int oldest;
    PATHSUBBANKSTATUS *subbank;
    PATHTRACKINFO *trackinfo;
    PATHSUBBANKINFO *subbankinfo;
    unsigned int aramtotal;
    unsigned int mramtotal;
    bool canpurge;
    bool shouldpurge;
    int subbanksize;

    if (track->trackimp == 0) {
        if ((Path::debugchannels & 0x100) != 0) {
            PATHI_printf("( %.8s )  PATH_createbankimp not called yet\n");
        }
        return PATHERR_NOT_INITED;
    }
    if (subbanknum >= track->trackimp->GetMaxSubBanks()) {
        if ((Path::debugchannels & 0x100) != 0) {
            PATHI_printf("( %.8s )  PATH_loadbank no subbank %d\n");
        }
        return PATHERR_INV_PARAM;
    }
    if (track->trackimp->GetSubBankPtr(subbanknum) != 0) {
        if ((Path::debugchannels & 0x100) != 0) {
            PATHI_printf("( %.8s )  PATH_loadbank already loaded subbank %d\n");
        }
        return PATHERR_ALREADYLOADED;
    }
    trackinfo = PATHI_gettrackinfo(track->trackID);
    subbankinfo = reinterpret_cast<PATHSUBBANKINFO *>(trackinfo + 1) + subbanknum;
    canpurge = subbankinfo != 0 && trackinfo != 0 && trackinfo->purgemode != 0;
    aramtotal = trackinfo->maxaram;
    mramtotal = trackinfo->maxmram;
    shouldpurge = (aramtotal != 0 && aramtotal < track->subbankaramuse + subbankinfo->aramsize) ||
                  (mramtotal != 0 && mramtotal < track->subbankmramuse + subbankinfo->mramsize) ||
                  track->trackimp->GetAvailSubBankPtr() == 0;
    if (canpurge && shouldpurge) {
        if (trackinfo->purgemode == 2) {
            lowesti = -1;
            lowest = 0x7fffffff;
            oldest = 0x7fffffff;
            for (i = 0; i < track->trackimp->GetMaxSubBanks(); i++) {
                subbank = track->trackimp->GetSubBankPtr(i);
                if (subbank != 0 && subbank->info.priority <= lowest &&
                    (subbank->info.priority < lowest || subbank->lastplaytime < static_cast<int>(oldest))) {
                    lowesti = i;
                    oldest = subbank->lastplaytime;
                    lowest = subbank->info.priority;
                }
            }
            if (lowesti == subbanknum) {
                return PATHERR_TOOMANY;
            }
            PATHI_unloadbank(track, lowesti);
        } else if (trackinfo->purgemode == 1) {
            PATHI_unloadmostneglectedsubbank(track);
        }
    }
    subbanksize = (subbankinfo->aramsize + subbankinfo->mramsize) * 1024;
    result = PATHI_loadbankdata(track, subbanknum, subbanksize);
    if (result == 0) {
        track->subbankaramuse += subbankinfo->aramsize;
        track->subbankmramuse += subbankinfo->mramsize;
        subbank = track->trackimp->GetSubBankPtr(subbanknum);
        if (subbank != 0) {
            subbank->info = *subbankinfo;
        }
    }
    return result;
}

int PATHI_subbankready(PATHTRACK *track, int subbanknum) {
    if (subbanknum < 0 || subbanknum >= track->trackimp->GetNumSubBanks()) {
        return PATHERR_INV_PARAM;
    }

    PATHSUBBANKSTATUS *subbank = track->trackimp->GetSubBankPtr(subbanknum);
    if (subbank == 0) {
        return PATHERR_INV_PARAM;
    }
    if (subbank->filedata == 0) {
        return PATHERR_INV_PARAM;
    }
    if (track->loadingfile != 0) {
        if (Path::IPathToReal::realimp->LoadFileDone(track->fileop, subbank->filedata) == 0) {
            return PATHERR_PENDING;
        }
        track->fileop = PATH_UNLIKELY_VALUE;
        track->freeable = track->trackimp->AddSubBank(subbanknum, subbank->filedata);
        track->loadingfile = 0;
    }
    if (track->loadingsubbank >= 0) {
        if (track->trackimp->AddSubBankDone(subbanknum) == 0) {
            return PATHERR_PENDING;
        }
        track->trackimp->DetachSubBankHeader(subbanknum, track->freeable);
        track->freeable = 0;
        track->loadingsubbank = -1;
    }
    subbank->subbanknum = subbanknum;
    subbank->ready = 1;
    return 0;
}

int PATHI_loadbankdata(PATHTRACK *track, int subbanknum, int subbanksize) {
    if (subbanknum < 0 || subbanknum >= track->trackimp->GetNumSubBanks()) {
        return PATHERR_INV_PARAM;
    }
    if (track->loadingsubbank >= 0) {
        return PATHERR_PENDING;
    }
    if (track->trackimp->GetSubBankPtr(subbanknum) != 0) {
        return PATHERR_ALREADYLOADED;
    }

    PATHSUBBANKSTATUS *subbank;

    subbank = track->trackimp->GetAvailSubBankPtr();

    if (subbank == 0) {
        return PATHERR_TOOMANY;
    }

    subbank->ready = 0;
    subbank->subbanknum = subbanknum;

    char newpath[512] = "";
    int len;

    len = strlen(track->musicfilename);
    sprintf(newpath, "%.*s%d.mus", len - 4, track->musicfilename, subbanknum);

    int fileop = PATH_UNLIKELY_VALUE;
    subbank->filedata = Path::IPathToReal::realimp->LoadFile(newpath, fileop, subbanksize);
    if (subbank->filedata == 0) {
        return PATHERR_CANTOPEN;
    }
    track->loadingsubbank = subbanknum;
    track->loadingfile = 1;
    track->fileop = fileop;
    PATHI_subbankready(track, subbanknum);
    return 0;
}

int PATHI_unloadbank(PATHTRACK *track, int subbanknum) {
    int status;
    PATHSUBBANKSTATUS *subbank;
    PATHSUBBANKSTATUS copybankinfo;

    if (subbanknum < 0 || subbanknum >= track->trackimp->GetNumSubBanks()) {
        return PATHERR_INV_PARAM;
    }
    subbank = track->trackimp->GetSubBankPtr(subbanknum);
    if (subbank == 0) {
        return PATHERR_INV_PARAM;
    }
    copybankinfo = *subbank;
    if (track->node >= 0) {
        int sampleoffset;
        int playingsubbanknum;

        sampleoffset = PATHI_sampleoffset(track->node);
        playingsubbanknum = static_cast<unsigned int>(sampleoffset) >> 16;
        if (playingsubbanknum == subbanknum) {
            PATHI_stop(track);
        }
    }
    status = track->trackimp->RemoveSubBank(subbanknum);
    track->subbankaramuse -= copybankinfo.info.aramsize;
    track->subbankmramuse -= copybankinfo.info.mramsize;
    if (track->loadingsubbank == subbanknum) {
        track->loadingsubbank = -1;
    }
    return status;
}

int PATHI_unloadmostneglectedsubbank(PATHTRACK *track) {
    int mostneglected;
    int mostneglectedi;

    mostneglected = 0x7ffffff;
    mostneglectedi = -1;
    {
        int subbank;

        for (subbank = 0; subbank < track->trackimp->GetNumSubBanks(); subbank++) {
            PATHSUBBANKSTATUS *bank = track->trackimp->GetSubBankPtr(subbank);

            if (bank != 0 && bank->lastplaytime >= 0 && bank->lastplaytime < mostneglected) {
                mostneglectedi = subbank;
                mostneglected = bank->lastplaytime;
            }
        }
    }
    if (mostneglectedi != -1) {
        return PATHI_unloadbank(track, mostneglectedi);
    }
    return 0;
}
