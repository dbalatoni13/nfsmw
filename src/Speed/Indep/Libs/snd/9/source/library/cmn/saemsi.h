#ifndef SND_CMN_AEMS_H
#define SND_CMN_AEMS_H

#include "slinklist.h"
#include "Speed/Indep/Libs/snd/9/extern/aemsdef.h"
#include "sndcmn.h"

namespace AemsDef {

// total size: 0x60
struct SNDAEMSSTREAMCHANNEL {
    // Members
    struct CListDNode ln; // offset 0x0, size 0x8
    int handle; // offset 0x8, size 0x4
    int priority; // offset 0xC, size 0x4
    unsigned int timestamp; // offset 0x10, size 0x4
    unsigned char state; // offset 0x14, size 0x1
    char pad[3]; // offset 0x15, size 0x3
    int sndstrmhandle; // offset 0x18, size 0x4
    int leadinrequesthandle; // offset 0x1C, size 0x4
    int leadinsamples; // offset 0x20, size 0x4
    SAMPLEENTRY * psampleentry; // offset 0x24, size 0x4
    char * filename; // offset 0x28, size 0x4
    int fileoffset; // offset 0x2C, size 0x4
    int input[12]; // offset 0x30, size 0x30
};

struct SNDAEMS {
    CListDStack timerclient; // offset 0x0, size 0x4, Decl: C:/packages/rwaudiocore/2.09.00/source/9/cmn/saemsi.h:14
    CListDStack modulebank; // offset 0x4, size 0x4, Decl: C:/packages/rwaudiocore/2.09.00/source/9/cmn/saemsi.h:17
    unsigned char streamchannels; // offset 0x8, size 0x1
    signed char timerUpdatesSkipped; // offset 0x9, size 0x1
    bool instreamsynctask; // offset 0xC, size 0x1
    bool asyncloading; // offset 0x10, size 0x1
    Snd::VariableTimerClient variabletimerclient; // offset 0x14, size 0x10, Decl: C:/packages/rwaudiocore/2.09.00/source/9/cmn/saemsi.h:20
    SNDAEMSSTREAMCHANNEL *pstreamchannel; // offset 0x24, size 0x4
    CListDStack allocatedstreams; // offset 0x28, size 0x4
    CListDStack freestreams; // offset 0x2C, size 0x4
    void (* logaddmodulebank)(); // offset 0x30, size 0x4
    void (* logremovemodulebank)(); // offset 0x34, size 0x4

    SNDAEMS() {}
};
typedef struct SNDAEMS SNDAEMS;

typedef void PLAYERINTERFACEFN(int, int);
typedef PLAYERHANDLE PLAYERPLAYFN(PLAYERSTATE *, SAMPLEENTRY *);
typedef void PLAYERSTOPFN(PLAYERSTATE *);
typedef void PLAYERPAUSEFN(PLAYERHANDLE);
typedef PLAYERSTOPFN PLAYERUNPAUSEFN;
typedef int PLAYERUPDATEFN(PLAYERSTATE *);
typedef void PLAYERSETOPTSFN(SNDPLAYOPTS *, int);

} // namespace AemsDef

extern AemsDef::SNDAEMS sndaems;
extern AemsDef::PLAYERSETOPTSFN *SNDAEMSoptfn[12];

#endif
