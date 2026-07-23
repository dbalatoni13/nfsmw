//
//
//
//
//
//
#ifndef SNDO_H
#define SNDO_H 1 // Decl: 8

#define SND_PLAYLOC_MAINCPU 4 // Decl: 32
#define SND_PLAYLOC_DEFAULT 0 // Decl: 33

#define SND_DEFAULT -1    // Decl: 37
#define SND_ALL_BANKS -1  // Decl: 38
#define SND_AUTO_STOP -1  // Decl: 39
#define SND_HOLD -1       // Decl: 40
#define SND_CANCEL_HOLD 0 // Decl: 41

#define SND_NEEDED 8 // Decl: 45
#define SND_OK 0     // Decl: 46

#define SNDERR_GENERAL -1         // Decl: 50
#define SNDERR_HANDLE_EXPIRED -2  // Decl: 51
#define SNDERR_UNUSED -3          // Decl: 52
#define SNDERR_MEM -6             // Decl: 53
#define SNDERR_FORMAT -7          // Decl: 54
#define SNDERR_INV_PARAM -8       // Decl: 55
#define SNDERR_ALLOCATE -9        // Decl: 56
#define SNDERR_NO_DRIVER -10      // Decl: 57
#define SNDERR_VER -11            // Decl: 58
#define SNDERR_MODE -12           // Decl: 59
#define SNDERR_TOO_MANY -13       // Decl: 60
#define SNDERR_NOT_INITED -14     // Decl: 61
#define SNDERR_UNSUPPORTED -15    // Decl: 62
#define SNDERR_OUT_OF_ORDER -16   // Decl: 63
#define SNDERR_TIMEOUT -17        // Decl: 64
#define SNDERR_INUSE -18          // Decl: 65
#define SNDERR_FILE_NOT_FOUND -19 // Decl: 66

#define SND_REQUEST_QUEUED 0   // Decl: 70
#define SND_REQUEST_READ 1     // Decl: 71
#define SND_REQUEST_PLAYING 2  // Decl: 72
#define SND_REQUEST_DEQUEUED 3 // Decl: 73

#define SND_SR_SIGN16LIT_INT 0 // Decl: 78

#define SND_SR_SIGN16BIG_INT 1 // Decl: 82

#define SND_SR_EAXA_INT 3 // Decl: 86

#define SND_SR_MT10_BLK 4 // Decl: 90

#define SND_SR_SIGN16BIG_BLK 7 // Decl: 94

#define SND_SR_SIGN16LIT_BLK 8 // Decl: 98

#define SND_SR_SIGN8_BLK 9 // Decl: 102

#define SND_SR_EAXA_BLK 10 // Decl: 106

#define SND_SR_LAYER3 16 // Decl: 110

#define SND_SR_SIGN24LIT_INT 19 // Decl: 114

#define SND_SR_MT5_BLK 22 // Decl: 118

#define SND_SR_EALAYER3 23 // Decl: 122

#define SND_UD_BANK_LOADED 0   // Decl: 128
#define SND_UD_BANK_PLAY 1     // Decl: 129
#define SND_UD_BANK_UNLOADED 2 // Decl: 130
#define SND_UD_STREAM_LOADED 3 // Decl: 131

#define SND_MAX_USER_DATA 4 // Decl: 135

#define SND_MAX_MULTI_CHAN 6 // Decl: 138

#define SNDCALL // Decl: 143

