#include "pathi.h"

#include <stdio.h>
#include <string.h>

using namespace Path;

int PATH_createstreamtrack(int trackhandle, char *musfilename, int latency)
{
    int result;

    if (!PATHI_lock())
        return PATHERR_INUSE;

    result = PATHI_createtrack(trackhandle, musfilename);
    if (result >= 0)
    {
        PATHTRACK *track = PATHI_gettrackptr(trackhandle);

        if (track)
            track->latency = (unsigned short)latency;
    }

    PATHI_unlock();

    return result;
}

/* RESUELTO r46 -- 424 B al 100 %. Lo que faltaba NO era reparto de registros:
   las 106 instrucciones ya eran identicas y la unica diferencia eran los 8 B de
   marco (`stwu r1,-0x28` contra `-0x20`).

   Esos 8 B son el area X del marco SVR4 de rs6000 --el temporal de conversion
   entero<->flotante--, que va detras de las locales y delante de los registros
   guardados: 8(fijo) + L(8) + X(8) = 0x18, que es donde el objetivo pone
   `stmw r30`. gcc 2.9 la reserva al EXPANDIR una conversion y NO la libera
   aunque despues muera todo el codigo de esa conversion. Medido: una conversion
   VIVA sube el marco a -0x28 y anade 44 B de codigo; una conversion MUERTA lo
   sube a -0x28 con las MISMAS 106 instrucciones.

   Que la conversion estaba en el original lo dice el mapa de lineas: el cuerpo
   ocupa las lineas 85..123 y deja sin una sola instruccion los huecos 92, 94 y
   101..109 --nueve lineas seguidas entre `byterate = PATHI_bytesperms(...)` y
   la llamada a `CreateStreamTrack`--. La sentencia exacta no es recuperable: el
   DWARF del original no nombra ninguna local de mas, y una local muerta SI sale
   en el DWARF (comprobado compilando una), asi que el original la escribio de
   otra forma. Dentro del bloque de `byterate` la conversion desplaza el
   planificador (95,368 %); delante de `trackimp = 0` reproduce el objeto
   EXACTO. Es una reconstruccion del TIPO de sentencia, no de su texto. */
IPathTrack *PATH_createstreamimp(int trackhandle, int maxrequests, float buffertime)
{
    IPathTrack *trackimp;

    if (!PATHI_lock())
        return 0;

    {
        /* muerta: solo reserva el area X del marco -- ver la nota de arriba */
        int buffersize = (int)buffertime;

        if (buffersize < 0)
            buffersize = 0;
    }

    trackimp = 0;

    if (!PATHI_switchvoice(trackhandle))
        goto abort;

    {
        PATHTRACK *track = PATHI_gettrackptr(trackhandle);

        if (!track)
            goto abort;

        {
            int byterate = PATHI_bytesperms(track->trackID);
            int result;

            result = IPathToSnd::sndimp->CreateStreamTrack(&trackimp, maxrequests, buffertime,
                                                           byterate);

            if (!trackimp || result < 0)
                goto abort;

            track->trackimp = trackimp;
            trackimp->SetName(track->trackname);
            trackimp->SetFilePath(track->musicfilename);
            trackimp->SetTrackInfo(PATHI_gettrackinfo(track->trackID));
            trackimp->SetVolume(track->volume * track->volscale / 100);
        }
    }

abort:
    PATHI_unlock();

    return trackimp;
}

int PATHI_createtrack(int trackhandle, char *musfilename)
{
    PATHTRACK *track;
    int result;

    if (!PATHI_switchvoice(trackhandle))
        return PATHERR_INV_PARAM;

    result = PATHI_inittrack(trackhandle, musfilename);
    if (result < 0)
        return result;

    track = PATHI_gettrackptr(trackhandle);
    pfstate->track[track->trackID] = track;

    return PATH_OK;
}

