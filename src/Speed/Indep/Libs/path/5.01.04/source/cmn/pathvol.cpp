#include "pathi.h"

using namespace Path;

int PATH_volume(int tracks, signed char scale)
{
    int result;

    if (!PATHI_lock())
        return PATHERR_INUSE;

    result = PATHERR_INV_PARAM;

    if (scale > 100)
        scale = 100;
    if (scale < 0)
        scale = 0;

    if (tracks == PATH_ALL)
        volscale = scale;

    for (int p = 0; p < PATH_MAX_PROJECTS; p++)
    {
        if (PATHI_switchproject(p, tracks))
        {
            for (int t = 0; t < PATH_MAX_TRACKS; t++)
            {
                PATHTRACK *track = pfstate->track[t];
                if (track == 0)
                    continue;
                if (!(((unsigned int)tracks >> t) & 1))
                    continue;
                {
                    track->volscale = scale;
                    if (track->volumefade.fadeto < 0)
                    {
                        track->trackimp->SetVolume(track->volume * scale / 100);
                    }
                    result = PATH_OK;
                }
            }
        }
    }
    PATHI_unlock();

    return result;
}

void PATHI_volume(PATHTRACK *track, signed char volume)
{
    track->volume = volume;
    track->trackimp->SetVolume(volume * track->volscale / 100);
    track->volumefade.fadefrom = -1;
    track->volumefade.fadeto = -1;
    track->volumefade.fadenum = -1;
    track->volumefade.fadestart = 0;
}

void PATHI_fade(PATHTRACK *track, int fadeto, int fadetime, int fadenum)
{
    float from;
    float to;

    track->volumefade.fadestart = milliseconds;
    track->volumefade.fadenum = fadenum;
    track->volumefade.fadetime = fadetime;

    from = track->trackimp->GetVolume();
    track->volumefade.fadefrom = from;
    track->volumefade.fadeto = (signed char)fadeto;

    to = fadeto;
    track->volume = to;
}

void PATHI_customsfxfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum)
{
    track->sfxsendfade.fadestart = milliseconds;
    track->sfxsendfade.fadetime = fadetime;
    track->sfxsendfade.fadenum = fadenum;
    track->sfxsendfade.fadefrom = track->trackimp->GetFXSendLevel(0);
    track->sfxsendfade.fadeto = (signed char)fadeto;
}

void PATHI_customdrylevelfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum)
{
    track->drylevelfade.fadestart = milliseconds;
    track->drylevelfade.fadetime = fadetime;
    track->drylevelfade.fadenum = fadenum;
    track->drylevelfade.fadefrom = track->trackimp->GetDryLevel();
    track->drylevelfade.fadeto = (signed char)fadeto;
}

void PATHI_custompitchfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum)
{
    track->pitchfade.fadestart = milliseconds;
    track->pitchfade.fadetime = fadetime;
    track->pitchfade.fadenum = fadenum;
    track->pitchfade.fadefrom = track->trackimp->GetPitchMult();
    track->pitchfade.fadeto = fadeto;
    if (fadeto < 0 || fadeto > 0x3FFF)
    {
        if (fadeto < 0)
            track->pitchfade.fadeto = 0;
        if (fadeto > 0x3FFF)
            track->pitchfade.fadeto = 0x4000;
    }
}

void PATHI_customstretchfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum)
{
    track->stretchfade.fadestart = milliseconds;
    track->stretchfade.fadetime = fadetime;
    track->stretchfade.fadenum = fadenum;
    track->stretchfade.fadefrom = track->trackimp->GetStretchMult();
    track->stretchfade.fadeto = fadeto;
    if (fadeto < 0x800 || fadeto > 0x1FFF)
    {
        if (fadeto < 0x800)
            track->stretchfade.fadeto = 0x800;
        if (fadeto > 0x1FFF)
            track->stretchfade.fadeto = 0x2000;
    }
}

