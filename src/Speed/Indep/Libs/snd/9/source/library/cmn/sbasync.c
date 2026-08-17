#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/file/filesys.h"
#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include "sbanki.h"
#include "snd/sndo.h"
#include <cstddef>

SNDBANKASYNCSTATE sndbas = {
    NULL,
    100,
    0,
};

void SNDBANKI_abortload() {
    SNDBANKI_asynccompletereads();
    SNDBANKI_asynccompletedownload();

    if (sndbas.pi->readscompleted < sndbas.pi->readsissued || sndbas.pi->processescompleted < sndbas.pi->processesissued) {
        return;
    }

    if (sndbas.pi->fileopen) {
        if (sndbas.pi->closeop == 0) {
            sndbas.pi->closeop = FILESYS_close(sndbas.pi->fhandle, sndbas.readpriority, NULL);
        }
        if (sndbas.pi->closeop != 0) {
            if (FILESYS_opstatus(sndbas.pi->closeop) != 1)
                return;
            FILESYS_completeop(sndbas.pi->closeop);
        }
    }

    if (sndbas.pi->synctaskadded) {
        iSNDserverremoveclient(SNDBANKI_asyncservice);
    }

    sndgs.banklist[sndbas.pi->bhandle].phdr = NULL;
    sndgs.banklist[sndbas.pi->bhandle].locked = 0;
    sndbas.pi = NULL;
}

void SNDBANKI_asyncresolve() {
    BANKVER5 *pb = sndbas.pi->pbank;
    BANKLIST *pbl = &sndgs.banklist[sndbas.pi->bhandle];
    int firstoffset = 0;
    int i = 0;

    pbl->phdr = pb;
    pbl->pspuram = sndbas.pi->pspuram;

    if (pb->spusize != 0) {
        firstoffset = i;
        for (; i < pb->numpatches; i++) {
            if (pb->patch[i] != NULL) {
                SNDBANKI_asyncresolvepatch(0x200, reinterpret_cast<TAGGEDPATCH *>((int)&pb->patch[i] + (int)pb->patch[i]), (char *)pbl->pspuram,
                                           &firstoffset);
            }
        }
    }

    pbl->locked = 0;
}

void SNDBANKI_asynccompletereads() {
    SNDASYNCLOADBUFFER *psalb;
    int curbuf = sndbas.pi->readscompleted & 3;
    int bytesread;

    if (sndbas.pi->readsissued <= sndbas.pi->readscompleted)
        return;

    do {
        psalb = &sndbas.pi->salb[curbuf];
        if (psalb->state != 1 || FILESYS_opstatus(psalb->fileop) != 1)
            break;

        bytesread = FILESYS_completeop(psalb->fileop);
        if (bytesread > 0) {
            psalb->state = 2;
        } else {
            psalb->state = 0;
        }

        sndbas.pi->readscompleted++;
        if (++curbuf > 3) {
            curbuf = 0;
        }
    } while (sndbas.pi->readsissued > sndbas.pi->readscompleted);
}

void SNDBANKI_asyncissuereads() {
    SNDASYNCLOADBUFFER *psalb;
    int curbuf = sndbas.pi->readsissued & 3;
    int maxsize = 0xFFFFFF;
    int readoffset;

    if (sndbas.pi->readscompleted <= sndbas.pi->readsissued - 4)
        return;

    do {
        readoffset = sndbas.pi->readsissued * sndbas.pi->loadbufsize;
        if (sndbas.pi->pbank != NULL) {
            maxsize = sndbas.pi->totalbanksize - readoffset;
            if (maxsize <= 0)
                break;
        }
        if (maxsize > sndbas.pi->loadbufsize) {
            maxsize = sndbas.pi->loadbufsize;
        }

        psalb = &sndbas.pi->salb[curbuf];
        if (psalb->state != 0)
            break;

        psalb->fileop = FILESYS_read(sndbas.pi->fhandle, sndbas.pi->fileoffset + readoffset, psalb->ploadbuf, maxsize, sndbas.readpriority, NULL);
        psalb->state = 1;
        sndbas.pi->readsissued++;
        if (++curbuf > 3) {
            curbuf = 0;
        }
    } while (sndbas.pi->readscompleted > sndbas.pi->readsissued - 4);
}

