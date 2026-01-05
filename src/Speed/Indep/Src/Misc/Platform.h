#ifndef MISC_PLATFORM_H
#define MISC_PLATFORM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// TODO I just made this up to store declarations for Platform.cpp

void InitPlatform();
void InitDisplaySystem();
void ServicePlatform();
int bDoWithStack(void *function, void *stack_pointer, int arg1, int arg2);
void EnableInterrupts();
void DVDErrorTask(void *, int);

#endif
