#ifndef FRONTEND_MENUSCREENS_LOADING_FEBOOTFLOWMANAGER_H
#define FRONTEND_MENUSCREENS_LOADING_FEBOOTFLOWMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"

struct BootFlowScreen : public bTNode<BootFlowScreen> {
    const char *Name;  // offset 0x8
};

struct BootFlowManager {
    static void Init();
    static void Destroy();
    static BootFlowManager *Get();
    virtual ~BootFlowManager();
    void JumpToHead();
    bool JumpToScreen(const char *screen_name);

    bTList<BootFlowScreen> BootFlowScreens; // offset 0x0
    BootFlowScreen *CurrentScreen;          // offset 0x8

    static BootFlowManager *mInstance;
};

#endif
