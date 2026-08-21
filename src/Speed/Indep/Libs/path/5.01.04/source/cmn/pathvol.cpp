#include "pathi.h"

int PATH_volume(int tracks, signed char scale) {
    int result;

    if (PATHI_lock() == 0) {
        return PATHERR_INUSE;
    }
    result = PATHERR_INV_PARAM;
    if (scale > 100) {
        scale = 100;
    }
    if (scale < 0) {
        scale = 0;
    }
    if (tracks == -1) {
        Path::volscale = scale;
    }
    {
        int p;

        for (p = 0; p < PATH_MAX_PROJECTS; p++) {
            if (PATHI_switchproject(p, tracks) != 0) {
                int t;

                for (t = 0; t < PATH_MAX_TRACKS; t++) {
                    PATHTRACK *track = Path::pfstate->track[t];

                    if (track != 0 && ((static_cast<unsigned int>(tracks) >> t) & 1) == 1) {
                        track->volscale = scale;
                        if (track->volumefade.fadeto < 0) {
                            track->trackimp->SetVolume(track->volume * scale / 100);
                        }
                        result = PATH_OK;
                    }
                }
            }
        }
    }
    PATHI_unlock();
    return result;
}

void PATHI_volume(PATHTRACK *track, signed char volume) {
    track->volume = volume;
    track->trackimp->SetVolume(volume * track->volscale / 100);
    track->volumefade.fadestart = 0;
    track->volumefade.fadeto = track->volumefade.fadefrom = track->volumefade.fadenum = -1;
}

void PATHI_fade(PATHTRACK *track, int fadeto, int fadetime, int fadenum) {
    track->volumefade.fadenum = fadenum;
    track->volumefade.fadestart = Path::milliseconds;
    track->volumefade.fadetime = fadetime;
    track->volumefade.fadefrom = track->trackimp->GetVolume();
    track->volumefade.fadeto = static_cast<signed char>(fadeto);
    track->volume = fadeto;
}

void PATHI_customsfxfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum) {
    track->sfxsendfade.fadetime = fadetime;
    track->sfxsendfade.fadestart = Path::milliseconds;
    track->sfxsendfade.fadenum = fadenum;
    track->sfxsendfade.fadefrom = track->trackimp->GetFXSendLevel(0);
    track->sfxsendfade.fadeto = static_cast<signed char>(fadeto);
}

void PATHI_customdrylevelfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum) {
    track->drylevelfade.fadetime = fadetime;
    track->drylevelfade.fadestart = Path::milliseconds;
    track->drylevelfade.fadenum = fadenum;
    track->drylevelfade.fadefrom = track->trackimp->GetDryLevel();
    track->drylevelfade.fadeto = static_cast<signed char>(fadeto);
}

void PATHI_custompitchfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum) {
    track->pitchfade.fadetime = fadetime;
    track->pitchfade.fadestart = Path::milliseconds;
    track->pitchfade.fadenum = fadenum;
    track->pitchfade.fadefrom = track->trackimp->GetPitchMult();
    track->pitchfade.fadeto = fadeto;
    if (fadeto < 0 || fadeto > 0x3fff) {
        if (fadeto < 0) {
            track->pitchfade.fadeto = 0;
        }
        if (fadeto > 0x3fff) {
            track->pitchfade.fadeto = 0x4000;
        }
    }
}

void PATHI_customstretchfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum) {
    track->stretchfade.fadetime = fadetime;
    track->stretchfade.fadestart = Path::milliseconds;
    track->stretchfade.fadenum = fadenum;
    track->stretchfade.fadefrom = track->trackimp->GetStretchMult();
    track->stretchfade.fadeto = fadeto;
    if (fadeto < 0x800 || fadeto > 0x1fff) {
        if (fadeto < 0x800) {
            track->stretchfade.fadeto = 0x800;
        }
        if (fadeto > 0x1fff) {
            track->stretchfade.fadeto = 0x2000;
        }
    }
}

