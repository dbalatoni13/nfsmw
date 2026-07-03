#ifndef __SPEEDFENG_HPP__
#define __SPEEDFENG_HPP__

#include <types.h>
#include "Speed/Indep/Src/Misc/Joystick.hpp"

#define SCREEN_WIDTH 640.0f  // :25
#define SCREEN_HEIGHT 480.0f // :26

#define NUM_ENTRIES(_a_) (signed)(sizeof(_a_) / sizeof(*(_a_))) // :29

#define FEngDebugPrintf if (0) // :39

#define FEMSG_SCREEN_RENDER 0x32C72D8F  // :43
#define FEMSG_SCREEN_TICK 0xC98356BA    // :44
#define FEMSG_MOUSE_CHANGED 0X9803F6E2  // :45
#define FEMSG_MOVIE_FINISHED 0xC3960EB9 // :46
#define FEMSG_HIDE_PACKAGE 0x0AD4BBDC   // :47
#define FEMSG_SHOW_PACKAGE 0x18883F75   // :48
#define FEMSG_ERROR_STATE 0xd0678849    // :49
#define FEMSG_EXIT_STARTED 0x84378bef   // :50

#define FEOBJ_EVENT_HANDLER 0x47ff4e7c // :53

#define FEMSG_ENABLE_OPTION_INPUT 0x6481273e // :56

int FEngMapJoyParamToJoyport(int feng_param);

int FEngMapJoyportToJoyParam(int joyport);

uint32 FEngHashString(const char *fmt /* r4 */, ...);

void FEngSNMakeHidden(char *outBuffer /* r31 */, int32 out_buf_size /* r30 */, const char *strInput /* r5 */);

void FEngSNMakeHidden(char *outBuffer /* r31 */, int32 out_buf_size /* r30 */, uint16 *strInput /* r5 */);

uint32 FindButtonNameHashForFEString(int config /* r30 */, int string_number /* r31 */, JoystickPort player /* r5 */);

#endif