#ifdef __cplusplus
extern "C" {
#endif

// total size: 0x18
// Decl: 149
typedef struct SNDPLAYOPTS {
    signed char vol;               // offset 0x0, size 0x1
    signed char bend;              // offset 0x1, size 0x1
    signed char keynum;            // offset 0x2, size 0x1
    signed char velocity;          // offset 0x3, size 0x1
    signed char drylevel;          // offset 0x4, size 0x1
    signed char fxlevel0;          // offset 0x5, size 0x1
    char pad[2];                   // offset 0x6, size 0x2
    unsigned short azimuth;        // offset 0x8, size 0x2
    short elevation;               // offset 0xA, size 0x2
    unsigned short pitchmult;      // offset 0xC, size 0x2
    unsigned short timemult;       // offset 0xE, size 0x2
    unsigned short tempomult;      // offset 0x10, size 0x2
    unsigned short pad2;           // offset 0x12, size 0x2
    unsigned short lowpasscutoff;  // offset 0x14, size 0x2
    unsigned short highpasscutoff; // offset 0x16, size 0x2
} SNDPLAYOPTS;

// total size: 0x10
// Decl: 169
typedef struct SNDREQUESTSTATUS {
    int state;        // offset 0x0, size 0x4, Decl: 170
    int currenttime;  // offset 0x4, size 0x4, Decl: 171
    int timetoend;    // offset 0x8, size 0x4, Decl: 172
    int timebuffered; // offset 0xC, size 0x4, Decl: 173
} SNDREQUESTSTATUS;

// total size: 0xC
// Decl: 179
typedef struct SNDSTREAMSTATUS {
    int outstandingrequests; // offset 0x0, size 0x4, Decl: 180
    int currentrequest;      // offset 0x4, size 0x4, Decl: 181
    int timebuffered;        // offset 0x8, size 0x4, Decl: 182
} SNDSTREAMSTATUS;

// total size: 0x68
// Decl: 188
typedef struct SNDSAMPLEATTR {
    short detune;                        // offset 0x0, size 0x2
    signed char priority;                // offset 0x2, size 0x1
    signed char vol;                     // offset 0x3, size 0x1
    signed char pan;                     // offset 0x4, size 0x1
    signed char fxlevel0;                // offset 0x5, size 0x1
    signed char bendrange;               // offset 0x6, size 0x1
    unsigned char platformver;           // offset 0x7, size 0x1
    unsigned short rendermode;           // offset 0x8, size 0x2
    char padchar[2];                     // offset 0xA, size 0x2
    unsigned short azimuth[6];           // offset 0xC, size 0xC
    void *ptsdata[6];                    // offset 0x18, size 0x18
    int tsdatasize[6];                   // offset 0x30, size 0x18
    void *puserdata[SND_MAX_USER_DATA];  // offset 0x48, size 0x10
    int userdatasize[SND_MAX_USER_DATA]; // offset 0x58, size 0x10
} SNDSAMPLEATTR;

// total size: 0x4
// Decl: 210
typedef struct SNDSAMPLEFORMAT {
    unsigned short samplerate; // offset 0x0, size 0x2
    unsigned char channels;    // offset 0x2, size 0x1
    unsigned char samplerep;   // offset 0x3, size 0x1
} SNDSAMPLEFORMAT;

// total size: 0x1C
// Decl: 221
typedef struct SNDSAMPLEDESC {
    unsigned int totalframes; // offset 0x0, size 0x4
    void *psamples[6];        // offset 0x4, size 0x18
} SNDSAMPLEDESC;

// total size: 0x24
// Decl: 229
typedef struct SNDPACKET {
    SNDSAMPLEFORMAT ssf;           // offset 0x0, size 0x4, Decl: 230
    unsigned int numframes : 31;   // offset 0x4, size 0x4
    unsigned int continuation : 1; // offset 0x4, size 0x4
    int sizeofsamples;             // offset 0x8, size 0x4, Decl: 233
    void *psamples[6];             // offset 0xC, size 0x18, Decl: 234
} SNDPACKET;

// total size: 0x14
// Decl: 240
typedef struct SNDUSERDATACBINFO {
    int type;           // offset 0x0, size 0x4
    void *pdata;        // offset 0x4, size 0x4
    int size;           // offset 0x8, size 0x4
    int shandle;        // offset 0xC, size 0x4
    int sndstrmrequest; // offset 0x10, size 0x4
} SNDUSERDATACBINFO;

// total size: 0x18
// Decl: 259
typedef struct SNDFILTERDEF {
    int id;                                          // offset 0x0, size 0x4
    int statesize;                                   // offset 0x4, size 0x4
    int priority;                                    // offset 0x8, size 0x4
    void (*filterinit)(void *, int, int);            // offset 0xC, size 0x4
    int (*filter)(void *, int, void *, void *, int); // offset 0x10, size 0x4
    void (*filterrestore)(void *);                   // offset 0x14, size 0x4
} SNDFILTERDEF;

int SND3dpos(int shandle, int azimuth, int elevation);

int SND_attrsetdef(SNDSAMPLEATTR *pssa);

int SNDBANK_patchinfo(int bhandle, int patnum, SNDSAMPLEFORMAT *pssf, SNDSAMPLEATTR *pssa, SNDSAMPLEDESC *pssd);

int SNDCTRL_getprogvol(int shandle);

int SNDplaysetdef(SNDPLAYOPTS *pspo);

int SNDPKTPLAY_create(void (*preleasefunc)(void *, void *), void (*pframesfunc)(int, int, void *), void *pclientdata, void *pmem, int memsize);
int SNDPKTPLAY_overhead(int maxpackets);
int SNDPKTPLAY_start(int packetinstancehandle, SNDSAMPLEFORMAT *pssf, SNDSAMPLEATTR *pssa, SNDPLAYOPTS *pspo);
int SNDPKTPLAY_stop(int packetinstancehandle);
int SNDPKTPLAY_submit(int packetinstancehandle, SNDPACKET *psp);
int SNDPKTPLAY_destroy(int packetinstancehandle);

int SNDSTRM_getprogvol(int sndstreamhandle);

void SNDSYS_entercritical();
void SNDSYS_leavecritical();

int SNDtimeremaining(int shandle);

#ifdef __cplusplus
}
#endif

#endif