int PATHI_inittrack(int trackhandle, char *musfilename)
{
    int i;
    int trackID = 0;
    int voiceID;
    PATHTRACK *track;
    char *p;
    char c;

    if (!(trackhandle & PATH_ALL_TRACKS) || !(trackhandle & PATH_ALL_PROJECTS) ||
        !(trackhandle & PATH_ALL_VOICES))
        return PATHERR_INV_PARAM;

    for (trackID = 0; trackID < PATH_MAX_TRACKS; trackID++)
    {
        if (!((trackhandle >> trackID) & 1))
            continue;
        break;
    }

    voiceID = trackhandle & 0xFF000000;

    if (pfstate->track[trackID])
        return PATHERR_TOOMANY;

    track = (PATHTRACK *)PATHI_memalloc(sizeof(PATHTRACK));
    if (!track)
        return PATHERR_FAILALLOC;

    memset(track, 0, sizeof(PATHTRACK));

    track->trackID = trackID;
    track->nobranch = 0;
    track->volscale = Path::volscale;
    track->volume = 127;
    track->node = -1;
    track->nodebeat = -1;
    track->volumefade.fadeto = -1;
    track->volumefade.fadefrom = -1;
    track->volumefade.fadenum = -1;
    track->sfxsendfade.fadeto = -1;
    track->sfxsendfade.fadefrom = -1;
    track->sfxsendfade.fadenum = -1;
    track->drylevelfade.fadefrom = -1;
    track->drylevelfade.fadenum = -1;
    track->pitchfade.fadefrom = -1;
    track->pitchfade.fadenum = -1;
    track->stretchfade.fadefrom = -1;
    track->stretchfade.fadenum = -1;
    track->sfxbus = Path::defaultfxbus;
    track->repeatnode = -1;
    track->status = PATHTRACK_STOPPED;
    track->newestrequesthandle = -1;
    track->loadingsubbank = -1;
    track->fileop = PATH_UNLIKELY_VALUE;
    track->mainvoice = ((trackhandle & PATH_ALL_VOICES) == 0x10000000);

    strcpy(track->musicfilename, musfilename);

    p = musfilename + strlen(musfilename);
    for (; p >= musfilename; p--)
    {
        if (*p == '\\' || *p == '/' || *p == ':')
            break;
    }
    p++;

    for (i = 0; i < PATH_MAX_VOICES; i++)
    {
        if (((voiceID >> (i + 28)) & 1) == 0)
            continue;
        break;
    }

    sprintf(track->trackname, "%x%s", i + 1, p);

    c = 'a';
    for (p = &track->trackname[1]; p < &track->trackname[32]; p++)
    {
        c = (c && *p) ? (char)(*p | 0x20) : (char)0;

        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
            continue;

        c = 0;
        if (p >= &track->trackname[12])
        {
            *p = 0;
            break;
        }
        *p = ' ';
    }
    track->trackname[11] = 0;

    pfstate->track[trackID] = track;

    return trackID;
}

PATHTRACK *PATHI_gettrackptr(unsigned int trackhandle)
{
    unsigned int p;
    int t;
    unsigned int voices = trackhandle & PATH_ALL_VOICES;
    unsigned int projects = trackhandle & PATH_ALL_PROJECTS;
    unsigned int tracks = trackhandle & PATH_ALL_TRACKS;

    for (p = 0; p < PATH_MAX_PROJECTS; p++)
    {
        if (!pfstates[p])
            continue;
        if (!(pfstates[p]->idflags & voices))
            continue;
        if (!(pfstates[p]->idflags & projects))
            continue;

        for (t = 0; t < PATH_MAX_TRACKS; t++)
        {
            if ((tracks >> t) & 1)
                return pfstates[p]->track[t];
        }
    }

    return 0;
}