void PATHI_setfadevolume(PATHTRACK *track)
{
    PATHFADEINFO fadeinfo;
    float pct;
    float range;
    float vol;
    float lowvol;

    range = (float)track->volumefade.fadeto - (float)track->volumefade.fadefrom;
    vol = -1.0f;

    fadeinfo.ms = track->volumefade.fadetime;
    fadeinfo.id = track->volumefade.fadenum;
    fadeinfo.flip = (range >= 0.0f);

    pct = (float)(milliseconds - track->volumefade.fadestart) / (float)fadeinfo.ms;

    if (pct >= 1.0f)
    {
        track->volume = track->volumefade.fadeto;
        track->volumefade.fadefrom = -1;
        track->volumefade.fadeto = -1;
        track->volumefade.fadenum = -1;
        track->volumefade.fadestart = 0;
        track->trackimp->SetVolume(track->volume * track->volscale / 100);
        return;
    }

    if (fadeinfo.flip)
        pct = 1.0f - pct;

    switch (fadeinfo.id)
    {
    case PATH_FADE_EQPOWER:
        pct = pct * pct;
        vol = 1.0f - pct;
        break;
    case PATH_FADE_LINEAR:
        vol = 1.0f - pct;
        break;
    case PATH_FADE_EXPONENTIAL:
        vol = (1.0f / pct) * 0.04f;
        break;
    }

    lowvol = track->volumefade.fadefrom;
    if (track->volumefade.fadeto < track->volumefade.fadefrom)
        lowvol = track->volumefade.fadeto;

    if (range < 0.0f)
        range = -range;

    if (vol >= 0.0f && vol <= 1.0f)
    {
        vol = range * vol + lowvol;
        track->trackimp->SetVolume(vol * track->volscale * 0.01f);
    }
}

void PATHI_setsfxfadevolume(PATHTRACK *track)
{
    PATHFADEINFO fadeinfo;
    float pct;
    float range;
    float vol;
    float lowvol;

    range = (float)track->sfxsendfade.fadeto - (float)track->sfxsendfade.fadefrom;
    vol = -1.0f;

    fadeinfo.ms = track->sfxsendfade.fadetime;
    fadeinfo.id = track->sfxsendfade.fadenum;
    fadeinfo.flip = (range >= 0.0f);

    pct = (float)(milliseconds - track->sfxsendfade.fadestart) / (float)fadeinfo.ms;

    if (pct >= 1.0f)
    {
        track->trackimp->SetFXSendLevel(track->sfxbus, track->sfxsendfade.fadeto);
        track->sfxsendfade.fadefrom = -1;
        track->sfxsendfade.fadeto = -1;
        track->sfxsendfade.fadenum = -1;
        track->sfxsendfade.fadestart = 0;
        return;
    }

    if (fadeinfo.flip)
        pct = 1.0f - pct;

    switch (fadeinfo.id)
    {
    case PATH_FADE_EQPOWER:
        pct = pct * pct;
        vol = 1.0f - pct;
        break;
    case PATH_FADE_LINEAR:
        vol = 1.0f - pct;
        break;
    case PATH_FADE_EXPONENTIAL:
        vol = (1.0f / pct) * 0.04f;
        break;
    }

    lowvol = track->sfxsendfade.fadefrom;
    if (track->sfxsendfade.fadeto < track->sfxsendfade.fadefrom)
        lowvol = track->sfxsendfade.fadeto;

    if (range < 0.0f)
        range = -range;

    if (vol >= 0.0f && vol <= 1.0f)
    {
        vol = range * vol + lowvol;
        track->trackimp->SetFXSendLevel(track->sfxbus, vol);
    }
}

void PATHI_setdrylevelfadevolume(PATHTRACK *track)
{
    PATHFADEINFO fadeinfo;
    float pct;
    float range;
    float vol;
    float lowvol;

    range = (float)track->drylevelfade.fadeto - (float)track->drylevelfade.fadefrom;
    vol = -1.0f;

    fadeinfo.ms = track->drylevelfade.fadetime;
    fadeinfo.id = track->drylevelfade.fadenum;
    fadeinfo.flip = (range >= 0.0f);

    pct = (float)(milliseconds - track->drylevelfade.fadestart) / (float)fadeinfo.ms;

    if (pct >= 1.0f)
    {
        track->trackimp->SetDryLevel(track->drylevelfade.fadeto);
        track->drylevelfade.fadefrom = -1;
        track->drylevelfade.fadeto = -1;
        track->drylevelfade.fadenum = -1;
        track->drylevelfade.fadestart = 0;
        return;
    }

    if (fadeinfo.flip)
        pct = 1.0f - pct;

    switch (fadeinfo.id)
    {
    case PATH_FADE_EQPOWER:
        pct = pct * pct;
        vol = 1.0f - pct;
        break;
    case PATH_FADE_LINEAR:
        vol = 1.0f - pct;
        break;
    case PATH_FADE_EXPONENTIAL:
        vol = (1.0f / pct) * 0.04f;
        break;
    }

    lowvol = track->drylevelfade.fadefrom;
    if (track->drylevelfade.fadeto < track->drylevelfade.fadefrom)
        lowvol = track->drylevelfade.fadeto;

    if (range < 0.0f)
        range = -range;

    if (vol >= 0.0f && vol <= 1.0f)
    {
        vol = range * vol + lowvol;
        track->trackimp->SetDryLevel(vol);
    }
}

