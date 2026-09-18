#include "./spchi.h"
#include <cstring>

extern VoxEvent *iSPCH_FindEvent(EventSpec *spec);
extern void *iSPCH_MemAlloc(unsigned int numBytes);
extern void iSPCH_MemFree(void *ptr);
extern int iSPCH_AddEvent(unsigned int *parms);
extern int SPCH_MakeEventSpec(int projectID, int datID, int eventID);

static void iSPCH_CsisCb(Csis::Parameter *pParameters, void *pClientData) {
    VoxEvent *event;
    unsigned int numBytes;
    unsigned int *parms;
    int result;
    EventSpec spec;

    spec = *(EventSpec *)&pClientData;
    event = iSPCH_FindEvent(&spec);
    if (event != 0) {
        numBytes = event->numParms * 4;
        parms = (unsigned int *)iSPCH_MemAlloc(numBytes + 4);
        if (parms != 0) {
            parms[0] = *(unsigned int *)&spec;
            memcpy(parms + 1, pParameters, numBytes);
            result = iSPCH_AddEvent(parms);
            if (result == 0) {
                iSPCH_MemFree(parms);
            }
        }
    }
}

void iSPCH_InitCsis(void *evtFile) {
    VoxData *voxData;
    int spec;
    char *names;
    CSIS_Data *csisData;
    int i;

    voxData = (VoxData *)evtFile;
    if (voxData == 0) {
        return;
    }
    if (voxData->csisOffset == 0) {
        return;
    }
    if (voxData->csisResolved != 0) {
        return;
    }
    csisData = (CSIS_Data *)((char *)voxData + voxData->csisOffset);
    names = (char *)csisData + (voxData->numEvents << 5);
    for (i = 0; i < voxData->numEvents; i++) {
        CSIS_Data *data = (CSIS_Data *)(i * 0x20 + (int)csisData);
        Csis::FunctionHandle *localHandle = &data->handle;
        Csis::FunctionClient *localClient;

        data->interfaceID.pString = names + (int)data->interfaceID.pString;
        localHandle->Set(&data->interfaceID);
        localClient = &data->client;
        localClient->pClientFunc = iSPCH_CsisCb;
        spec = SPCH_MakeEventSpec(voxData->projectID, voxData->datID,
                                  *(unsigned short *)iSPCH_GetOffset16((unsigned char *)voxData,
                                                                      (unsigned short *)((char *)voxData + 0x18), i));
        localClient->pClientData = (void *)spec;
        Csis::Function::Subscribe(localHandle, localClient);
    }
    voxData->csisResolved = 1;
}