void PATHI_getmastertrack()
{
    PATHTRACK *track;
    int t;
    char trackplaying;

    Path::pfstate->masterlatency = 0;
    Path::pfstate->mastertrack = -1;
    Path::pfstate->mastersection = 0;

    for (t = 0; t < PATH_MAX_TRACKS; t++)
    {
        track = Path::pfstate->track[t];
        if (!track || track->paused)
            continue;

        trackplaying = (track->node >= 0 && track->entryinfo != 0);

        if (!trackplaying || Path::pfstate->masterlatency >= track->latency)
            continue;

        Path::pfstate->masterlatency = track->latency;
        Path::pfstate->mastertrack = t;
        Path::pfstate->mastersection = track->entryinfo->sectionID;
    }
}

#undef Path::pfstate

int PATH_numtracks(unsigned int projects)
{
    int numtracks = 0;

    {
        int p;

        for (p = 0; p < PATH_MAX_PROJECTS; p++)
        {
            if (!(projects & PATH_PROJECT(p)))
                continue;
            if (!pfstates[p])
                continue;

            {
                int t;

                for (t = 0; t < PATH_MAX_TRACKS; t++)
                {
                    if (pfstates[p]->track[t])
                        numtracks++;
                }
            }
        }
    }

    return numtracks;
}

void PATHI_mainvoice(PATHTRACK *track, int mainvoice)
{
    int trackID = track->trackID;
    int foundmain = (mainvoice > 0);

    track->mainvoice = (mainvoice != 0);

    {
        int p;

        for (p = 0; p < PATH_MAX_PROJECTS; p++)
        {
            if (!pfstates[p])
                continue;

            {
                PATHTRACK *thistrack = pfstates[p]->track[trackID];

                if (!thistrack)
                    continue;
                if (thistrack == track)
                    continue;

                thistrack->mainvoice = !foundmain;
                foundmain = 1;
            }
        }
    }

    if (!foundmain)
        track->mainvoice = 1;
}

void PATHI_statusall(int clear)
{
    {
        int p;

        for (p = 0; p < PATH_MAX_PROJECTS; p++)
        {
            if (!pfstates[p])
                continue;
            if (!PATHI_switchproject(p, PATH_ALL))
                continue;

            {
                int t;

                for (t = 0; t < PATH_MAX_TRACKS; t++)
                {
                    PATHTRACK *track = pfstate->track[t];

                    if (!track)
                        continue;
                    if (!track->trackimp)
                        continue;

                    if (clear)
                        track->trackimp->UpdateStatus();
                    else
                        track->trackimp->CheckStatus();
                }
            }
        }
    }
}

namespace Path {

IPathTrack::IPathTrack()
{
    mHandle = 0;
    mStatusCode = 0;
    mFXSendLevel = 0;
    mDryLevel = 127;
    mVolume = 127;
    mMaxRequests = 1;
    mActiveRequests = 0;
    mTrackInfo = 0;
    mPlayOpts = 0;
    mSubBanks = 0;
    mMaxSubBanks = 0;
    mUpdateStatus = 1;
    mStatus.timeremaining = -1;
}

IPathTrack::~IPathTrack()
{
}

int IPathTrack::GetNumSubBanks()
{
    return mTrackInfo ? mTrackInfo->numsubbanks : 0;
}

int IPathTrack::GetMaxSubBanks()
{
    return mMaxSubBanks;
}

PATHSUBBANKSTATUS *IPathTrack::GetSubBankPtr(int subbanknum)
{
    return 0;
}

PATHSUBBANKSTATUS *IPathTrack::GetAvailSubBankPtr()
{
    return 0;
}

int IPathTrack::AddSubBank(int subbanknum, void *pbank)
{
    return PATHERR_INV_PARAM;
}

int IPathTrack::AddSubBankDone(int subbanknum)
{
    return PATHERR_INV_PARAM;
}

int IPathTrack::DetachSubBankHeader(int subbanknum, int status)
{
    return PATHERR_INV_PARAM;
}

int IPathTrack::RemoveSubBank(int subbanknum)
{
    return PATHERR_INV_PARAM;
}

}; // namespace Path
