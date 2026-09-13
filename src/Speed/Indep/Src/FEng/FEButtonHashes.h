#ifndef FEBUTTONHASHES_H_
#define FEBUTTONHASHES_H_
#include "types.h"

/*
const u32 FEHash_Button_Highlight;        // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:18
const u32 FEHash_Button_UnHighlight;      // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:19
const u32 FEHash_Button_Pressed;          // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:20
const u32 FEHash_Button_Released;         // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:21
const u32 FEHash_Pad_Control_Passed;      // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:24
const u32 FEHash_Pad_Up;                  // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:27
const u32 FEHash_Pad_UpRight;             // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:28
const u32 FEHash_Pad_Right;               // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:29
const u32 FEHash_Pad_DownRight;           // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:30
const u32 FEHash_Pad_Down;                // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:31
const u32 FEHash_Pad_DownLeft;            // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:32
const u32 FEHash_Pad_Left;                // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:33
const u32 FEHash_Pad_UpLeft;              // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:34
const u32 FEHash_Pad_Start;               // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:37
const u32 FEHash_Pad_Back;                // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:38
const u32 FEHash_Pad_Accept;              // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:39
const u32 FEHash_Pad_LTrigger;            // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:40
const u32 FEHash_Pad_RTrigger;            // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:41
const u32 FEHash_Pad_Button0;             // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:42
const u32 FEHash_Pad_Button1;             // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:43
const u32 FEHash_Pad_Button2;             // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:44
const u32 FEHash_Pad_Button3;             // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:45
const u32 FEHash_Pad_Button4;             // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:46
const u32 FEHash_Pad_Button5;             // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:47
const u32 FEHash_Pad_Button6;             // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:48
const u32 FEHash_Pad_Button7;             // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:49
const u32 FEHash_Pad_Button8;             // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:50
const u32 FEHash_Pad_Button9;             // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:51
const u32 FEHash_Pad_Quit;                // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:53
const u32 FEHash_Pad_LTrigger_Held;       // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:62
const u32 FEHash_Pad_RTrigger_Held;       // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:63
const u32 FEHash_Pad_Start_Released;      // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:66
const u32 FEHash_Pad_Back_Released;       // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:67
const u32 FEHash_Pad_Accept_Released;     // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:68
const u32 FEHash_Pad_LTrigger_Released;   // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:69
const u32 FEHash_Pad_RTrigger_Released;   // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:70
const u32 FEHash_Pad_Button0_Released;    // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:71
const u32 FEHash_Pad_Button1_Released;    // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:72
const u32 FEHash_Pad_Button2_Released;    // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:73
const u32 FEHash_Pad_Button3_Released;    // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:74
const u32 FEHash_Pad_Button4_Released;    // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:75
const u32 FEHash_Pad_Button5_Released;    // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:76
const u32 FEHash_Pad_Button6_Released;    // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:77
const u32 FEHash_Pad_Button7_Released;    // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:78
const u32 FEHash_Pad_Button8_Released;    // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:79
const u32 FEHash_Pad_Button9_Released;    // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:80
const u32 FEHash_SND_InGamePauseMenuOpen; // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:82
const u32 FEHash_SND_SliderLeft;          // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:83
const u32 FEHash_SND_SliderRight;         // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:84
const u32 FEHash_Mouse_Enter;             // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:87
const u32 FEHash_Mouse_Over;              // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:88
const u32 FEHash_Mouse_Exit;              // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:89
const u32 FEHash_Mouse_Left_Pressed;      // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:90
const u32 FEHash_Mouse_Left_Held;         // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:91
const u32 FEHash_Mouse_Left_Released;     // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:92
const u32 FEHash_Mouse_Right_Pressed;     // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:93
const u32 FEHash_Mouse_Right_Held;        // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:94
const u32 FEHash_Mouse_Right_Released;    // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:95
const u32 FEHash_Mouse_Wheel_Up;          // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:96
const u32 FEHash_Mouse_Wheel_Down;        // size: 0x4, Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEButtonHashes.h:97
*/

#endif
