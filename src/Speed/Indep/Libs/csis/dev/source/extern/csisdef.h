#ifndef CSIS_CSISDEF_H
#define CSIS_CSISDEF_H 1

#include <csis/csis.h>
#include "Speed/Indep/Libs/snd/9/source/library/cmn/slinklist.h"

namespace Csis {

struct CrcAndKey {
    short crc;
    short key;
};

struct FunctionDesc {
    union {
        CrcAndKey crcAndKey;
        int key;
        CListDNode *clients;
    };
    const char *pStringId;
    union {
        CrcAndKey crcAndKey;
        int key;
    } u;
};

typedef FunctionDesc ClassDesc;

struct GlobalVariableDesc {
    union {
        CrcAndKey crcAndKey;
        int key;
        CListDNode *clients;
    };
    Parameter curVal;
    const char *pStringId;
    union {
        CrcAndKey crcAndKey;
        int key;
    } u;
};

struct SystemDesc {
    char id[4];
    unsigned char ver;
    unsigned char verCsisxMajor;
    unsigned char verCsisxMinor;
    unsigned char verCsisxPatch;
    unsigned char platform;
    unsigned char resolved;
    unsigned short numFunctions;
    unsigned short numClasses;
    unsigned short numGlobalVariables;
    short crc;
    char pad[2];
    FunctionDesc *pFunctionDesc;
    ClassDesc *pClassDesc;
    GlobalVariableDesc *pGlobalVariableDesc;
    CListDNode linkNode;
};

} // namespace Csis

#endif
