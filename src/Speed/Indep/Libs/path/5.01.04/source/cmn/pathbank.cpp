#include "pathi.h"

#include <stdio.h>
#include <string.h>

using namespace Path;

int PATHI_loadbank(PATHTRACK *track, int subbanknum)
{
    int result;
    int i;
    int lowesti;
    unsigned int lowest;
    int oldest;
    PATHSUBBANKSTATUS *subbank;
    PATHTRACKINFO *trackinfo;
    PATHSUBBANKINFO *subbankinfo;
    unsigned int aramtotal;
    unsigned int mramtotal;
    bool canpurge;
    bool shouldpurge;
    int subbanksize;

    if (!track->trackimp)
    {
        if (debugchannels & kPathErrorDebugChannel)
            PATHI_printf("( %.8s )  PATH_createbankimp not called yet\n", track->trackname);

        return PATHERR_NOT_INITED;
    }

    if (subbanknum >= track->trackimp->GetNumSubBanks())
    {
        if (debugchannels & kPathErrorDebugChannel)
            PATHI_printf("( %.8s )  PATH_loadbank no subbank %d\n", track->trackname, subbanknum);

        return PATHERR_INV_PARAM;
    }

    if (track->trackimp->GetSubBankPtr(subbanknum))
    {
        if (debugchannels & kPathErrorDebugChannel)
            PATHI_printf("( %.8s )  PATH_loadbank already loaded subbank %d\n", track->trackname,
                         subbanknum);

        return PATHERR_ALREADYLOADED;
    }

    trackinfo = PATHI_gettrackinfo(track->trackID);
    subbankinfo = (PATHSUBBANKINFO *)(trackinfo + 1);
    subbankinfo += subbanknum;

    aramtotal = track->subbankaramuse + subbankinfo->aramsize;
    mramtotal = track->subbankmramuse + subbankinfo->mramsize;

    canpurge = false;
    if (subbankinfo && trackinfo && trackinfo->purgemode)
        canpurge = true;

    shouldpurge = false;
    if ((trackinfo->maxaram && (aramtotal > trackinfo->maxaram || mramtotal > trackinfo->maxaram)) ||
        !track->trackimp->GetAvailSubBankPtr())
        shouldpurge = true;

    if (canpurge && shouldpurge)
    {
        if (trackinfo->purgemode == 2)
        {
            oldest = 0x7FFFFFFF;
            lowest = 0x7FFFFFFF;
            lowesti = -1;

            for (i = 0; i < track->trackimp->GetNumSubBanks(); i++)
            {
                subbank = track->trackimp->GetSubBankPtr(i);
                if (!subbank)
                    continue;

                if (subbank->info.priority <= lowest)
                {
                    if (subbank->info.priority < lowest || subbank->lastplaytime < oldest)
                    {
                        oldest = subbank->lastplaytime;
                        lowest = subbank->info.priority;
                        lowesti = i;
                    }
                }
            }

            if (lowesti == subbanknum)
                return PATHERR_TOOMANY;

            PATHI_unloadbank(track, lowesti);
        }
        else if (trackinfo->purgemode == 1)
        {
            PATHI_unloadmostneglectedsubbank(track);
        }
    }

    subbanksize = (subbankinfo->aramsize + subbankinfo->mramsize) * 1024;

    result = PATHI_loadbankdata(track, subbanknum, subbanksize);
    if (result == 0)
    {
        track->subbankaramuse += subbankinfo->aramsize;
        track->subbankmramuse += subbankinfo->mramsize;

        subbank = track->trackimp->GetSubBankPtr(subbanknum);
        if (subbank)
            subbank->info = *subbankinfo;
    }

    return result;
}

int PATHI_subbankready(PATHTRACK *track, int subbanknum)
{
    PATHSUBBANKSTATUS *subbank;

    if (subbanknum < 0 || subbanknum >= track->trackimp->GetNumSubBanks())
        return PATHERR_INV_PARAM;

    subbank = track->trackimp->GetSubBankPtr(subbanknum);
    if (!subbank)
        return PATHERR_INV_PARAM;

    if (!subbank->filedata)
        return PATHERR_INV_PARAM;

    if (track->loadingfile)
    {
        if (!IPathToReal::realimp->LoadFileDone(track->fileop, subbank->filedata))
            return PATHERR_PENDING;

        track->fileop = PATH_UNLIKELY_VALUE;
        track->freeable = track->trackimp->AddSubBank(subbanknum, subbank->filedata);
        track->loadingfile = 0;
    }

    if (track->loadingsubbank >= 0)
    {
        if (!track->trackimp->AddSubBankDone(subbanknum))
            return PATHERR_PENDING;

        track->trackimp->DetachSubBankHeader(subbanknum, track->freeable);
        track->freeable = 0;
        track->loadingsubbank = -1;
    }

    subbank->subbanknum = subbanknum;
    subbank->ready = 1;

    return PATH_OK;
}