void SNDBANKI_asyncxferhdr() {
    if (sndbas.pi->hdrxferremaining <= 0)
        return;

    SNDASYNCLOADBUFFER *psalb;
    int curbuf = sndbas.pi->processesissued & 3;
    int maxsize;

    if (sndbas.pi->processescompleted >= sndbas.pi->readscompleted)
        return;

    do {
        psalb = &sndbas.pi->salb[curbuf];
        if (sndbas.pi->hdrxferremaining < sndbas.pi->loadbufsize) {
            maxsize = sndbas.pi->hdrxferremaining;
        } else {
            maxsize = sndbas.pi->loadbufsize;
        }

        Snd::Util::MemCpy((char *)sndbas.pi->pbank + sndbas.pi->loadbufsize * sndbas.pi->processescompleted, psalb->ploadbuf, maxsize);

        sndbas.pi->hdrxferremaining -= maxsize;
        if (sndbas.pi->hdrxferremaining <= 0 && maxsize < sndbas.pi->loadbufsize) {
            sndbas.pi->dloffset = maxsize;
            return;
        }

        psalb->state = 0;
        sndbas.pi->processesissued++;
        sndbas.pi->processescompleted++;
        if (++curbuf > 3) {
            curbuf = 0;
        }
    } while (sndbas.pi->processescompleted < sndbas.pi->readscompleted);
}

void SNDBANKI_asynccompletedownload() {
    SNDASYNCLOADBUFFER *psalb;
    int curbuf = sndbas.pi->processescompleted & 3;

    if (sndbas.pi->processesissued <= sndbas.pi->processescompleted)
        return;

    do {
        psalb = &sndbas.pi->salb[curbuf];
        if (SNDPLATFORM_downloadcomplete(psalb->dlhandle) == 0 || psalb->dlhandle <= 0)
            break;

        psalb->state = 0;
        sndbas.pi->processescompleted++;
        if (++curbuf > 3) {
            curbuf = 0;
        }

    } while (sndbas.pi->processesissued > sndbas.pi->processescompleted);
}

void SNDBANKI_asyncissuedownloads() {
    if (sndbas.pi->spuxferremaining <= 0)
        return;

    SNDASYNCLOADBUFFER *psalb;
    int curbuf = sndbas.pi->processescompleted & 3;
    int xfersize;

    if (sndbas.pi->processesissued >= sndbas.pi->processescompleted + 4)
        return;

    do {
        psalb = &sndbas.pi->salb[curbuf];
        if (psalb->state != 2)
            break;
        if (sndbas.pi->spuxferremaining <= 0) {
            psalb->state = 0;
            break;
        }

        if (sndbas.pi->dloffset < sndbas.pi->loadbufsize) {
            xfersize = sndbas.pi->loadbufsize - sndbas.pi->dloffset;
            if (xfersize > sndbas.pi->spuxferremaining) {
                xfersize = sndbas.pi->spuxferremaining;
            }

            psalb->dlhandle =
                SNDPLATFORM_download(0x200, &psalb->ploadbuf[sndbas.pi->dloffset], &sndbas.pi->pspuram[sndbas.pi->spudownloaded], xfersize);
            psalb->state = 3;
            sndbas.pi->spudownloaded += xfersize;
            sndbas.pi->spuxferremaining -= xfersize;
        }

        if (psalb->state == 3) {
            sndbas.pi->dloffset = 0;
            sndbas.pi->processesissued++;
        }

        if (++curbuf > 3) {
            curbuf = 0;
        }
    } while (sndbas.pi->processesissued < sndbas.pi->processescompleted + 4);
}

void SNDBANKI_asyncprocess() {
    SNDBANKI_asynccompletereads();
    SNDBANKI_asyncxferhdr();
    SNDBANKI_asynccompletedownload();
    SNDBANKI_asyncissuedownloads();
    SNDBANKI_asyncissuereads();
}

void SNDBANKI_asyncserver() {
    SNDBANKI_asyncprocess();

    if (sndbas.pi->hdrxferremaining > 0)
        return;
    if (sndbas.pi->spuxferremaining > 0)
        return;

    if (sndbas.pi->closeop == 0) {
        sndbas.pi->closeop = FILESYS_close(sndbas.pi->fhandle, sndbas.readpriority, NULL);
    }

    if (sndbas.pi->processesissued != sndbas.pi->processescompleted)
        return;

    if (FILESYS_opstatus(sndbas.pi->closeop) == 1) {
        FILESYS_completeop(sndbas.pi->closeop);
        SNDSYS_remove100hzclient(SNDBANKI_asyncserver);
        SNDBANKI_asyncresolve();
        sndbas.pi = NULL;
    }
}

