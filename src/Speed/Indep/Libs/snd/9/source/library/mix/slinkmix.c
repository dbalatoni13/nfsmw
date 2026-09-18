typedef struct MIXCONFIG MIXCONFIG;

extern "C" {
void MIX_create(MIXCONFIG *pmc);
void MIX_destroy();
void MIX_audioslice(void **poutput, int frames);
void MIX_playinit(int voice, int samplerep, int playtype, void *psample, void *ploopstart, void *ptsdata, int numchan, int totalframes, int sustainstart, int sustainend, void *pcoeftable, void *ploopstate, int platformver, int requester);
void MIX_play(int voice);
void MIX_stop(int voice);
void MIX_setpitch(int voice, int pitch);
void SNDSYS_linkmaincpumixer();
}

extern void (*MIXinitfn)(MIXCONFIG *);
extern void (*MIXrestorefn)();
extern void (*MIXaudioslicefn)(void **, int);
extern void (*MIXplayinitfn)(int, int, int, void *, void *, void *, int, int, int, int, void *, void *, int, int);
extern void (*MIXplayfn)(int);
extern void (*MIXstopfn)(int);
extern void (*MIXsetpitchfn)(int, int);

void SNDSYS_linkmaincpumixer() {
    MIXinitfn = MIX_create;
    MIXrestorefn = MIX_destroy;
    MIXaudioslicefn = MIX_audioslice;
    MIXplayinitfn = MIX_playinit;
    MIXplayfn = MIX_play;
    MIXstopfn = MIX_stop;
    MIXsetpitchfn = MIX_setpitch;
}
