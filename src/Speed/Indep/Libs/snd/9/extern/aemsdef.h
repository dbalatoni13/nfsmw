#ifndef SND_EXTERN_AEMSDEF_H
#define SND_EXTERN_AEMSDEF_H

#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include "Speed/Indep/Libs/csis/dev/include/csis/csis.h"
#include <cstddef>

namespace AemsDef {

// total size: 0x10
struct TIMERCLIENT {
    CListDNode ln; // offset 0x0, size 0x8
    void (* pclientfn)(void *); // offset 0x8, size 0x4
    void * pclientdata; // offset 0xC, size 0x4
};

// static const int MAX_COMPONENTS; // size: 0x4
// static const unsigned char PLATFORM_WIN; // size: 0x1
// static const unsigned char PLATFORM_MAC; // size: 0x1
// static const unsigned char PLATFORM_PS2; // size: 0x1
// static const unsigned char PLATFORM_XBOX; // size: 0x1
// static const unsigned char PLATFORM_GC; // size: 0x1
// static const unsigned char PLATFORM_XENON; // size: 0x1
// static const unsigned char PLATFORM_PSP; // size: 0x1
// static const unsigned char PLATFORM_MAX; // size: 0x1
// static const unsigned char SAMPLEENTRY_PATCH; // size: 0x1
// static const unsigned char SAMPLEENTRY_STREAM; // size: 0x1
// static const unsigned char SAMPLEENTRY_LOOPEDSTREAM; // size: 0x1
// static const unsigned char SAMPLEENTRY_MIDI; // size: 0x1
// static const unsigned char SAMPLEENTRY_MAX; // size: 0x1
// static const unsigned char ENVELOPE_CONTROL_STOP; // size: 0x1
// static const unsigned char ENVELOPE_CONTROL_PLAY; // size: 0x1
// static const unsigned char ENVELOPE_CONTROL_PAUSE; // size: 0x1
// static const unsigned char ENVELOPE_CONTROL_RELEASE; // size: 0x1
// static const unsigned char PLAYER_INPUT_PITCHMULT; // size: 0x1
// static const unsigned char PLAYER_INPUT_TIMEMULT; // size: 0x1
// static const unsigned char PLAYER_INPUT_VOL; // size: 0x1
// static const unsigned char PLAYER_INPUT_AZIMUTH; // size: 0x1
// static const unsigned char PLAYER_INPUT_ELEVATION; // size: 0x1
// static const unsigned char PLAYER_INPUT_FXWET0; // size: 0x1
// static const unsigned char PLAYER_INPUT_LOWPASS; // size: 0x1
// static const unsigned char PLAYER_INPUT_HIGHPASS; // size: 0x1
// static const unsigned char PLAYER_INPUT_DRYLEVEL; // size: 0x1
// static const unsigned char PLAYER_INPUT_MAX; // size: 0x1
// static const unsigned char PLAYER_PLAYCONTROL_STOP; // size: 0x1
// static const unsigned char PLAYER_PLAYCONTROL_PLAY; // size: 0x1
// static const unsigned char PLAYER_PLAYCONTROL_PAUSE; // size: 0x1
// static const unsigned char OSCILLATOR_WAVEFORM_SINE; // size: 0x1
// static const unsigned char OSCILLATOR_WAVEFORM_SQUARE; // size: 0x1
// static const unsigned char OSCILLATOR_WAVEFORM_SAWTOOTH; // size: 0x1
// static const unsigned char OSCILLATOR_WAVEFORM_TRIANGLE; // size: 0x1

enum InterfaceType {
    INTERFACETYPE_GLOBALVARIABLE = 0,
    INTERFACETYPE_CLASS = 1,
    INTERFACETYPE_FUNCTION = 2,
};

// total size: 0x10
// Decl: 311
struct TWEAKHEADER {
    char id[4]; // offset 0x0, size 0x4, Decl: 312
    unsigned char ver; // offset 0x4, size 0x1, Decl: 314
    unsigned char platform; // offset 0x5, size 0x1, Decl: 315
    char pad[2]; // offset 0x6, size 0x2, Decl: 316
    int crc; // offset 0x8, size 0x4, Decl: 318
    int numcomponents; // offset 0xC, size 0x4, Decl: 320
};

// total size: 0x5C
// Decl: 333
struct ModuleBank {
    char id[4]; // offset 0x0, size 0x4, Decl: 334
    unsigned char ver; // offset 0x4, size 0x1, Decl: 338
    unsigned char veraimexmajor; // offset 0x5, size 0x1, Decl: 340
    unsigned char veraimexminor; // offset 0x6, size 0x1, Decl: 341
    unsigned char veraimexpatch; // offset 0x7, size 0x1, Decl: 342
    unsigned char platform; // offset 0x8, size 0x1, Decl: 344
    char pad; // offset 0x9, size 0x1
    unsigned short nummodules; // offset 0xA, size 0x2, Decl: 351
    int debugcrc; // offset 0xC, size 0x4, Decl: 354
    int uniqueid; // offset 0x10, size 0x4, Decl: 357
    int totalsize; // offset 0x14, size 0x4, Decl: 360
    int residentsize; // offset 0x18, size 0x4, Decl: 362
    int moduleoffset; // offset 0x1C, size 0x4, Decl: 364
    int sfxbankoffset; // offset 0x20, size 0x4, Decl: 366
    int sfxbanksizepadded; // offset 0x24, size 0x4, Decl: 367
    int midibankoffset; // offset 0x28, size 0x4, Decl: 369
    int midibanksizepadded; // offset 0x2C, size 0x4, Decl: 370
    int funcfixupoffset; // offset 0x30, size 0x4, Decl: 372
    int staticdatafixupoffset; // offset 0x34, size 0x4, Decl: 374
    int interfaceOffset; // offset 0x38, size 0x4, Decl: 376
    int modulebankhandle; // offset 0x3C, size 0x4, Decl: 379
    int sfxbhandle; // offset 0x40, size 0x4
    int midibhandle; // offset 0x44, size 0x4, Decl: 394
    char *streamfilepath; // offset 0x48, size 0x4, Decl: 402
    int streamfileoffset; // offset 0x4C, size 0x4, Decl: 404
    CListDNode ln; // offset 0x50, size 0x8, Decl: 410
    TWEAKHEADER *ptweakheader; // offset 0x58, size 0x4, Decl: 417
};

typedef struct TIMERCLIENT TIMERCLIENT;

// total size: 0x18
struct MODULEINSTANCE {
    CListDNode ln; // offset 0x0, size 0x8
    TIMERCLIENT timerclient; // offset 0x8, size 0x10
};

// total size: 0x3C
// Decl: 542
class Module {
public:
    int id; // offset 0x0, size 0x4, Decl: 544
    Csis::ClassHandle classHandle; // offset 0x4, size 0x8, Decl: 551
    Csis::ClassConstructorClient constructorClient; // offset 0xC, size 0x10, Decl: 552
    short curinstances; // offset 0x1C, size 0x2, Decl: 555
    short maxinstances; // offset 0x1E, size 0x2, Decl: 556
    unsigned short numGlobals; // offset 0x20, size 0x2, Decl: 559
    unsigned short numFunctions; // offset 0x22, size 0x2, Decl: 560
    unsigned char numPlayers; // offset 0x24, size 0x1, Decl: 562
    unsigned char classDestructorPresent; // offset 0x25, size 0x1, Decl: 563
    unsigned char classDataPresent; // offset 0x26, size 0x1, Decl: 564
    unsigned char numClassControllers; // offset 0x27, size 0x1, Decl: 565
    void (* pcode)(void *); // offset 0x28, size 0x4, Decl: 576
    void *pdata; // offset 0x2C, size 0x4, Decl: 583
    int datasize; // offset 0x30, size 0x4, Decl: 585
    int destroydataoffset; // offset 0x34, size 0x4, Decl: 590
    CListDStack moduleInstance; // offset 0x38, size 0x4, Decl: 593
};

// total size: 0xC
// Decl: 663
struct InterfaceReference {
    int handleOffset; // offset 0x0, size 0x4, Decl: 664
    int IDOffset; // offset 0x4, size 0x4, Decl: 665
    unsigned char type; // offset 0x8, size 0x1, Decl: 666
    char pad[3]; // offset 0x9, size 0x3, Decl: 667
};

// total size: 0x10
// Decl: 671
struct InterfaceFixupHeader {
    int numFixups; // offset 0x0, size 0x4, Decl: 672
    InterfaceReference reference[1]; // offset 0x4, size 0xC, Decl: 673
};

// total size: 0x4
struct PATCHENTRY {
    int patnum; // offset 0x0, size 0x4
};

// total size: 0x4
struct STREAMENTRY {
    int offset; // offset 0x0, size 0x4
};

// total size: 0x8
struct LOOPEDSTREAMENTRY {
    int leadinoffset; // offset 0x0, size 0x4
    int loopoffset; // offset 0x4, size 0x4
};

// total size: 0x4
struct MIDIENTRY {
    int offset; // offset 0x0, size 0x4
};

// total size: 0xC
// Decl: 700
struct SAMPLEENTRY {
    unsigned char type; // offset 0x0, size 0x1, Decl: 702
    unsigned char priority; // offset 0x1, size 0x1, Decl: 703
    char pad[2]; // offset 0x2, size 0x2, Decl: 704
    union {
        struct PATCHENTRY patch; // offset 0x0, size 0x4
        struct STREAMENTRY stream; // offset 0x0, size 0x4
        struct LOOPEDSTREAMENTRY loopedstream; // offset 0x0, size 0x8
        struct MIDIENTRY midi; // offset 0x0, size 0x4
    } u; // offset 0x4, size 0x8
};

// total size: 0x10
struct SAMPLEGROUP {
    int numentries; // offset 0x0, size 0x4
    SAMPLEENTRY entry[1]; // offset 0x4, size 0xC
};

// total size: 0x14
// Decl: 731
struct TABLE {
    unsigned char sizeofentry; // offset 0x0, size 0x1, Decl: 732
    char pad[1]; // offset 0x1, size 0x1, Decl: 733
    unsigned short numentries; // offset 0x2, size 0x2, Decl: 734
    int minvalue; // offset 0x4, size 0x4, Decl: 736
    int maxvalue; // offset 0x8, size 0x4, Decl: 737
    float resolution; // offset 0xC, size 0x4, Decl: 738
    union {
        signed char entry8[4]; // offset 0x0, size 0x4
        short entry16[2]; // offset 0x0, size 0x4
        int entry32[1]; // offset 0x0, size 0x4
    } table; // offset 0x10, size 0x4
};

// total size: 0x4
struct CREATESETTINGS {
    int triggered; // offset 0x0, size 0x4
};

// total size: 0x4
// Decl: 920
struct CREATESTATE {
    CREATESETTINGS settings; // offset 0x0, size 0x4, Decl: 921
};

// total size: 0xC
struct DESTROYSETTINGS {
    Module *pModule; // offset 0x0, size 0x4
    MODULEINSTANCE *pModuleInstance; // offset 0x4, size 0x4
    Csis::Class *pClass; // offset 0x8, size 0x4
};

// total size: 0x10
struct DESTROYSTATE {
    DESTROYSETTINGS settings; // offset 0x0, size 0xC
    int triggered; // offset 0xC, size 0x4
};

// total size: 0x8
struct ParameterClampDesc {
    int minVal; // offset 0x0, size 0x4
    int maxVal; // offset 0x4, size 0x4
};

// total size: 0xC
struct CallFunctionSettings {
    Csis::FunctionHandle functionHandle; // offset 0x0, size 0x8
    unsigned char clampParameters; // offset 0x8, size 0x1
    unsigned char numParameters; // offset 0x9, size 0x1
    char pad[2]; // offset 0xA, size 0x2
};

// total size: 0x8
struct CallFunctionInputs {
    int triggered; // offset 0x0, size 0x4
    int parameter[1]; // offset 0x4, size 0x4
};

// total size: 0xC
struct CallFunctionState {
    CallFunctionSettings settings; // offset 0x0, size 0xC
};

// total size: 0x10
// Decl: 1039
struct ControlClassSettings {
    Csis::ClassHandle classHandle; // offset 0x0, size 0x8, Decl: 1040
    Csis::Class *pClass; // offset 0x8, size 0x4, Decl: 1044
    unsigned char clampParameters; // offset 0xC, size 0x1, Decl: 1046
    unsigned char numParameters; // offset 0xD, size 0x1, Decl: 1047
    char pad[2]; // offset 0xE, size 0x2, Decl: 1048
};

// total size: 0xC
struct ControlClassInputs {
    int constructorTriggered; // offset 0x0, size 0x4
    int destructorTriggered; // offset 0x4, size 0x4
    int parameter[1]; // offset 0x8, size 0x4
};


// total size: 0x10
struct ControlClassState {
    ControlClassSettings settings; // offset 0x0, size 0x10
};

// total size: 0x14
struct SetGlobalVariableSettings {
    Csis::GlobalVariableHandle globalVariableHandle; // offset 0x0, size 0x8
    int minValue; // offset 0x8, size 0x4
    int maxValue; // offset 0xC, size 0x4
    int prevValue; // offset 0x10, size 0x4
};

// total size: 0x4
struct SetGlobalVariableInputs {
    int value; // offset 0x0, size 0x4
};

// total size: 0x18
struct SetGlobalVariableState {
    SetGlobalVariableSettings settings; // offset 0x0, size 0x14
    SetGlobalVariableInputs inputs; // offset 0x14, size 0x4
};

// total size: 0x10
// Decl: 1103
struct COUNTERSETTINGS {
    int minvalue; // offset 0x0, size 0x4, Decl: 1104
    int maxvalue; // offset 0x4, size 0x4, Decl: 1105
    int value; // offset 0x8, size 0x4, Decl: 1108
    signed char direction; // offset 0xC, size 0x1, Decl: 1111
    char pad[3]; // offset 0xD, size 0x3, Decl: 1112
};

// total size: 0x18
// Decl: 1116
struct COUNTERSTATE {
    COUNTERSETTINGS settings; // offset 0x0, size 0x10, Decl: 1117
    int triggered; // offset 0x10, size 0x4, Decl: 1119
    int override; // offset 0x14, size 0x4, Decl: 1120
};

// total size: 0xC
struct RANDOMSETTINGS {
    int minvalue; // offset 0x0, size 0x4
    int range; // offset 0x4, size 0x4
    int outputcurrent; // offset 0x8, size 0x4
};

// total size: 0x10
// Decl: 1136
struct RANDOMSTATE {
    RANDOMSETTINGS settings; // offset 0x0, size 0xC, Decl: 1137
    int triggered; // offset 0xC, size 0x4, Decl: 1139
};

// total size: 0x14
// Decl: 1143
struct RANDOMSHUFFLESETTINGS {
    unsigned short inputoffset; // offset 0x0, size 0x2, Decl: 1147
    unsigned char sizeofnumbersetentry; // offset 0x2, size 0x1, Decl: 1150
    signed char avoidrepeat; // offset 0x3, size 0x1, Decl: 1153
    int minvalue; // offset 0x4, size 0x4, Decl: 1156
    unsigned short numbersetindex; // offset 0x8, size 0x2, Decl: 1159
    unsigned short range; // offset 0xA, size 0x2, Decl: 1160
    int outputcurrent; // offset 0xC, size 0x4, Decl: 1163
    union {
        unsigned char setu8[4]; // offset 0x0, size 0x4
        unsigned short setu16[2]; // offset 0x0, size 0x4
    } numberset; // offset 0x10, size 0x4
};

// total size: 0x4
struct RANDOMSHUFFLEINPUTS {
    int triggered; // offset 0x0, size 0x4
};

// total size: 0x14
// Decl: 1183
struct RANDOMSHUFFLESTATE {
    RANDOMSHUFFLESETTINGS settings; // offset 0x0, size 0x14, Decl: 1184
};

// total size: 0x10
struct RANDOMWEIGHTEDSETTINGS {
    TABLE * ptable; // offset 0x0, size 0x4
    int minvalue; // offset 0x4, size 0x4
    int range; // offset 0x8, size 0x4
    int outputcurrent; // offset 0xC, size 0x4
};

// total size: 0x14
struct RANDOMWEIGHTEDSTATE {
    RANDOMWEIGHTEDSETTINGS settings; // offset 0x0, size 0x10
    int triggered; // offset 0x10, size 0x4
};

// total size: 0x14
// Decl: 1241
struct RANGETRIGSETTINGS {
    int tripgte; // offset 0x0, size 0x4, Decl: 1242
    int triplte; // offset 0x4, size 0x4, Decl: 1243
    int resetgte; // offset 0x8, size 0x4, Decl: 1245
    int resetlte; // offset 0xC, size 0x4, Decl: 1246
    signed char hastripped; // offset 0x10, size 0x1, Decl: 1248
    signed char output; // offset 0x11, size 0x1, Decl: 1249
    char pad[2]; // offset 0x12, size 0x2, Decl: 1250
};

// total size: 0x18
// Decl: 1254
struct RANGETRIGSTATE {
    RANGETRIGSETTINGS settings; // offset 0x0, size 0x14, Decl: 1255
    int input; // offset 0x14, size 0x4, Decl: 1257
};

// total size: 0x8
// Decl: 1261
struct DELAYTRIGSETTINGS {
    float curtime; // offset 0x0, size 0x4, Decl: 1264
    signed char output; // offset 0x4, size 0x1, Decl: 1268
    char pad[3]; // offset 0x5, size 0x3, Decl: 1269
};

// total size: 0x10
struct DELAYTRIGSTATE {
    DELAYTRIGSETTINGS settings; // offset 0x0, size 0x8
    int triggered; // offset 0x8, size 0x4
    int delaytime; // offset 0xC, size 0x4
};

// total size: 0xC
// Decl: 1281
struct STATEGENSETTINGS {
    unsigned short inputoffset; // offset 0x0, size 0x2, Decl: 1284
    unsigned char numinputs; // offset 0x2, size 0x1, Decl: 1285
    char pad; // offset 0x3, size 0x1, Decl: 1286
    int outputcurrent; // offset 0x4, size 0x4, Decl: 1289
    int outputvalue[1]; // offset 0x8, size 0x4, Decl: 1292
};

// total size: 0x4
struct STATEGENINPUTS {
    int triggered[1]; // offset 0x0, size 0x4
};

// total size: 0xC
// Decl: 1301
struct STATEGENSTATE {
    STATEGENSETTINGS settings; // offset 0x0, size 0xC, Decl: 1302
};

// total size: 0x4
// Decl: 1309
struct MERGETRIGSETTINGS {
    unsigned char numinputs; // offset 0x0, size 0x1, Decl: 1310
    int triggered[1]; // offset 0x4, size 0x4, Decl: 1311
};

// total size: 0x8
struct MERGETRIGSTATE {
    MERGETRIGSETTINGS settings; // offset 0x0, size 0x4
    int triggered[1]; // offset 0x4, size 0x4
};

// total size: 0x8
struct ENVELOPESEGMENT {
    float duration; // offset 0x0, size 0x4
    float targetvalue; // offset 0x4, size 0x4
};

// total size: 0x20
// Decl: 1332
struct ENVELOPESETTINGS {
    unsigned short inputoffset; // offset 0x0, size 0x2, Decl: 1333
    signed char prevcontrol; // offset 0x2, size 0x1, Decl: 1334
    unsigned char cursegment; // offset 0x3, size 0x1, Decl: 1335
    float segmentremaining; // offset 0x4, size 0x4, Decl: 1337
    float delta; // offset 0x8, size 0x4, Decl: 1338
    float outputvalue; // offset 0xC, size 0x4, Decl: 1339
    unsigned char numsegments; // offset 0x10, size 0x1, Decl: 1341
    char pad; // offset 0x11, size 0x1, Decl: 1342
    short releasesegment; // offset 0x12, size 0x2, Decl: 1343
    float initialvalue; // offset 0x14, size 0x4, Decl: 1345
    ENVELOPESEGMENT segment[1]; // offset 0x18, size 0x8, Decl: 1348
};

// total size: 0x20
struct ENVELOPESTATE {
    ENVELOPESETTINGS settings; // offset 0x0, size 0x20
};

// total size: 0x4
struct ENVELOPEINPUTS {
    int control; // offset 0x0, size 0x4
};

// total size: 0xC
struct TABLESETTINGS {
    TABLE * ptable; // offset 0x0, size 0x4
    int previnputvalue; // offset 0x4, size 0x4
    int outputvalue; // offset 0x8, size 0x4
};

// total size: 0x10
struct TABLESTATE {
    TABLESETTINGS settings; // offset 0x0, size 0xC
    int inputvalue; // offset 0xC, size 0x4
};

// total size: 0x10
// Decl: 1406
struct DELAYLINESETTINGS {
    unsigned short inputoffset; // offset 0x0, size 0x2, Decl: 1407
    unsigned short maxslots; // offset 0x2, size 0x2, Decl: 1408
    unsigned short inputslot; // offset 0x4, size 0x2, Decl: 1410
    unsigned short outputslot; // offset 0x6, size 0x2, Decl: 1411
    int prevdelaytime; // offset 0x8, size 0x4, Decl: 1413
    int delayline[1]; // offset 0xC, size 0x4, Decl: 1416
};

// total size: 0x8
struct DELAYLINEINPUTS {
    int inputvalue; // offset 0x0, size 0x4
    int delaytime; // offset 0x4, size 0x4
};

// total size: 0x10
// Decl: 1427
struct DELAYLINESTATE {
    DELAYLINESETTINGS settings; // offset 0x0, size 0x10, Decl: 1428
};

// total size: 0x4
struct MUXSETTINGS {
    unsigned char numinputs; // offset 0x0, size 0x1
    char pad[3]; // offset 0x1, size 0x3
};

// total size: 0x8
// Decl: 1440
struct MUXSTATE {
    MUXSETTINGS settings; // offset 0x0, size 0x4, Decl: 1441
    union {
        int control; // offset 0x0, size 0x4
        int inputvalue[1]; // offset 0x0, size 0x4
    } u; // offset 0x4, size 0x4
};

// total size: 0x4
// Decl: 1457
struct DEMUXSETTINGS {
    unsigned char numoutputs; // offset 0x0, size 0x1, Decl: 1458
    char pad; // offset 0x1, size 0x1, Decl: 1459
    short prevcontrol; // offset 0x2, size 0x2, Decl: 1461
};

// total size: 0x8
struct DEMUXINPUTS {
    int control; // offset 0x0, size 0x4
    int value; // offset 0x4, size 0x4
};

// total size: 0x4
struct DEMUXOUTPUTS {
    int value[1]; // offset 0x0, size 0x4
};

// total size: 0x10
// Decl: 1477
struct DEMUXSTATE {
    DEMUXSETTINGS settings; // offset 0x0, size 0x4, Decl: 1478
    DEMUXINPUTS inputs; // offset 0x4, size 0x8, Decl: 1479
    DEMUXOUTPUTS outputs; // offset 0xC, size 0x4, Decl: 1480
};

struct MINSETTINGS {
    unsigned char numinputs; // offset 0x0, size 0x1
    char pad[3]; // offset 0x1, size 0x3
};

// total size: 0x8
struct MINSTATE {
    MINSETTINGS settings; // offset 0x0, size 0x4
    int input[1]; // offset 0x4, size 0x4
};

// total size: 0x4
struct MAXSETTINGS {
    unsigned char numinputs; // offset 0x0, size 0x1
    char pad[3]; // offset 0x1, size 0x3
};

// total size: 0x8
struct MAXSTATE {
    MAXSETTINGS settings; // offset 0x0, size 0x4
    int input[1]; // offset 0x4, size 0x4
};

// total size: 0x8
struct SCALESETTINGS {
    unsigned char numinputs; // offset 0x0, size 0x1
    char pad[3]; // offset 0x1, size 0x3
    float scalereciprocal; // offset 0x4, size 0x4
};

// total size: 0xC
struct SCALESTATE {
    SCALESETTINGS settings; // offset 0x0, size 0x8
    int input[1]; // offset 0x8, size 0x4
};

// total size: 0x4
struct ADDSETTINGS {
    unsigned char numinputs; // offset 0x0, size 0x1
    char pad[3]; // offset 0x1, size 0x3
};

// total size: 0x8
struct ADDSTATE {
    ADDSETTINGS settings; // offset 0x0, size 0x4
    int input[1]; // offset 0x4, size 0x4
};

// total size: 0x8
struct SUBTRACTSTATE {
    int inputa; // offset 0x0, size 0x4
    int inputb; // offset 0x4, size 0x4
};

// total size: 0x8
struct MULTIPLYSTATE {
    int inputa; // offset 0x0, size 0x4
    int inputb; // offset 0x4, size 0x4
};

// total size: 0x8
struct DIVIDESTATE {
    int inputa; // offset 0x0, size 0x4
    int inputb; // offset 0x4, size 0x4
};

// total size: 0x8
struct MODULOSTATE {
    int inputa; // offset 0x0, size 0x4
    int inputb; // offset 0x4, size 0x4
};

// total size: 0x8
struct PLAYERINPUTSETTINGS {
    unsigned char type; // offset 0x0, size 0x1
    char pad[3]; // offset 0x1, size 0x3
    int prevvalue; // offset 0x4, size 0x4
};

// total size: 0xC
// Decl: 1594
struct PLAYERINPUTSTATE {
    PLAYERINPUTSETTINGS settings; // offset 0x0, size 0x8, Decl: 1595
    int value; // offset 0x8, size 0x4, Decl: 1597
};

// total size: 0x4
struct PLAYERHANDLE {
    union {
        int shandle; // offset 0x0, size 0x4
        int streamhandle; // offset 0x0, size 0x4
        int seqhandle; // offset 0x0, size 0x4
    } u; // offset 0x0, size 0x4
};

// total size: 0x14
// 1615
struct PLAYERSETTINGS {
    ModuleBank *pModuleBank; // offset 0x0, size 0x4, Decl: 1621
    SAMPLEGROUP *psamplegroup; // offset 0x4, size 0x4, Decl: 1628
    PLAYERHANDLE handle; // offset 0x8, size 0x4
    signed char prevplaycontrol[2]; // offset 0xC, size 0x2, Decl: 1650
    unsigned char numinputs; // offset 0xE, size 0x1, Decl: 1652
    unsigned char updateoutputs; // offset 0xF, size 0x1, Decl: 1654
    signed char sampletype; // offset 0x10, size 0x1, Decl: 1657
    char pad[3]; // offset 0x11, size 0x3, Decl: 1662
};

// total size: 0x1C
struct PLAYERSTATE {
    PLAYERSETTINGS settings; // offset 0x0, size 0x14
    int sampleselect; // offset 0x14, size 0x4
    int playcontrol; // offset 0x18, size 0x4
};

// total size: 0x8
struct PLAYEROUTPUTS {
    int timeleft; // offset 0x0, size 0x4
    int timecurr; // offset 0x4, size 0x4
};

// total size: 0x8
// 1739
struct OSCILLATORSETTINGS {
    unsigned char waveform; // offset 0x0, size 0x1, Decl: 1740
    char pad[3]; // offset 0x1, size 0x3, Decl: 1741
    float percentofcycle; // offset 0x4, size 0x4, Decl: 1743
};

// total size: 0x10
// 1747
struct OSCILLATORSTATE {
    OSCILLATORSETTINGS settings; // offset 0x0, size 0x8, Decl: 1748
    int period; // offset 0x8, size 0x4, Decl: 1751
    int amplitude; // offset 0xC, size 0x4, Decl: 1752
};

// total size: 0x10
struct RAMPSETTINGS {
    float currentvalue; // offset 0x0, size 0x4
    float delta; // offset 0x4, size 0x4
    int prevtargetvalue; // offset 0x8, size 0x4
    int prevduration; // offset 0xC, size 0x4
};

// total size: 0x1C
// 1764
struct RAMPSTATE {
    RAMPSETTINGS settings; // offset 0x0, size 0x10, Decl: 1765
    int duration; // offset 0x10, size 0x4, Decl: 1768
    int scale; // offset 0x14, size 0x4, Decl: 1769
    int targetvalue; // offset 0x18, size 0x4, Decl: 1770
};

// total size: 0x8
struct ADDMAXSETTINGS {
    unsigned char numinputs; // offset 0x0, size 0x1
    char pad[3]; // offset 0x1, size 0x3
    int maxvalue; // offset 0x4, size 0x4
};

// total size: 0xC
struct ADDMAXSTATE {
    ADDMAXSETTINGS settings; // offset 0x0, size 0x8
    int input[1]; // offset 0x8, size 0x4
};

// total size: 0x4
struct SUBTRACTMINSETTINGS {
    int minvalue; // offset 0x0, size 0x4
};

// total size: 0xC
// 1800
struct SUBTRACTMINSTATE {
    SUBTRACTMINSETTINGS settings; // offset 0x0, size 0x4, Decl: 1801
    int inputa; // offset 0x4, size 0x4, Decl: 1804
    int inputb; // offset 0x8, size 0x4, Decl: 1805
};

// total size: 0x4
struct MULTIPLYMAXSETTINGS {
    int maxvalue; // offset 0x0, size 0x4
};

// total size: 0xC
struct MULTIPLYMAXSTATE {
    MULTIPLYMAXSETTINGS settings; // offset 0x0, size 0x4
    int inputa; // offset 0x4, size 0x4
    int inputb; // offset 0x8, size 0x4
};

// total size: 0x8
struct MIN2STATE {
    int inputa; // offset 0x0, size 0x4
    int inputb; // offset 0x4, size 0x4
};

// total size: 0x8
struct MAX2STATE {
    int inputa; // offset 0x0, size 0x4
    int inputb; // offset 0x4, size 0x4
};

// total size: 0x4
struct SCALE2SETTINGS {
    float scalereciprocal; // offset 0x0, size 0x4
};

// total size: 0xC
struct SCALE2STATE {
    SCALE2SETTINGS settings; // offset 0x0, size 0x4
    int inputa; // offset 0x4, size 0x4
    int inputb; // offset 0x8, size 0x4
};

// total size: 0x8
struct ADD2STATE {
    int inputa; // offset 0x0, size 0x4
    int inputb; // offset 0x4, size 0x4
};

// total size: 0x1C
// 1860
struct FunctionSettings {
    Csis::FunctionHandle functionHandle; // offset 0x0, size 0x8, Decl: 1866
    Csis::FunctionClient client; // offset 0x8, size 0x10, Decl: 1871
    unsigned char numParameters; // offset 0x18, size 0x1, Decl: 1875
    unsigned char triggered; // offset 0x19, size 0x1, Decl: 1876
    char pad[2]; // offset 0x1A, size 0x2, Decl: 1877
};

// total size: 0x4
struct FunctionOutputs {
    int value[1]; // offset 0x0, size 0x4
};

// total size: 0x20
struct FunctionState {
    FunctionSettings settings; // offset 0x0, size 0x1C
    FunctionOutputs outputs; // offset 0x1C, size 0x4
};

// total size: 0x1C
struct GlobalVariableSettings {
    Csis::GlobalVariableHandle globalVariableHandle; // offset 0x0, size 0x8
    Csis::FunctionClient client; // offset 0x8, size 0x10
    int globalValue; // offset 0x18, size 0x4
};
// total size: 0x1C
struct GlobalVariableState {
    GlobalVariableSettings settings; // offset 0x0, size 0x1C
};

// total size: 0x14
struct ClassDataSettings {
    Csis::FunctionClient client; // offset 0x0, size 0x10
    unsigned char numOutputs; // offset 0x10, size 0x1
    char pad[3]; // offset 0x11, size 0x3
};

// total size: 0x4
struct ClassDataOutputs {
    int value[1]; // offset 0x0, size 0x4
};

// total size: 0x18
struct ClassDataState {
    ClassDataSettings settings; // offset 0x0, size 0x14
    ClassDataOutputs outputs; // offset 0x14, size 0x4
};

// total size: 0x14
struct ClassDestructorSettings {
    Csis::ClassDestructorClient client; // offset 0x0, size 0x10
    int triggered; // offset 0x10, size 0x4
};

// total size: 0x14
struct ClassDestructorState {
    ClassDestructorSettings settings; // offset 0x0, size 0x14
};

}

#endif