void PATHI_setfadevolume(PATHTRACK *track) {
    PATHFADEINFO fadeinfo;
    float pct;
    float range;
    float vol;
    float lowvol;

    range = static_cast<float>(track->volumefade.fadeto) - static_cast<float>(track->volumefade.fadefrom);
    fadeinfo.ms = track->volumefade.fadetime;
    fadeinfo.id = track->volumefade.fadenum;
    fadeinfo.flip = range >= 0.0f;
    pct = static_cast<float>(Path::milliseconds - track->volumefade.fadestart) / fadeinfo.ms;
    if (pct >= 1.0f) {
        track->volume = track->volumefade.fadeto;
        track->volumefade.fadenum = -1;
        track->volumefade.fadestart = 0;
        track->volumefade.fadefrom = -1;
        track->volumefade.fadeto = -1;
        track->trackimp->SetVolume(track->volume * track->volscale / 100);
        return;
    }
    if (fadeinfo.flip != 0) {
        pct = 1.0f - pct;
    }
    vol = 0.0f;
    switch (fadeinfo.id) {
    case 2:
        pct *= pct;
    case 1:
        vol = 1.0f - pct;
        break;
    case 3:
        vol = 1.0f / pct * 0.01f;
        break;
    }
    lowvol = static_cast<float>(track->volumefade.fadefrom);
    if (track->volumefade.fadeto < track->volumefade.fadefrom) {
        lowvol = static_cast<float>(track->volumefade.fadeto);
    }
    if (range < 0.0f) {
        range = -range;
    }
    if (vol >= 0.0f && vol <= 1.0f) {
        track->trackimp->SetVolume(static_cast<int>((range * vol + lowvol) * track->volscale * 0.01f));
    }
}

void PATHI_setsfxfadevolume(PATHTRACK *track) {
    PATHFADEINFO fadeinfo;
    float pct;
    float range;
    float vol;
    float lowvol;

    range = static_cast<float>(track->sfxsendfade.fadeto) - static_cast<float>(track->sfxsendfade.fadefrom);
    fadeinfo.ms = track->sfxsendfade.fadetime;
    fadeinfo.id = track->sfxsendfade.fadenum;
    fadeinfo.flip = range >= 0.0f;
    pct = static_cast<float>(Path::milliseconds - track->sfxsendfade.fadestart) / fadeinfo.ms;
    if (pct >= 1.0f) {
        track->trackimp->SetFXSendLevel(track->sfxbus, track->sfxsendfade.fadeto);
        track->sfxsendfade.fadestart = 0;
        track->sfxsendfade.fadenum = -1;
        track->sfxsendfade.fadefrom = -1;
        track->sfxsendfade.fadeto = -1;
        return;
    }
    if (fadeinfo.flip != 0) {
        pct = 1.0f - pct;
    }
    vol = 0.0f;
    switch (fadeinfo.id) {
    case 2:
        pct *= pct;
    case 1:
        vol = 1.0f - pct;
        break;
    case 3:
        vol = 1.0f / pct * 0.01f;
        break;
    }
    lowvol = static_cast<float>(track->sfxsendfade.fadefrom);
    if (track->sfxsendfade.fadeto < track->sfxsendfade.fadefrom) {
        lowvol = static_cast<float>(track->sfxsendfade.fadeto);
    }
    if (range < 0.0f) {
        range = -range;
    }
    if (vol >= 0.0f && vol <= 1.0f) {
        track->trackimp->SetFXSendLevel(track->sfxbus, static_cast<int>(range * vol + lowvol));
    }
}

void PATHI_setdrylevelfadevolume(PATHTRACK *track) {
    PATHFADEINFO fadeinfo;
    float pct;
    float range;
    float vol;
    float lowvol;

    range = static_cast<float>(track->drylevelfade.fadeto) - static_cast<float>(track->drylevelfade.fadefrom);
    fadeinfo.ms = track->drylevelfade.fadetime;
    fadeinfo.id = track->drylevelfade.fadenum;
    fadeinfo.flip = range >= 0.0f;
    pct = static_cast<float>(Path::milliseconds - track->drylevelfade.fadestart) / fadeinfo.ms;
    if (pct >= 1.0f) {
        track->trackimp->SetDryLevel(track->drylevelfade.fadeto);
        track->drylevelfade.fadestart = 0;
        track->drylevelfade.fadenum = -1;
        track->drylevelfade.fadefrom = -1;
        track->drylevelfade.fadeto = -1;
        return;
    }
    if (fadeinfo.flip != 0) {
        pct = 1.0f - pct;
    }
    vol = 0.0f;
    switch (fadeinfo.id) {
    case 2:
        pct *= pct;
    case 1:
        vol = 1.0f - pct;
        break;
    case 3:
        vol = 1.0f / pct * 0.01f;
        break;
    }
    lowvol = static_cast<float>(track->drylevelfade.fadefrom);
    if (track->drylevelfade.fadeto < track->drylevelfade.fadefrom) {
        lowvol = static_cast<float>(track->drylevelfade.fadeto);
    }
    if (range < 0.0f) {
        range = -range;
    }
    if (vol >= 0.0f && vol <= 1.0f) {
        track->trackimp->SetDryLevel(static_cast<int>(range * vol + lowvol));
    }
}

