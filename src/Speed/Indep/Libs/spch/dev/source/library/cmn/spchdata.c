#include "spch/spch.h"

MemAllocFuncPtr gMemAlloc = 0;
MemFreeFuncPtr gMemFree = 0;
SPCHType_ExtVecs gExtVecs = {0, 0, 0};
int gSPCH_Initialized = 0;
VoxBankInfo *gVoxBanks = 0;
int gUniqueBankHandle = 0;
int gNumBanks = 0;
int gBankCount = 0;
AddEventFuncPtr gSPCH_AddEvent = 0;
int gClearCycle = 1;

SPCH_Callbacks gCallbacks;
EventDatInfo gEventDats[8];
VoxPendingEvents gVoxEvents;
VOXINGAME gVoxInGame[8];
unsigned int gGameNum;
int gDataRate;
int gFilterSetting[8];
int gPreLoadTicks;
unsigned int gLastTick;
unsigned short gLastSubTick;
