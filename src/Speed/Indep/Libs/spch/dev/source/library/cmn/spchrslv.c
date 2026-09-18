#include "./spchi.h"

extern void SPCH_GetEventDatInfo(char *eventData, int *projID, int *datID);
extern void iSPCH_InitCsis(void *evtFile);

void SPCH_ClearMatchParmSettings(unsigned long inChannel) {
    int j;
    int i;
    unsigned char *matchParmArray;

    for (i = 0; i < 8; i++) {
        if (gEventDats[i].data != 0 && gEventDats[i].channel == inChannel) {
            matchParmArray = iSPCH_GetGlobalMatchParmAddr(gEventDats[i].data);
            for (j = 0; j < gEventDats[i].data->numGlobalMatchParms; j++) {
                matchParmArray[j] = 0xFF;
            }
        }
    }
}

static int iSPCH_BindData(char *dataFile, unsigned int inChannel) {
    int result;
    int i;
    VoxData *data;
    int datID;
    int projID;

    result = 0;
    if ((*(unsigned int *)dataFile & ~0xFFFF) != (0x0312 << 16)) {
        goto abort;
    }
    SPCH_GetEventDatInfo(dataFile, &projID, &datID);
    for (i = 0; i < 8; i++) {
        data = gEventDats[i].data;
        if (data != 0) {
            if (data->datID == datID) {
                if (data->projectID == projID) {
                    goto abort;
                }
            }
        }
    }
    for (i = 0; i < 8; i++) {
        if (gEventDats[i].data == 0) {
            gEventDats[i].data = (VoxData *)dataFile;
            result = 1;
            gEventDats[i].channel = inChannel;
            break;
        }
    }
    SPCH_ClearMatchParmSettings(inChannel);
abort:
    return result;
}

int SPCH_AddEventDB(char *dataFile, unsigned int channel) {
    int result;

    result = 0;
    if (channel <= 7) {
        result = iSPCH_BindData(dataFile, channel);
        iSPCH_InitCsis(dataFile);
    }
    return result;
}
