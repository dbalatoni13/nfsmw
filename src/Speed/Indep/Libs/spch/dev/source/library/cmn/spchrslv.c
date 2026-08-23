
#include "spch/spch.h"

static int iSPCH_BindData(char *dataFile, unsigned int channel);
void iSPCH_InitCsis(void *dataFile);
void SPCH_GetEventDatInfo(char *eventData, int *projID, int *datID);
void SPCH_ClearMatchParmSettings(unsigned long inChannel);

static int iSPCH_BindData(char *dataFile, unsigned int inChannel) {
    int result;
    int i;
    VoxData *data;
    int datID, projID;

    result = 0;
    if ((*reinterpret_cast<unsigned int *>(dataFile) & 0xFFFF0000) == 0x03120000) {
        SPCH_GetEventDatInfo(dataFile, &projID, &datID);
        i = 0;
        do {
            if (gEventDats[i].eventDat != 0) {
                data = gEventDats[i].eventDat;
                if (data->datID == datID) {
                    if (data->projectID == projID) {
                        goto abort;
                    }
                }
            }
            i++;
        } while (i < 8);
        i = 0;
        do {
            if (gEventDats[i].eventDat == 0) {
                gEventDats[i].eventDat = reinterpret_cast<VoxData *>(dataFile);
                result = 1;
                gEventDats[i].channel = inChannel;
                break;
            }
            i++;
        } while (i <= 7);
        SPCH_ClearMatchParmSettings(inChannel);
    }
abort:
    return result;
}

int SPCH_AddEventDB(char *dataFile, unsigned int channel) {
    int result;

    result = 0;
    if (channel >= 8) {
        goto abort;
    }
    result = iSPCH_BindData(dataFile, channel);
    iSPCH_InitCsis(dataFile);
abort:
    return result;
}

void SPCH_ClearMatchParmSettings(unsigned long inChannel) {
    int i, j;
    unsigned char *matchParmArray;

    for (i = 0; i < 8; i++) {
        if (gEventDats[i].eventDat != 0 && gEventDats[i].channel == inChannel) {
            matchParmArray = iSPCH_GetGlobalMatchParmAddr(gEventDats[i].eventDat);
            for (j = 0; j < gEventDats[i].eventDat->numGlobalMatchParms; j++) {
                matchParmArray[j] = 0xFF;
            }
        }
    }
}