void SNDBANKI_asyncservice() {
    SNDASYNCLOADBUFFER *psalb;
    int opstatus;

    if (sndbas.pi->aborting) {
        SNDBANKI_abortload();
        return;
    }
    if (sndbas.pi->openop != 0) {
        opstatus = FILESYS_opstatus(sndbas.pi->openop);
        if (opstatus != 1) {
            if (opstatus == 0)
                return;

            FILESYS_completeop(sndbas.pi->openop);
            sndbas.lasterror = -19;
            sndbas.pi->aborting = true;
            SNDBANKI_abortload();
            return;
        }

        sndbas.pi->fhandle = FILESYS_completeop(sndbas.pi->openop);
        sndbas.pi->openop = 0;
        sndbas.pi->fileopen = true;

        SNDBANKI_asyncissuereads();
    }

    psalb = &sndbas.pi->salb[0];
    if (FILESYS_opstatus(psalb->fileop) != 1)
        return;

    FILESYS_completeop(psalb->fileop);
    psalb->state = 2;

    sndbas.pi->readscompleted++;
    sndbas.pi->pbank = reinterpret_cast<BANKVER5 *>(psalb->ploadbuf);
    sndbas.pi->hdrxferremaining = sndbas.pi->pbank->hdrsize;
    sndbas.pi->totalbanksize = sndbas.pi->pbank->hdrsize;
    sndbas.pi->spuxferremaining = sndbas.pi->pbank->spusize;
    sndbas.pi->totalbanksize = sndbas.pi->totalbanksize + sndbas.pi->pbank->spusize;

    sndbas.pi->pbank = reinterpret_cast<BANKVER5 *>(sndbas.pi->mallocfn(sndbas.pi->hdrxferremaining));
    if (sndbas.pi->pbank == NULL) {
        sndbas.pi->aborting = true;
        sndbas.lasterror = -6;
        return;
    }

    if (sndbas.pi->spuxferremaining != 0) {
        sndbas.pi->pspuram = reinterpret_cast<char *>(SNDPLATFORM_memalloc(0x200, sndbas.pi->spuxferremaining + 0x40));
        if (sndbas.pi->pspuram == NULL) {
            sndbas.pi->aborting = true;
            sndbas.lasterror = -6;
            return;
        }
    } else {
        sndbas.pi->pspuram = NULL;
    }

    iSNDserverremoveclient(SNDBANKI_asyncservice);
    sndbas.pi->synctaskadded = false;
    SNDBANKI_asyncprocess();
    SNDSYS_add100hzclient(SNDBANKI_asyncserver);
}

int SNDBANK_asyncloadi(char *filename, int fileoffset, void *pmem, int memsize, void *(*mallocfn)(int), bool failgracefully) {
    SNDASYNCLOADBUFFER *psalb;
    BANKLIST *pbanklist;
    int i;

    sndbas.pi = reinterpret_cast<SNDBANKASYNCINSTANCE *>(pmem);
    sndbas.lasterror = 0;

    sndbas.pi->aborting = false;
    sndbas.pi->fileopen = false;
    sndbas.pi->failgracefully = failgracefully;
    sndbas.pi->mallocfn = mallocfn;
    sndbas.pi->fileoffset = fileoffset;
    sndbas.pi->dloffset = 0;
    sndbas.pi->totalbanksize = 0;
    sndbas.pi->hdrxferremaining = 0;
    sndbas.pi->spudownloaded = 0;
    sndbas.pi->pbank = NULL;
    sndbas.pi->readsissued = 0;
    sndbas.pi->readscompleted = 0;
    sndbas.pi->processesissued = 0;
    sndbas.pi->processescompleted = 0;
    sndbas.pi->closeop = 0;
    sndbas.pi->loadbufsize = (unsigned int)(memsize - 460) >> 2;

    if (sndbas.pi->loadbufsize > 0x6000) {
        sndbas.pi->loadbufsize = 0x6000;
    }
    sndbas.pi->loadbufsize &= ~0x3F;
    sndbas.pi->openop = FILESYS_open(filename, 1, sndbas.readpriority, NULL);

    for (i = 0; i < 4; i++) {
        psalb = &sndbas.pi->salb[i];
        psalb->dlhandle = -1;
        psalb->state = 0;
        psalb->ploadbuf = (char *)(((sndbas.pi->loadbufsize + 0x40) * i + (int)pmem + 0x10C) & ~0x3F);
    }

    for (i = 0; i < sndgs.sso.set.maxbanks; i++) {
        pbanklist = &sndgs.banklist[i];
        if (pbanklist->phdr == NULL) {
            pbanklist->locked = 1;
            pbanklist->phdr = (BANKVER5 *)1;
            sndbas.pi->bhandle = i;
            break;
        }
    }

    iSNDserveraddclient(SNDBANKI_asyncservice);
    sndbas.pi->synctaskadded = true;
    SNDBANKI_asyncservice();

    if (sndbas.pi == NULL) {
        return sndbas.lasterror;
    } else {
        return sndbas.pi->bhandle;
    }
}

int SNDBANK_asyncload(char *filename, int fileoffset, void *pmem, int memsize, void *(*mallocfn)(int)) {
    return SNDBANK_asyncloadi(filename, fileoffset, pmem, memsize, mallocfn, false);
}

int SNDBANK_asyncdone() {
    if (sndbas.pi == NULL) {
        if (sndbas.lasterror != 0) {
            return sndbas.lasterror;
        }
        return 1;
    }
    return 0;
}