void PATHI_setpitchfadevolume(PATHTRACK *track)
{
    PATHFADEINFO fadeinfo;
    float pct;
    float range;
    float vol;
    float lowvol;

    range = (float)track->pitchfade.fadeto - (float)track->pitchfade.fadefrom;
    vol = -1.0f;

    fadeinfo.ms = track->pitchfade.fadetime;
    fadeinfo.id = track->pitchfade.fadenum;
    fadeinfo.flip = (range >= 0.0f);

    pct = (float)(milliseconds - track->pitchfade.fadestart) / (float)fadeinfo.ms;

    if (pct >= 1.0f)
    {
        track->trackimp->SetPitchMult(track->pitchfade.fadeto);
        track->pitchfade.fadefrom = -1;
        track->pitchfade.fadeto = -1;
        track->pitchfade.fadenum = -1;
        track->pitchfade.fadestart = 0;
        return;
    }

    if (fadeinfo.flip)
        pct = 1.0f - pct;

    switch (fadeinfo.id)
    {
    case PATH_FADE_EQPOWER:
        pct = pct * pct;
        vol = 1.0f - pct;
        break;
    case PATH_FADE_LINEAR:
        vol = 1.0f - pct;
        break;
    case PATH_FADE_EXPONENTIAL:
        vol = (1.0f / pct) * 0.04f;
        break;
    }

    lowvol = track->pitchfade.fadefrom;
    if (track->pitchfade.fadeto < track->pitchfade.fadefrom)
        lowvol = track->pitchfade.fadeto;

    if (range < 0.0f)
        range = -range;

    if (vol >= 0.0f && vol <= 1.0f)
    {
        vol = range * vol + lowvol;
        track->trackimp->SetPitchMult(vol);
    }
}

void PATHI_setstretchfadevolume(PATHTRACK *track)
{
    PATHFADEINFO fadeinfo;
    float pct;
    float range;
    float vol;
    float lowvol;

    range = (float)track->stretchfade.fadeto - (float)track->stretchfade.fadefrom;
    vol = -1.0f;

    fadeinfo.ms = track->stretchfade.fadetime;
    fadeinfo.id = track->stretchfade.fadenum;
    fadeinfo.flip = (range >= 0.0f);

    pct = (float)(milliseconds - track->stretchfade.fadestart) / (float)fadeinfo.ms;

    if (pct >= 1.0f)
    {
        track->trackimp->SetStretchMult(track->stretchfade.fadeto);
        track->stretchfade.fadefrom = -1;
        track->stretchfade.fadeto = -1;
        track->stretchfade.fadenum = -1;
        track->stretchfade.fadestart = 0;
        return;
    }

    if (fadeinfo.flip)
        pct = 1.0f - pct;

    switch (fadeinfo.id)
    {
    case PATH_FADE_EQPOWER:
        pct = pct * pct;
        vol = 1.0f - pct;
        break;
    case PATH_FADE_LINEAR:
        vol = 1.0f - pct;
        break;
    case PATH_FADE_EXPONENTIAL:
        vol = (1.0f / pct) * 0.04f;
        break;
    }

    lowvol = track->stretchfade.fadefrom;
    if (track->stretchfade.fadeto < track->stretchfade.fadefrom)
        lowvol = track->stretchfade.fadeto;

    if (range < 0.0f)
        range = -range;

    if (vol >= 0.0f && vol <= 1.0f)
    {
        vol = range * vol + lowvol;
        track->trackimp->SetStretchMult(vol);
    }
}