void PATHI_setpitchfadevolume(PATHTRACK *track) {
    PATHFADEINFO fadeinfo;
    float pct;
    float range;
    float vol;
    float lowvol;

    range = static_cast<float>(track->pitchfade.fadeto) - static_cast<float>(track->pitchfade.fadefrom);
    fadeinfo.ms = track->pitchfade.fadetime;
    fadeinfo.id = track->pitchfade.fadenum;
    fadeinfo.flip = range >= 0.0f;
    pct = static_cast<float>(Path::milliseconds - track->pitchfade.fadestart) / fadeinfo.ms;
    if (pct >= 1.0f) {
        track->trackimp->SetPitchMult(track->pitchfade.fadeto);
        track->pitchfade.fadestart = 0;
        track->pitchfade.fadenum = -1;
        track->pitchfade.fadefrom = -1;
        track->pitchfade.fadeto = -1;
        return;
    }
    if (fadeinfo.flip != 0) {
        pct = 1.0f - pct;
    }
    vol = 0.0f;
    switch (fadeinfo.id) {
    case 2:
        pct *= pct;
    case 1:
        vol = 1.0f - pct;
        break;
    case 3:
        vol = 1.0f / pct * 0.01f;
        break;
    }
    lowvol = static_cast<float>(track->pitchfade.fadefrom);
    if (track->pitchfade.fadeto < track->pitchfade.fadefrom) {
        lowvol = static_cast<float>(track->pitchfade.fadeto);
    }
    if (range < 0.0f) {
        range = -range;
    }
    if (vol >= 0.0f && vol <= 1.0f) {
        track->trackimp->SetPitchMult(static_cast<int>(range * vol + lowvol));
    }
}

void PATHI_setstretchfadevolume(PATHTRACK *track) {
    PATHFADEINFO fadeinfo;
    float pct;
    float range;
    float vol;
    float lowvol;

    range = static_cast<float>(track->stretchfade.fadeto) - static_cast<float>(track->stretchfade.fadefrom);
    fadeinfo.ms = track->stretchfade.fadetime;
    fadeinfo.id = track->stretchfade.fadenum;
    fadeinfo.flip = range >= 0.0f;
    pct = static_cast<float>(Path::milliseconds - track->stretchfade.fadestart) / fadeinfo.ms;
    if (pct >= 1.0f) {
        track->trackimp->SetStretchMult(track->stretchfade.fadeto);
        track->stretchfade.fadestart = 0;
        track->stretchfade.fadenum = -1;
        track->stretchfade.fadefrom = -1;
        track->stretchfade.fadeto = -1;
        return;
    }
    if (fadeinfo.flip != 0) {
        pct = 1.0f - pct;
    }
    vol = 0.0f;
    switch (fadeinfo.id) {
    case 2:
        pct *= pct;
    case 1:
        vol = 1.0f - pct;
        break;
    case 3:
        vol = 1.0f / pct * 0.01f;
        break;
    }
    lowvol = static_cast<float>(track->stretchfade.fadefrom);
    if (track->stretchfade.fadeto < track->stretchfade.fadefrom) {
        lowvol = static_cast<float>(track->stretchfade.fadeto);
    }
    if (range < 0.0f) {
        range = -range;
    }
    if (vol >= 0.0f && vol <= 1.0f) {
        track->trackimp->SetStretchMult(static_cast<int>(range * vol + lowvol));
    }
}
