/*

Copyright (C) Microsoft Corporation 2005.  All rights reserved.

Module Name:

    pmcPB.h

Abstract:
    PB performance monitor APIs.

    There are 2 sets of APIs : a low-level access API, and a simple
    setup-based API (PMCInstallSetup) which does everything you'd need.

    See PMCPBSetup.h
*/



#ifndef __PMCPB_H__
#define __PMCPB_H__

#pragma warning(push)
// warning C4201: nonstandard extension used : nameless struct/union
#pragma warning(disable: 4201)

#ifdef __cplusplus
extern "C"{
#endif

////////////////////////////////////////////////////////////////////
//
// Constants
//

/**********************************************************************************************/
/* The following are defines that define the selection of PB units & events for PFM bus lanes */
/**********************************************************************************************/
/*
 * PB PFM Bus Lane01 thread select, HID7(00:02)
 */
#define PB_PFM_BUS_LANE_0_1_IU_THREAD_0     0
#define PB_PFM_BUS_LANE_0_1_XU_THREAD_1     1
#define PB_PFM_BUS_LANE_0_1_VSU_THREAD_1    2
#define PB_PFM_BUS_LANE_0_1_MMU             4

/*
 * PB PFM Bus Lane23 thread select, HID7(13:15)
 */
#define PB_PFM_BUS_LANE_2_3_IU_THREAD_0     0
#define PB_PFM_BUS_LANE_2_3_XU_THREAD_1     1
#define PB_PFM_BUS_LANE_2_3_VSU_THREAD_0    2
#define PB_PFM_BUS_LANE_2_3_VSU_THREAD_1    3
#define PB_PFM_BUS_LANE_2_3_MMU             4

/*
 * PB PFM Bus Lane45 thread select, HID7(26:28)
 */
#define PB_PFM_BUS_LANE_4_5_IU_THREAD_1     0
#define PB_PFM_BUS_LANE_4_5_XU_THREAD_0     1
#define PB_PFM_BUS_LANE_4_5_VSU_THREAD_0    2
#define PB_PFM_BUS_LANE_4_5_VSU_THREAD_1    3
#define PB_PFM_BUS_LANE_4_5_MMU             4

/*
 * PB PFM Bus Lane67 thread select, HID7(39:41)
 */
#define PB_PFM_BUS_LANE_6_7_IU_THREAD_1     0
#define PB_PFM_BUS_LANE_6_7_XU_THREAD_0     1
#define PB_PFM_BUS_LANE_6_7_VSU_THREAD_0    2
#define PB_PFM_BUS_LANE_6_7_MMU             4

/*
 * PB IU Threads PFM event select
 *
 *    IU Thread0: HID7(03:07) Lane 0
 *                HID7(08:12) Lane 1
 *                HID7(16:20) Lane 2
 *                HID7(21:25) Lane 3
 *
 *    IU Thread1: HID7(29:33) Lane 4
 *                HID7(34:38) Lane 5
 *                HID7(42:46) Lane 6
 *                HID7(47:51) Lane 7
 */
#define PB_IU_PFM_EVENT_BRANCH_COMMIT            0x0
#define PB_IU_PFM_EVENT_BRANCH_FLUSH             0x1
#define PB_IU_PFM_EVENT_IBUF_EMPTY               0x2
#define PB_IU_PFM_EVENT_IERAT_MISS               0x3
#define PB_IU_PFM_EVENT_IL1_MISS_CYCLES          0x4
#define PB_IU_PFM_EVENT_UCODE_COMMIT             0x5
#define PB_IU_PFM_EVENT_DISPATCH_BLOCKED         0x6
#define PB_IU_PFM_EVENT_INSTR_STALL              0x7
#define PB_IU_PFM_EVENT_INSTR_DEP_STALL          0x8
#define PB_IU_PFM_EVENT_INSTR_FLUSHED            0x9
#define PB_IU_PFM_EVENT_TWO_INSTR_ISSUE          0xa   // 10
#define PB_IU_PFM_EVENT_PPC_COMMIT               0xb   // 11
#define PB_IU_PFM_EVENT_EXT_INT_PENDING          0xc   // 12
#define PB_IU_PFM_EVENT_EXT_INT_PENDING_HW_LAT   0xd   // 13
#define PB_IU_PFM_EVENT_INTEGER_COMMIT           0xe   // 14
#define PB_IU_PFM_EVENT_FP_COMMIT                0xf   // 15
#define PB_IU_PFM_EVENT_ISYNC_STALL              0x10  // 16
#define PB_IU_PFM_EVENT_VIQ_STALL                0x11  // 17
#define PB_IU_PFM_EVENT_VIQ_DEP_STALL            0x12  // 18

/*
 * PB XU Threads PFM events
 *
 *    XU Thread0: HID7(29:33) Lane 4
 *                HID7(34:38) Lane 5
 *                HID7(42:46) Lane 6
 *                HID7(47:51) Lane 7
 *
 *    XU Thread1: HID7(03:07) Lane 0
 *                HID7(08:12) Lane 1
 *                HID7(16:20) Lane 2
 *                HID7(21:25) Lane 3
 */
#define PB_XU_PFM_EVENT_MFSPR_CYCLES             0x0
#define PB_XU_PFM_EVENT_MULT_DIV_STALLS          0x1
#define PB_XU_PFM_EVENT_DERAT_MISS               0x2
#define PB_XU_PFM_EVENT_STORE_REQUEST            0x3
#define PB_XU_PFM_EVENT_LOAD                     0x4
#define PB_XU_PFM_EVENT_LOAD_MISS                0x5
#define PB_XU_PFM_EVENT_MISALIGNED_FLUSH         0x6
#define PB_XU_PFM_EVENT_STQ_FULL_FLUSH           0x7
#define PB_XU_PFM_EVENT_STCX_BOT_OF_STQ          0x8
#define PB_XU_PFM_EVENT_SYNC_BOT_OF_STQ          0x9
#define PB_XU_PFM_EVENT_STCX_FAIL_COUNT          0xa   // 10
#define PB_XU_PFM_EVENT_LOAD_MISS_LHS            0xb   // 11
#define PB_XU_PFM_EVENT_LHR_NON_DCBT             0xc   // 12
#define PB_XU_PFM_EVENT_LOAD_MISS_LHR            0xd   // 13
#define PB_XU_PFM_EVENT_DCBT_SENT                0xe   // 14
#define PB_XU_PFM_EVENT_DCBT_HITS_L1             0xf   // 15

/*
 * PB VSU Threads PFM events
 *
 *    VSU Thread0: HID7(16:20) Lane 2 
 *                 HID7(21:25) Lane 3
 *                 HID7(29:33) Lane 4
 *                 HID7(34:38) Lane 5
 *                 HID7(42:46) Lane 6
 *                 HID7(47:51) Lane 7
 *
 *    VSU Thread1: HID7(03:07) Lane 0
 *                 HID7(08:12) Lane 1
 *                 HID7(16:20) Lane 2
 *                 HID7(21:25) Lane 3
 *                 HID7(29:33) Lane 4
 *                 HID7(34:38) Lane 5
 */
#define PB_VSU_PFM_EVENT_TWO_INSTR_COMMIT        0x0
#define PB_VSU_PFM_EVENT_SLOT_0_COMMIT           0x1
#define PB_VSU_PFM_EVENT_SLOT_1_COMMIT           0x2
#define PB_VSU_PFM_EVENT_SIMPLE_COMMIT           0x3
#define PB_VSU_PFM_EVENT_VMX_SP_COMMIT           0x4
#define PB_VSU_PFM_EVENT_TBL_COMMIT              0x5
#define PB_VSU_PFM_EVENT_PERM_COMMIT             0x6
#define PB_VSU_PFM_EVENT_FPU_LOAD_COMMIT         0x7
#define PB_VSU_PFM_EVENT_VMX_LOAD_COMMIT         0x8
#define PB_VSU_PFM_EVENT_VMX_STORE_COMMIT        0x9
#define PB_VSU_PFM_EVENT_OTHER_STALL             0xa   // 10
#define PB_VSU_PFM_EVENT_DIV_SQRT_STALL          0xb   // 11
#define PB_VSU_PFM_EVENT_DENORM_STALL            0xc   // 12
#define PB_VSU_PFM_EVENT_FPU_NAN_STALL           0xd   // 13
#define PB_VSU_PFM_EVENT_DOT_PRODUCT             0xe   // 14
#define PB_VSU_PFM_EVENT_VMX_ABNORMAL_RES        0xf   // 15

/*
 * PB MMU PFM events
 *
 *    MMU Thread: HID7(03:07) Lane 0
 *                HID7(08:12) Lane 1
 *                HID7(16:20) Lane 2
 *                HID7(21:25) Lane 3
 *                HID7(29:33) Lane 4
 *                HID7(34:38) Lane 5
 *                HID7(42:46) Lane 6
 *                HID7(47:51) Lane 7
 */
#define PB_MMU_PFM_EVENT_TLBIE_AGGREGATE         0x0
#define PB_MMU_PFM_EVENT_LOCAL_TLBIE             0x1
#define PB_MMU_PFM_EVENT_BUS_TLBIE               0x2
#define PB_MMU_PFM_EVENT_PAGE_FAULT              0x3
#define PB_MMU_PFM_EVENT_TABLEWALK               0x4
#define PB_MMU_PFM_EVENT_TABLEWALK_CYCLES        0x5
#define PB_MMU_PFM_EVENT_TLB_HIT                 0x6
#define PB_MMU_PFM_EVENT_SLB_MISS                0x7
#define PB_MMU_PFM_EVENT_SLBIE                   0x8
#define PB_MMU_PFM_EVENT_SLBIA                   0x9
#define PB_MMU_PFM_EVENT_SPR_READ                0xa   //10
#define PB_MMU_PFM_EVENT_SPR_WRITE               0xb   //11

/*
 * PB Default PFM events
 *   always enabled for lane 0-7
 */
#define PB_DEFAULT_PFM_EVENT_T0_IL1_MISS_CYCLES  0x13  // 19
#define PB_DEFAULT_PFM_EVENT_T0_PPC_COMMIT       0x14  // 20
#define PB_DEFAULT_PFM_EVENT_T0_LOAD_MISS        0x15  // 21
#define PB_DEFAULT_PFM_EVENT_T1_IL1_MISS_CYCLES  0x16  // 22
#define PB_DEFAULT_PFM_EVENT_T1_PPC_COMMIT       0x17  // 23
#define PB_DEFAULT_PFM_EVENT_T1_LOAD_MISS        0x18  // 24


typedef unsigned long LOGICAL;
typedef unsigned long ULONG;
typedef unsigned long DWORD;

////////////////////////////////////////////////////////////////////
//
// Helpful enums
//

typedef enum
{
    PMC_THREAD0=0,
    PMC_THREAD1,
    PMC_THREAD_INVALID
}ePMCThreadID;

typedef enum
{
    PMC_UNIT_IU=0,
    PMC_UNIT_XU,
    PMC_UNIT_VSU,
    PMC_UNIT_MMU,
    PMC_UNIT_DEFAULT,
    PMC_UNIT_INVALID
}ePMCUnitID;


////////////////////////////////////////////////////////////////////
//
// Register structures
//

//
// HID7 bits
//

// "enable"s select a UNIT 
// set Enable's with PB_PFM_BUS_LANE_0_1_*'s
// Set lane*Select's with unit events
typedef union _PMCHID7
{
    struct
    {
        unsigned __int64 lane01Enable      :3;
        unsigned __int64 lane0Select       :5;
        unsigned __int64 lane1Select       :5;

        unsigned __int64 lane23Enable      :3;
        unsigned __int64 lane2Select       :5;
        unsigned __int64 lane3Select       :5;

        unsigned __int64 lane45Enable      :3;
        unsigned __int64 lane4Select       :5;
        unsigned __int64 lane5Select       :5;

        unsigned __int64 lane67Enable      :3;
        unsigned __int64 lane6Select       :5;
        unsigned __int64 lane7Select       :5;

        unsigned __int64 _padding          :12;  // 52:63
    };
    
    unsigned __int64 asQW;
}PMCHID7, *PPMCHID7;

#define PMCHID7_SETLANE4SELECT(hid7, x)     (hid7).lane4Select = (x)
#define PMCHID7_GETLANE4SELECT(hid7)        ((hid7).lane4Select)

typedef union _PMCHID5
{
    struct
    {
        unsigned long _reserved0        :16;
        unsigned long trig0             :3;
        unsigned long trig1             :3;
        unsigned long trig2             :3;
        unsigned long trig3             :3;
        unsigned long _reserved1        :1;
        unsigned long tracetrig0        :1;
        unsigned long tracetrig1        :1;
        unsigned long _reserved2        :1;

        unsigned long _padding          :32;
    };
    
    unsigned __int64 asQW;
}PMCHID5, *PPMCHID5;

//
// PFM control register
//

typedef union _PFMControl
{
    struct
    {
        unsigned long enable        :1; 
        unsigned long stopAtMax     :1; // 1 - stop event counters at max values
        unsigned long freeze        :2;
            // freeze - 00 do not freeze
            //          01 freeze ctrs and interval ctr on any ctr overflow except interval ctr
            //          10 freeze ctrs and interval ctr on any ctr overflow including interval ctr            
        unsigned long mode0         :2; // count mode PB0
            // mode   - 00 supervisor
            //          01 hypervisor
            //          10 problem
            //          11 any of the above
        unsigned long mode1         :2; // count mode PB1
        unsigned long mode2         :2; // count mode PB2
        unsigned long traceMode     :2; 
            // tracemode - 00 disable tracing
            //             01 TLA or to debug IOs every interval timer 
            //             10 partial. lower 4 bits to TLA or Debug IOs (max rate)
        unsigned long traceDest     :1; // 0- TLA (2Ghz), 1- debug IOs (500mhz)
        unsigned long countQual     :2; // 00 - none, 01- PMC0, 10- PMC4
        
        unsigned long _padding0     :16; // 15:63
        unsigned long _padding1     :32; 
    };
    
    unsigned __int64 asQW;
}PFMControl, *PPFMControl;


//
// PFM Status register
//
// Read returns status - 0 no overflow since last READ
//                       1 overflow since last READ
// Write sets overflow interrupt enable
//                       0 interrupt disabled
//                       1 interrupt enabled
typedef union _PFMStatus
{
    struct
    {
        // status and interrupt enable bits
        unsigned long SI0 : 1;
        unsigned long SI1 : 1;
        unsigned long SI2 : 1;
        unsigned long SI3 : 1;
        unsigned long SI4 : 1;
        unsigned long SI5 : 1;
        unsigned long SI6 : 1;
        unsigned long SI7 : 1;
        unsigned long SI8 : 1;
        unsigned long SI9 : 1;
        unsigned long SI10: 1;
        unsigned long SI11: 1;
        unsigned long SI12: 1;
        unsigned long SI13: 1;
        unsigned long SI14: 1;
        unsigned long SI15: 1;
        unsigned long SI16: 1; // interval timer

        unsigned long _padding0:15; // 17:63
        unsigned long _padding1:32;
    };
    
    unsigned __int64 asQW;
}PFMStatus;


//
// PFM Input Selection register
//

// use this to override standard lanes with triggers 
typedef union _PFMInputSelect
{
    struct
    {
        unsigned long IS0 : 1;
            // 0 - PB1 lane 0
            // 1 - PB0 SPR trigger 1
        unsigned long IS1 : 1;
            // 0 - PMW lane 0
            // 1 - PB0 SPR trigger 1
        unsigned long IS2 : 1;
            // 0 - PB1 Lane 7
            // 1 - PB0 SPR trigger 2
        unsigned long IS3 : 1;
            // 0 - PMW Lane 8
            // 1 - PB0 SPR trigger 2
        unsigned long IS4 : 1;
            // 0 - PB1 Lane 3
            // 1 - PB0 thread 0 IABR trigger
        unsigned long IS5 : 1;
            // 0 - XIF lane 0
            // 1 - PB0 thread 0 IABR trigger
        unsigned long IS6 : 1;
            // 0 - PB1 Lane 4
            // 1 - PB0 thread 1 IABR trigger            
        unsigned long IS7 : 1;
            // 0 - XIF Lane 4
            // 1 - PB0 thread 1 IABR trigger            
        unsigned long IS8 : 1;
            // 0 - PB2 Lane 0
            // 1 - PB1 SPR trigger 1
        unsigned long IS9 : 1;
            // 0 - PMW Lane 1
            // 1 - PB1 SPR trigger 1
        unsigned long IS10: 1;
            // 0 - PB2 Lane 7
            // 1 - PB1 SPR trigger 2
        unsigned long IS11: 1;
            // 0 - PMW Lane 9
            // 1 - PB1 SPR trigger 2
        unsigned long IS12: 1;
            // 0 - PB2 Lane 3
            // 1 - PB1 thread 0 IABR trigger            
        unsigned long IS13: 1;
            // 0 - XIF Lane 1
            // 1 - PB1 thread 0 IABR trigger            
        unsigned long IS14: 1;
            // 0 - PB2 Lane 4
            // 1 - PB1 thread 1 IABR trigger            
        unsigned long IS15: 1;
            // 0 - XIF Lane 5
            // 1 - PB1 thread 1 IABR trigger            
        unsigned long IS16: 1;
            // 0 - PB0 Lane 0
            // 1 - PB2 SPR trigger 1
        unsigned long IS17: 1;
            // 0 - PMW Lane 2
            // 1 - PB2 SPR trigger 1
        unsigned long IS18: 1;
            // 0 - PB0 Lane 7
            // 1 - PB2 SPR trigger 2
        unsigned long IS19: 1;
            // 0 - PMW Lane 10
            // 1 - PB2 SPR trigger 2
        unsigned long IS20: 1;
            // 0 - PB0 Lane 3
            // 1 - PB2 thread 0 IABR trigger
        unsigned long IS21: 1;
            // 0 - XIF Lane 2
            // 1 - PB2 thread 0 IABR trigger
        unsigned long IS22: 1;
            // 0 - PB0 Lane 4
            // 1 - PB2 thread 1 IABR trigger
        unsigned long IS23: 1;
            // 0 - XIF Lane 6
            // 1 - PB2 thread 1 IABR trigger

        unsigned long _padding0 :8; // 24:63
        unsigned long _padding1 :32; 
    };
    
    unsigned __int64 asQW;
}PFMInputSelect;

//
// PFM Trigger Start/Stop Register
//

#define PFM_TRIGGER_CONTROL_NONE               0x0
#define PFM_TRIGGER_CONTROL_PB0_SPR_1          0x1
#define PFM_TRIGGER_CONTROL_PB0_SPR_2          0x2
#define PFM_TRIGGER_CONTROL_PB0_T0_IABR        0x3
#define PFM_TRIGGER_CONTROL_PB0_T0_DABR        0x4
#define PFM_TRIGGER_CONTROL_PB0_T1_IABR        0x5
#define PFM_TRIGGER_CONTROL_PB0_T1_DABR        0x6

#define PFM_TRIGGER_CONTROL_PB1_SPR_1          0x7
#define PFM_TRIGGER_CONTROL_PB1_SPR_2          0x8
#define PFM_TRIGGER_CONTROL_PB1_T0_IABR        0x9
#define PFM_TRIGGER_CONTROL_PB1_T0_DABR        0xa
#define PFM_TRIGGER_CONTROL_PB1_T1_IABR        0xb
#define PFM_TRIGGER_CONTROL_PB1_T1_DABR        0xc

#define PFM_TRIGGER_CONTROL_PB2_SPR_1          0xd
#define PFM_TRIGGER_CONTROL_PB2_SPR_2          0xe
#define PFM_TRIGGER_CONTROL_PB2_T0_IABR        0xf
#define PFM_TRIGGER_CONTROL_PB2_T0_DABR        0x10
#define PFM_TRIGGER_CONTROL_PB2_T1_IABR        0x11
#define PFM_TRIGGER_CONTROL_PB2_T1_DABR        0x12

#define PFM_TRIGGER_CONTROL_TLC                0x13

typedef union _PFMTriggerStartStop
{
    struct
    {
        unsigned long bank0Start        : 5;
        unsigned long bank1Start        : 5;
        unsigned long bank23Start       : 5;

        unsigned long bank0Stop         : 5;
        unsigned long bank1Stop         : 5;
        unsigned long bank23Stop        : 5;

        unsigned long _padding0         : 2;
        unsigned long _padding1         : 32;
    };

    unsigned __int64 asQW;
}PFMTriggerStartStop;


////////////////////////////////////////////////////////////////////
//
// PMW MMIO registers
//

//
// This is a map of how the PMW bus is arranged.  L2 signals must pass 
// through BIU, so make sure no enables are turned on for a given lane in BOTH 
// L2 and BIU.  
//
// Basically, avoid setting a bitlane enabled for more than one unit
//
//                      L2->BIU\
// NCU1 -> NCU2 -> NCU0 -> ->   ->
//                       /
//              CIU Slice
//

//
// CIU and NCU use same setup logic
//

typedef union _PMWCIUNCUControl
{
    struct
    {
        unsigned long master_en         :1; // enables pass through
        unsigned long unit_en           :1; // enables base lane -> rotator
        unsigned long _r1               :6;
        unsigned long unit_bit_en       :16;// enable base lane
        unsigned long unit_group_sel    :4; // rotate LEFT 
        unsigned long _r2               :4;
        unsigned long _r3               :32;        
    };
    unsigned __int64 asQW; // they don't tell me the size.  god i hope i'm right.
}PMWCIUNCUControl;



typedef union _PMWBIUControl
{
    struct
    {
        unsigned long master_en         :1;
        unsigned long unit_en           :1;// FAKE, easier to make macros
        unsigned long _r1               :5;
        unsigned long alternate_en      :1;
        unsigned long unit_bit_en       :16;
        unsigned long unit_group_sel    :4;
        unsigned long _r2               :4;
        unsigned long _r3               :32;        
        
    };

    unsigned __int64 asQW; // XXX 
}PMWBIUControl;


//
// IBM did not deign to name the registers something useful, and 
// since i'd rather avoid all confusion, the L2 regs are named after their
// addresses
//
typedef union _PMWL2Control310
{
    unsigned __int64 asQW;
    struct
    {
        // 
        // these AND with the 50 L2 event signals
        // coming from the L2 event logic.
        //
        // "upper" controls 0:7 of the L2 perfmon bus, A-Z but not L
        // "lower" controls 8:15 of the perfmon bus, a-z but not l
        //
        // After the event is selected, it's bit lane is SET. you must then
        // enable that bit lane in L2Control2.unit_bit_en
        //
        
        unsigned long upper_mode_sel        :25;
        unsigned long lower_mode_sel_a      :7;
        unsigned long lower_mode_sel_b      :18;
        unsigned long _r1                   :14;
    };
}PMWL2Control310;


typedef union _PMWL2Control318
{
    struct
    {
        unsigned long unit_en            :1;// FAKE, easier to make macros
        unsigned long _r1                :31;
        unsigned long _r2                :15;
        unsigned long master_en          :1; // 47 hopefully
        // 48:64
        unsigned long unit_bit_en        :16;
    };
    unsigned __int64 asQW;
}PMWL2Control318;

//
// L2 events
//
// you can choose an uppercase group and a lower case group.
// PMCAddL2Event will make sure you don't have more than
// one set up per group.
//
// The lack of "L" group is intentional.
//

#define _BITZERO64_ 0x8000000000000000
#define _BITZERO16_ 0x8000

#define PMW_L2_EVENT_GROUP_A    0
#define PMW_L2_EVENT_GROUP_B    1
#define PMW_L2_EVENT_GROUP_C    2
#define PMW_L2_EVENT_GROUP_D    3
#define PMW_L2_EVENT_GROUP_E    4
#define PMW_L2_EVENT_GROUP_F    5
#define PMW_L2_EVENT_GROUP_G    6
#define PMW_L2_EVENT_GROUP_H    7
#define PMW_L2_EVENT_GROUP_I    8
#define PMW_L2_EVENT_GROUP_J    9
#define PMW_L2_EVENT_GROUP_K    10
#define PMW_L2_EVENT_GROUP_M    11
#define PMW_L2_EVENT_GROUP_N    12
#define PMW_L2_EVENT_GROUP_O    13
#define PMW_L2_EVENT_GROUP_P    14
#define PMW_L2_EVENT_GROUP_Q    15
#define PMW_L2_EVENT_GROUP_R    16
#define PMW_L2_EVENT_GROUP_S    17
#define PMW_L2_EVENT_GROUP_T    18
#define PMW_L2_EVENT_GROUP_U    19
#define PMW_L2_EVENT_GROUP_V    20
#define PMW_L2_EVENT_GROUP_W    21
#define PMW_L2_EVENT_GROUP_X    22
#define PMW_L2_EVENT_GROUP_Y    23
#define PMW_L2_EVENT_GROUP_Z    24
#define PMW_L2_EVENT_GROUP_a    25
#define PMW_L2_EVENT_GROUP_b    26
#define PMW_L2_EVENT_GROUP_c    27
#define PMW_L2_EVENT_GROUP_d    28
#define PMW_L2_EVENT_GROUP_e    29
#define PMW_L2_EVENT_GROUP_f    30
#define PMW_L2_EVENT_GROUP_g    31
#define PMW_L2_EVENT_GROUP_h    32
#define PMW_L2_EVENT_GROUP_i    33
#define PMW_L2_EVENT_GROUP_j    34
#define PMW_L2_EVENT_GROUP_k    35
#define PMW_L2_EVENT_GROUP_m    36
#define PMW_L2_EVENT_GROUP_n    37
#define PMW_L2_EVENT_GROUP_o    38
#define PMW_L2_EVENT_GROUP_p    39
#define PMW_L2_EVENT_GROUP_q    40
#define PMW_L2_EVENT_GROUP_r    41
#define PMW_L2_EVENT_GROUP_s    42
#define PMW_L2_EVENT_GROUP_t    43
#define PMW_L2_EVENT_GROUP_u    44
#define PMW_L2_EVENT_GROUP_v    45
#define PMW_L2_EVENT_GROUP_w    46
#define PMW_L2_EVENT_GROUP_x    47
#define PMW_L2_EVENT_GROUP_y    48
#define PMW_L2_EVENT_GROUP_z    49


//
// CIU Events
//
/*
static char * CIUEvents[] =
{
0   "CIU STQ Full P0",
1   "CIU STQ Not Empty & L2_STQ not avail P0",
2   "CIU Slice Load Queue Full P0",
3   "CIU Load Req to L2 canceled by LHS P0",
4   "CIU STQ Full P0",
5   "CIU STQ Not Empty & L2_STQ not avail P0",
6   "CIU Slice Load Queue Full P0",
7   "CIU Load Req to L2 canceled by LHS P0",
8   "CIU STQ Full P1",
9   "CIU STQ Not Empty & L2_STQ not avail P1",
10  "CIU Slice Load Queue Full P1",
11  "CIU Load Req to L2 canceled by LHS P1",
12  "CIU STQ Full P2",
13  "CIU STQ Not Empty & L2_STQ not avail P2",
14  "CIU Slice Load Queue Full P2",
15  "CIU Load Req to L2 canceled by LHS P2",    
};

static char * NCUEvents[] =
{
0   "448 NCU ST Req (E)",
1   "466 NCU STQ Full (C)",
2   "452 NCU Non-Ugly ST (E)",
3   "453 NCU Non-Guarded ST (E)",
4   "463 NCU ST Buffer Full (C)",
5   "460 NCU STQ Last Gather (E)",
6   "450 NCU STQ Last SHS (C)",
7   "451 NCU STQ Wait (C)",
8   "456 NCU STQ Last Sync (C)",
9   "457 NCU STQ Last LWSync (C)",
10  "458 NCU STQ Last EIEIO (C)",
11  "461 NCU Load Cancel (E)",
12  "462 NCU LDQ LHS (E)",
13  "464 NCU LDQ LD (E)",
14  "475R NCU ICBI Valid (C)",
15  "476 NCU TLBIE Valid (C)"        
};

static char * BIUEvents[] =
{
0   "387(E) Retry issued by BIU",
1   "387(E) Retry issued by BIU",
2   "389(C) Only 2 stations are busy ",
3   "391(C) All 3 stations are busy ",
4   "391(C) Any of 3 command stations are busy",
5   "393(C) All 3 stations stalled",
6   "394(E) Rcv 8-beat data from MPi",
7   "395(E) Send 8-beat data to MPi",
8   "396(E) Send Addr_cmd(incl retries)",
9   "397(C) cycles bet data req and grant",
10  "403(C) NCU ld q not empty (1)",
11  "405(C) Castout q full(8)",
12  "406(C) Push q full(4)",
13  "407(C) NCU G=0 stq full(4)",
14  "13A(E) Castout/Push retry",
15  "13A(E) Castout/Push retry",

    // ALT EVENTS START at 16
        
0   "(E) Loads from PM MMIO space",
1   "(E) Stores to PM MMIO space",
2   "(E) Only 2 stations busy",
3   "(E) All 3 stations busy",
4   "(E) All 3 stations stalled",
5   "(C) No credit for a command",
6   "(C) RC Cmd q not empty (8)",
7   "(C) Castout Cmd q not empty (8)",
8   "(C) Push Cmd Q not empty (4)",
9   "(C) NCU St Cmd q not empty (5)",
10  "(C) RC Cmd q full (8)",
11  "(C) Castout Data Buffer full",
12  "(C) Push/Intervention Data Buffer full",
13  "(C) NCU G=0 St Data Buffer full",
14  "reserved",
15  "reserved",

};
*/

#define PMWClearEnables(x)          (x).unit_bit_en = 0;(x).unit_group_sel=0
#define PMWSetEnable(x, lane)       (x).unit_bit_en |= (_BITZERO16_ >> lane)
#define PMWSetRotate(x, rot)        (x).unit_group_sel = rot


////////////////////////////////////////////////////////////////////
//
// Library state
//

//
// PFMCounterControls[] controls the giant muxes in front of the
// performance counters.  Use PMCAssignBusToPMC to assign a unit 
// and lane.  It will return FALSE if that is not possible.
//
// e.g. if want to set pmc8, it lives in bank2, so bits 0-7 are the
//      lanes coming out of PB1
//

typedef union _PFMCounterControl
{
    struct
    {
        unsigned long inputSelect       :5;
        unsigned long countEnable       :1;
        unsigned long polarity          :1; // 0 count when zero, or negative transition
        unsigned long countCycles       :1; // THIS IS SET BY PMCSetCounterControl, don't touch!
        
        unsigned long _padding0         : 24;
        unsigned long _padding1         : 32;
    };

    unsigned __int64 asQW;
}PFMCounterControl;

// there are 16 of these, one for each counter
extern PFMCounterControl PFMCounterControls[];

/*
    BANK0 (PMC0-PMC3)
        0 -  7  - PB0 Lanes
        8 - 15  - PB1 Lanes
        16- 19  - PMW Lane bits 0-3
        20- 31  - PMW Lane bits 4-15
    BANK1 (PMC4-PMC7)
        0 -  7  - PB0 Lanes
        8 - 15  - PB2 Lanes
        16- 19  - PMW Lane bits 0-3        
        20- 31  - PMW Lane bits 4-15
    BANK2 (PMC8-PMC11)
        0 -  7  - PB1 Lanes
        8 - 15  - PB2 Lanes
        16- 27  - PMW Lane bits 4-15
        28- 31  - XIF Lane bits 0-3
    BANK3 (PMC12-PMC15)
        0 -  7  - PB0 Lanes
        8 - 11  - PMW Lane bits 0-3        
        12- 23  - PMW Lane bits 4-15
        24- 31  - XIF Lane bits 0-7
 */

typedef enum
{
    PMCSourcePB0,
    PMCSourcePB1,
    PMCSourcePB2,
    PMCSourcePMW,
    PMCSourceXIF,
}ePMCSourceUnit;


//
// PMCSTATE
//
// This is the state saved from the pfm block with PMCGetCounters
// The physical counters are 32bit, but we keep 64 bits so we can 
// accumulate into this structure and use the same routines to dump
// them.
//
typedef struct _PMCState
{
    unsigned long long  pmc[16];    // pmc values
    __int64             ticks;      // timebase ticks measured    
    double              interval;   // core cycles measured (only 32b).  
                                    // if it overflows we switch to converting ticks.
    PFMStatus           pfmstatus;  // overflow status

    //
    // helpful values extracted by PMCDumpCountersVerbose
    //
    __int64         instrCount;
}PMCState, *PPMCState;
    

////////////////////////////////////////////////////////////////////
//
// Low level access APIs
//
// SPR1/2 are used to trigger all 4 banks simultaneously
//

/*
    Before doing anything, call PMCEnable();

    Counting with this library is controlled by the following registers:
    
    1) HID7 for core event selection.  
       Set up a hid7, then call PMCSetEvents()
    2) PFMCounterControls for bus -> counter selection 
       Use PMCAssignBusToPMC(), then call PMCSetCounterControl()

    Use PMCDumpConfig to sanity check your setup.

    Once set up you can profile with:

    PMCResetCounters();
    PMCStart();

    ... code to profile goes here ...
    
    PMCStop();
    PMCGetCounters(&pmcstate);
    PMCDumpCountersVerbose(&pmcstate, 0);
    

    For setting up the L2 counters, here's sample code for counters 8-15:

    PMCSetPMWFor(PMWUnitL2);
    // PMW events
    PMCAssignBusToPMC(PMCSourcePMW, 12, 8);
    PMCAssignBusToPMC(PMCSourcePMW, 9, 9);
    PMCAssignBusToPMC(PMCSourcePMW, 10, 10);
    PMCAssignBusToPMC(PMCSourcePMW, 11, 11);
    PMCAssignBusToPMC(PMCSourcePMW, 2, 12);
    PMCAssignBusToPMC(PMCSourcePMW, 3, 13);
    PMCAssignBusToPMC(PMCSourcePMW, 4, 14);
    PMCAssignBusToPMC(PMCSourcePMW, 5, 15);

    l2control310.asQW = 0;
    PMCAddL2Event(PMW_L2_EVENT_GROUP_o, &l2control310);
    PMCAddL2Event(PMW_L2_EVENT_GROUP_A, &l2control310);
    PMWSetReg(PMW_L2_CONTROL_REG_310_ADDR, l2control310.asQW);

    PMCSetCounterControl();
    
*/


//
// PMCEnable
//
// enable cpu perf counters, reset PMCPB.lib internal state, notably
// PFMCounterControls[]
// 
// configures triggers to SPR1(start)/SPR2(stop) for all 4 banks
//
void PMCEnable();

//
// PMCSetTriggerProcessor
//
// For simplicity's sake, we can just let one processor control the triggers
// This API selects which physical processor gets to control the triggers.
// 
//
void PMCSetTriggerProcessor(unsigned long processor);


//
// select which incoming bus to count
//
// This is such a pain to set up.  
// PMCAssignBusToPMC will attempt to take a bitlane from
// a specified unit and route it to counter. 
//
// Will ENABLE the counter and set to count edges, which is
// the only thing that appears to work.  After all the inputs
// have been selected, call PMCSetCounterControl()
//
// Returns:
//  TRUE    if successful
//  FALSE   if bitlane cannot be routed to that counter
//

LOGICAL PMCAssignBusToPMC(ePMCSourceUnit sourceUnit, unsigned long bitlane, unsigned counter);

//
// PMCGetCounters
//
// Populates the PMCState structure provided
// Assumes the counters are STOPPED with PMCStop
//
void PMCGetCounters(PPMCState pmcstate);

//
// PMCGetCounter
//
// Get specific counter
//
ULONG PMCGetCounter(ULONG reg);


//
// PMCResetCounters
//
// Resets the PMCs, the interval timer (including interval
// reload register).  Also clears all status bits.
//
void PMCResetCounters();

//
// PMCSetEvents
//
// Writes a populated PMCHID7 structure to HID7
//
void PMCSetEvents(PMCHID7 hid7);

//
// PMCAddL2Event
//
// this api will OR in the appropriate bit to l2control1 
// and check for lane conflicts
//
void PMCAddL2Event(unsigned long l2EventGroup, PMWL2Control310* l2control310);
    

//
// PMCSetCounterControl
//
// Writes the contents of PFMCounterControls to 
// the counter control registers in the pervasive unit
//
void PMCSetCounterControl();

//
// PMCSetCountMode
//
// Set to count in supervisor only, hypervisor only, or any
// system state
//
// Defaults to MODE_ANY
//
#define PFMCONTROL_MODE_SUPERVISOR      0
#define PFMCONTROL_MODE_HYPERVISOR      1
#define PFMCONTROL_MODE_ANY             3

void PMCSetCountMode(unsigned long mode);


//
// PMCStart
//
// Toggle SPR1 bit to start counters on all banks
// Sets counters for all of hypervisor, supervisor and user states
// Records current time base tick count
//
void PMCStart();

//
// PMCStop
//
// Records current timebase tick count
// Fires SPR2 trigger to stop counters
// For good measure, disables counters by writing to PFMControl enable
// 
void PMCStop();

//
// PMCDumpCounters
//
// barebones dump of pmcstate.  for friendlier output (incl. event names)
// use PMCDumpCountersVerbose
//
void PMCDumpCounters(PPMCState pmcstate);
    
//
// PMCSetBIUPassThrough
//
// if you care not about BIU events, then set pass thru to allow all
// L2 events to get counted.
//
// This is currently the easiest way to set up for L2 events
//

typedef enum
{
    PMWUnitNCU1=0,
    PMWUnitNCU2,
    PMWUnitNCU0,
    PMWUnitBIU,
    PMWUnitCIU,
    PMWUnitL2,
    PMWNumberOfUnits
}ePMWUnit;


#define PMCSetBIUPassThrough()  PMCSetPMWFor(PMWUnitL2)
// helper to setup pmw to count just one unit
void PMCSetPMWFor(ePMWUnit pmwUnit);

//
// PMCComputeFrequency
//
// spend a second to figure out our clock speed
//
__int64 PMCComputeFrequency();


////////////////////////////////////////////////////////////////////
//
// Helpful APIs.  May suck in many string constants, will want to 
// ifdef away to avoid massive bloat.
//

//
// PMCDumpCountersVerbose
//
// Friendly dump with counter event printed.  Sucks in many strings.
//

#define PMC_VERBOSE_NOL2ECC         0x1
#define PMC_VERBOSE_NOFRAMERATE     0x2
#define PMC_VERBOSE_NOTIME          0x4
#define PMC_VERBOSE_MIN             0xFFFFFFFF
#define PMC_VERBOSE_MAX             0x0

void PMCDumpCountersVerbose(PPMCState pmcstate, unsigned long verbosity);

//
// Global spew control
//
void PMCSetVerboseLevel(unsigned long level);

//
// Sanity checking
//
// Dumps everything we can tell about the configuration of the 
// perfmon block.  does not handle overrides via inputselect register
//
void PMCDumpConfig();


////////////////////////////////////////////////////////////////////
//
// Unfriendly low level APIs
//
void PMCSetHID5(PMCHID5 hid5);


// XXX

//
// PMW registers
//
// These registers don't have names yet.
// when new documentation comes in, we'll need to double check the 
// register names and offsets
//

#define PMW_BIU_CONTROL_REG_ADDR                0xB50
#define PMW_CIU_CONTROL_REG_ADDR                0x708
#define PMW_L2_CONTROL_REG_318_ADDR             0x318
#define PMW_L2_CONTROL_REG_310_ADDR             0x310
#define PMW_NCU0_CONTROL_REG_ADDR               0x480
#define PMW_NCU1_CONTROL_REG_ADDR               0x580
#define PMW_NCU2_CONTROL_REG_ADDR               0x680

//
// KeSetPMWRegister is used to access those registers.
//
extern void
KeSetPMWRegister(unsigned long addr, unsigned long long value);

#define PMWSetReg KeSetPMWRegister


//
// Dump L2 Error count registers
//
// Useful to see if your CPU is flakey.
//
// It will display the number of L2 correctable errors
// since the last hang pulse. Counters saturate at 63
//
void PMCDumpL2ECCErrorCount();

#ifdef __cplusplus
}
#endif


#pragma warning(pop)


#endif
