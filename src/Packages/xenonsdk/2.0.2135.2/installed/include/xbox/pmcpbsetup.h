/*

Copyright (C) Microsoft Corporation 2005.  All rights reserved.

Module Name:

    pmcPBSetup.h

Abstract:

    Extensible setup API for CPU performance counters.

    void main()
    {
        PMCInstallSetup(&PMCDefaultSetups[PMC_SETUP_OVERVIEW]);    
        PMCStart();
        ... code to profile ...
        PMCStop();
        CHECKPOINTPMC();
    }

    see comments on configuring your own setups
    
*/


#ifndef __PMCPBSETUP_H__
#define __PMCPBSETUP_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <pmcpb.h>




////////////////////////////////////////////////////////////////////
//
// PMC Setup structures and enums
//

//
// PMCEventSource describes where an event comes from
//
typedef struct sPMCEventSource
{
    ePMCSourceUnit  unit;
    unsigned char   lane;
}PMCEventSource;

//
// This structure encapsulates a complete configuration,
// but does not support any PMW or XIF events besides the L2
//
typedef struct sPMCSetup
{
    // "hid7" controls the per-CPU core event muxing
    PMCHID7             hid7;
    PMWL2Control310     l2control310;
    unsigned long       countMode;
    // "source' sets the final counter muxes that determine
    // what each counter actually counts.
    // use PMC_FINAL_* macros to set source[] for the most
    // common cases
    PMCEventSource      source[16];
}PMCSetup;

//
// default setups
//
// OVERVIEW_PB0T0 looks like this:
/*
    { 
        { // core event muxing
            { 
                PB_PFM_BUS_LANE_0_1_IU_THREAD_0,
                    PB_IU_PFM_EVENT_IL1_MISS_CYCLES,
                    PB_IU_PFM_EVENT_INSTR_FLUSHED,
                PB_PFM_BUS_LANE_2_3_IU_THREAD_0,
                    PB_IU_PFM_EVENT_INSTR_STALL,
                    PB_IU_PFM_EVENT_INSTR_DEP_STALL,
                PB_PFM_BUS_LANE_4_5_XU_THREAD_0,
                    PB_DEFAULT_PFM_EVENT_T0_PPC_COMMIT,
                    PB_XU_PFM_EVENT_LOAD_MISS,
                PB_PFM_BUS_LANE_6_7_XU_THREAD_0,
                    PB_XU_PFM_EVENT_LOAD_MISS_LHS,
                    PB_XU_PFM_EVENT_DERAT_MISS,
            }
        },

        { // l2 event muxing
            _BITZERO64_ >> PMW_L2_EVENT_GROUP_o|
            _BITZERO64_ >> PMW_L2_EVENT_GROUP_D
        },

        // problem state count mode
        PFMCONTROL_MODE_ANY,

        { // final counter muxing
            PMC_FINAL_PB0
        }
    },
*/

typedef enum _ePMCSetup
{
    PMC_SETUP_FIRST = 0,

    PMC_SETUP_OVERVIEW_PB0T0 =0,
    PMC_SETUP_OVERVIEW_PB0T1, 
    PMC_SETUP_OVERVIEW_PB1T0, 
    PMC_SETUP_OVERVIEW_PB1T1, 
    PMC_SETUP_OVERVIEW_PB2T0, 
    PMC_SETUP_OVERVIEW_PB2T1, 

    PMC_SETUP_OVERVIEW_PB0,
    PMC_SETUP_OVERVIEW_PB1,
    PMC_SETUP_OVERVIEW_PB2,    

    PMC_SETUP_OVERVIEW_6THREADS, 

    PMC_SETUP_INSTRUCTIONMIX_PB0T0,
    PMC_SETUP_INSTRUCTIONMIX_PB0T1,
    PMC_SETUP_INSTRUCTIONMIX_PB1T0,
    PMC_SETUP_INSTRUCTIONMIX_PB1T1,
    PMC_SETUP_INSTRUCTIONMIX_PB2T0,    
    PMC_SETUP_INSTRUCTIONMIX_PB2T1,    
    
    PMC_SETUP_FLUSHREASONS_PB0T0,
    PMC_SETUP_FLUSHREASONS_PB0T1,
    PMC_SETUP_FLUSHREASONS_PB1T0,
    PMC_SETUP_FLUSHREASONS_PB1T1,
    PMC_SETUP_FLUSHREASONS_PB2T0,
    PMC_SETUP_FLUSHREASONS_PB2T1,
    
    PMC_SETUP_VMXMIX_PB0T0,
    PMC_SETUP_VMXMIX_PB0T1,
    PMC_SETUP_VMXMIX_PB1T0,
    PMC_SETUP_VMXMIX_PB1T1,
    PMC_SETUP_VMXMIX_PB2T0,
    PMC_SETUP_VMXMIX_PB2T1,
    
    PMC_SETUP_VMXFPU_PB0T0,
    PMC_SETUP_VMXFPU_PB0T1,
    PMC_SETUP_VMXFPU_PB1T0,
    PMC_SETUP_VMXFPU_PB1T1,
    PMC_SETUP_VMXFPU_PB2T0,
    PMC_SETUP_VMXFPU_PB2T1,
    
    PMC_SETUP_FPU_PB0T0,
    PMC_SETUP_FPU_PB0T1,
    PMC_SETUP_FPU_PB1T0,
    PMC_SETUP_FPU_PB1T1,
    PMC_SETUP_FPU_PB2T0,
    PMC_SETUP_FPU_PB2T1,
    
    PMC_SETUP_PREFETCH,

    //
    // counters dealing with events that go to main memory
    // 
    PMC_SETUP_MEMORY_LATENCY_PB0,
    PMC_SETUP_MEMORY_LATENCY_PB1,
    PMC_SETUP_MEMORY_LATENCY_PB2,

    PMC_SETUP_LAST    
}ePMCSetup;

