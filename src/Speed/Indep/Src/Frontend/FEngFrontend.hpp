#ifndef FRONTEND_FENGFRONTEND_H
#define FRONTEND_FENGFRONTEND_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"

int FEngMapJoyParamToJoyport(int feng_param);

int FEngMapJoyportToJoyParam(int joyport);

uint32 FEngHashString(const char *fmt /* r4 */, ...);

void FEngSNMakeHidden(char *outBuffer /* r31 */, int32 out_buf_size /* r30 */, const char *strInput /* r5 */);

void FEngSNMakeHidden(char *outBuffer /* r31 */, int32 out_buf_size /* r30 */, uint16 *strInput /* r5 */);

uint32 FindButtonNameHashForFEString(int config /* r30 */, int string_number /* r31 */, JoystickPort player /* r5 */);

#endif