int PATHI_loadbankdata(PATHTRACK *track, int subbanknum, int subbanksize)
{
    PATHSUBBANKSTATUS *subbank;

    if (subbanknum < 0 || subbanknum >= track->trackimp->GetNumSubBanks())
        return PATHERR_INV_PARAM;

    if (track->loadingsubbank >= 0)
        return PATHERR_PENDING;

    subbank = track->trackimp->GetSubBankPtr(subbanknum);
    if (subbank)
        return PATHERR_ALREADYLOADED;

    subbank = track->trackimp->GetAvailSubBankPtr();
    if (!subbank)
        return PATHERR_TOOMANY;

    subbank->ready = 0;
    subbank->subbanknum = subbanknum;

    char newpath[512] = "";
    int len = strlen(track->musicfilename);

    sprintf(newpath, "%.*s%d.mus", len - 4, track->musicfilename, subbanknum);

    int fileop;
    // scaf-r1: ANDAMIO FUERA, CERRADO CON ALMACEN VOLATIL. La barrera
    // `__asm__("" : "+m"(fileop))` se retira: en su lugar el almacen se escribe
    // `*(volatile int *)&fileop = ...`. Para el planificador un almacen volatile
    // no lo cruzan las lecturas de la vtable (y la `lwz` de realimp sigue
    // pudiendo adelantarse, como en el objetivo); para el DWARF la local sigue
    // siendo `int fileop; r1+0x208` --el cast no deja rastro ni añade locales
    // (dwbody igual, regmap 7/7). MEDIDO, sello .text/.rodata del .o:
    //   con la barrera asm              -> 0019da19 / e3866ff5   (100 %)
    //   sin nada                        -> 888cb6f0 / e3866ff5   (87,78 %, 12 filas)
    //   `(&fileop)[0] = X` (ARRAY_REF)  -> 888cb6f0: el frontend lo PLIEGA,
    //                                      igual que quitar la barrera
    //   `*(volatile int *)&fileop = X`  -> 0019da19 / e3866ff5   EXACTO
    // Unidad 2156/2156 B, 5/5 funciones al 100 %. El diagnostico de la r67
    // (exencion fixed_scalar_and_varying_struct_p) sigue en pie: el volatile
    // rompe esa exencion sin mentir el tipo (`int fileop[1]`) ni tocar .debug.
    /* El objetivo materializa y ALMACENA el 0xBEDFACED antes de preparar los
       argumentos de LoadFile --que es el orden de la fuente-- y nuestro
       planificador retrasaba el `stw` doce ranuras, arrastrando con el todo el
       bloque. `fileop` se pasa por referencia, asi que vive en la pila y la
       barrera tiene que ser de MEMORIA: un `"+r"` se ignora en silencio cuando
       se toma la direccion. Medidas: `"m"` de solo lectura y el clobber de
       memoria dejan 4 filas; `"+m"` las deja a cero. */
    *(volatile int *)&fileop = PATH_UNLIKELY_VALUE;

    subbank->filedata = IPathToReal::realimp->LoadFile(newpath, fileop, subbanksize);
    if (!subbank->filedata)
        return PATHERR_CANTOPEN;

    track->loadingsubbank = subbanknum;
    track->loadingfile = 1;
    track->fileop = fileop;

    PATHI_subbankready(track, subbanknum);

    return PATH_OK;
}

int PATHI_unloadbank(PATHTRACK *track, int subbanknum)
{
    int status;
    PATHSUBBANKSTATUS *subbank;
    PATHSUBBANKSTATUS copybankinfo;

    if (subbanknum < 0 || subbanknum >= track->trackimp->GetNumSubBanks())
        return PATHERR_INV_PARAM;

    subbank = track->trackimp->GetSubBankPtr(subbanknum);
    if (!subbank)
        return PATHERR_INV_PARAM;

    copybankinfo = *subbank;

    if (track->node >= 0)
    {
        int sampleoffset = PATHI_sampleoffset(track->node);
        int playingsubbanknum = (unsigned int)sampleoffset >> 16;

        if (playingsubbanknum == subbanknum)
            PATHI_stop(track);
    }

    status = track->trackimp->RemoveSubBank(subbanknum);

    track->subbankaramuse -= copybankinfo.info.aramsize;
    track->subbankmramuse -= copybankinfo.info.mramsize;

    if (track->loadingsubbank == subbanknum)
        track->loadingsubbank = -1;

    return status;
}

int PATHI_unloadmostneglectedsubbank(PATHTRACK *track)
{
    int mostneglected = 0x7FFFFFF;
    int mostneglectedi = -1;

    {
        int subbank;

        for (subbank = 0; subbank < track->trackimp->GetNumSubBanks(); subbank++)
        {
            PATHSUBBANKSTATUS *bank = track->trackimp->GetSubBankPtr(subbank);

            if (!bank)
                continue;

            if (bank->lastplaytime < 0)
                continue;

            if (bank->lastplaytime >= mostneglected)
                continue;

            mostneglected = bank->lastplaytime;
            mostneglectedi = subbank;
        }
    }

    if (mostneglectedi == -1)
        return 0;

    return PATHI_unloadbank(track, mostneglectedi);
}