extern PMCSetup PMCDefaultSetups[PMC_SETUP_LAST];

////////////////////////////////////////////////////////////////////
//
// PMC Setup APIs
//

//
// PMCInstallSetup
//
// takes a setup configuration and applies it to the entire CPU
// 
// it will call PMCEnable on every core, and set up each CPU core
// with the same core event configuration.
//
// Use PMCInstallSetupOnThisCore/PMCInstallPRVSetup if you want more
// control over what gets called where.
//
void PMCInstallSetup(PMCSetup* setup);


//
// PMCInstallSetupOnThisCore
//
// installs the HID7 setup.  L2 and final counter muxing are done with
// PMCInstallPRVSetup
//
void PMCInstallSetupOnThisCore(PMCSetup* setup);

//
// PMCInstallPRVSetup
//
// installs the L2 events and final counter muxing.  per-core muxing is 
// installed with PMCInstallSetupOnThisCore
//
void PMCInstallPRVSetup(PMCSetup* setup);

//
// helper macro to dump the counters to stdout
//

#define CHECKPOINTPMC()  { PMCState pmcstate;\
                           PMCStop();\
                           PMCGetCounters(&pmcstate); \
                           PMCDumpCountersVerbose(&pmcstate,0);\
                           PMCResetCounters(); \
                           PMCStart();}




//
// helper macros to set up the final mux controlled by
// PMCEventSource
//

// all 8 lanes from PB0
#define PMC_FINAL_PB0 \
            {PMCSourcePB0,0}, \
            {PMCSourcePB0,1}, \
            {PMCSourcePB0,2}, \
            {PMCSourcePB0,3}, \
            {PMCSourcePB0,4}, \
            {PMCSourcePB0,5}, \
            {PMCSourcePB0,6}, \
            {PMCSourcePB0,7}, \
            {PMCSourcePMW,4}, \
            {PMCSourcePMW,5}, \
            {PMCSourcePMW,6}, \
            {PMCSourcePMW,7}, \
            {PMCSourcePMW,0}, \
            {PMCSourcePMW,1}, \
            {PMCSourcePMW,2}, \
            {PMCSourcePMW,3}, 



#define PMC_FINAL_PB1 \
            {PMCSourcePB1,0}, \
            {PMCSourcePB1,1}, \
            {PMCSourcePB1,2}, \
            {PMCSourcePB1,3}, \
            {PMCSourcePMW,0}, \
            {PMCSourcePMW,1}, \
            {PMCSourcePMW,2}, \
            {PMCSourcePMW,3}, \
            {PMCSourcePB1,4}, \
            {PMCSourcePB1,5}, \
            {PMCSourcePB1,6}, \
            {PMCSourcePB1,7}, \
            {PMCSourcePMW,4}, \
            {PMCSourcePMW,5}, \
            {PMCSourcePMW,6}, \
            {PMCSourcePMW,7}, 


#define PMC_FINAL_PB2 \
            {PMCSourcePMW,0}, \
            {PMCSourcePMW,1}, \
            {PMCSourcePMW,2}, \
            {PMCSourcePMW,3}, \
            {PMCSourcePB2,0}, \
            {PMCSourcePB2,1}, \
            {PMCSourcePB2,2}, \
            {PMCSourcePB2,3}, \
            {PMCSourcePB2,4}, \
            {PMCSourcePB2,5}, \
            {PMCSourcePB2,6}, \
            {PMCSourcePB2,7}, \
            {PMCSourcePMW,4}, \
            {PMCSourcePMW,5}, \
            {PMCSourcePMW,6}, \
            {PMCSourcePMW,7}, 


#define PMC_FINAL_PMW \
            {PMCSourcePMW,0}, \
            {PMCSourcePMW,1}, \
            {PMCSourcePMW,2}, \
            {PMCSourcePMW,3}, \
            {PMCSourcePMW,4}, \
            {PMCSourcePMW,5}, \
            {PMCSourcePMW,6}, \
            {PMCSourcePMW,7}, \
            {PMCSourcePMW,8}, \
            {PMCSourcePMW,9}, \
            {PMCSourcePMW,10}, \
            {PMCSourcePMW,11}, \
            {PMCSourcePMW,12}, \
            {PMCSourcePMW,13}, \
            {PMCSourcePMW,14}, \
            {PMCSourcePMW,15}, 






#ifdef __cplusplus
}
#endif

#endif
