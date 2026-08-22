#include "spch/spch.h"
#include <csis/csis.h>
#include <string.h>

extern VoxEvent *iSPCH_FindEvent(EventSpec *eventSpec);
extern void *iSPCH_MemAlloc(unsigned int numBytes);
extern int iSPCH_AddEvent(unsigned int *parms);
extern int SPCH_MakeEventSpec(int projID, int datID, int eventID);

struct CSIS_Data {
    Csis::InterfaceId interfaceID;
    Csis::FunctionHandle handle;
    Csis::FunctionClient client;
};

static void iSPCH_CsisCb(Csis::Parameter *pParameters, void *pClientData) {
    VoxEvent *event;
    unsigned int numBytes;
    unsigned int *parms;
    int result;
    EventSpec spec;

    spec = *reinterpret_cast<EventSpec *>(&pClientData);
    event = iSPCH_FindEvent(&spec);
    if (event == 0) {
        goto abort;
    }
    numBytes = event->numParms * 4;
    parms = reinterpret_cast<unsigned int *>(iSPCH_MemAlloc(numBytes + 4));
    if (parms == 0) {
        goto abort;
    }
    *parms = *reinterpret_cast<unsigned int *>(&spec);
    memcpy(parms + 1, pParameters, numBytes);
    result = iSPCH_AddEvent(parms);
    if (result == 0) {
        iSPCH_MemFree(parms);
    }
abort:
    return;
}

void iSPCH_InitCsis(void *evtFile) {
    VoxData *voxData;
    int spec;
    char *names;
    CSIS_Data *csisData;
    VoxEvent *event;
    int i;

    if (evtFile == 0) {
        goto abort;
    }
    voxData = reinterpret_cast<VoxData *>(evtFile);
    if (voxData->csisOffset == 0) {
        goto abort;
    }
    if (voxData->csisResolved != 0) {
        goto abort;
    }
    csisData = reinterpret_cast<CSIS_Data *>(
        reinterpret_cast<char *>(evtFile) + voxData->csisOffset);
    names = reinterpret_cast<char *>(csisData + voxData->numEvents);
    i = 0;
    if (i < voxData->numEvents) {
        do {
            Csis::FunctionHandle *localHandle;
            Csis::FunctionClient *localClient;

            csisData[i].interfaceID.pString = reinterpret_cast<const char *>(
                names + reinterpret_cast<unsigned int>(csisData[i].interfaceID.pString));
            localHandle = &csisData[i].handle;
            localHandle->Set(&csisData[i].interfaceID);
            localClient = &csisData[i].client;
            localClient->pClientFunc = iSPCH_CsisCb;
            event = reinterpret_cast<VoxEvent *>(iSPCH_GetOffset16(
                reinterpret_cast<unsigned char *>(evtFile),
                reinterpret_cast<unsigned short *>(voxData + 1), i));
            spec = SPCH_MakeEventSpec(voxData->projectID, voxData->datID, event->ID);
            localClient->pClientData = reinterpret_cast<void *>(spec);
            Csis::Function::Subscribe(localHandle, localClient);
            i++;
        } while (i < voxData->numEvents);
    }
    voxData->csisResolved = 1;
abort:
    return;
}
