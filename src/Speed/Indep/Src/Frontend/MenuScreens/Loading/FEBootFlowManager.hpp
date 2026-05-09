#ifndef FRONTEND_MENUSCREENS_LOADING_FEBOOTFLOWMANAGER_H
#define FRONTEND_MENUSCREENS_LOADING_FEBOOTFLOWMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"

class BootFlowScreen : public bTNode<BootFlowScreen> {
  public:
    // virtual ~BootFlowScreen() {}

    const char *Name; // offset 0x8
};

class BootFlowManager {
  public:
    static void Init();
    static void Destroy();
    static BootFlowManager *Get();
    void ChangeToNextBootFlowScreen(int mask);
    bool JumpToScreen(const char *screen_name);
    void JumpToHead();
    bool DoAttract();

  private:
    BootFlowManager();
    virtual ~BootFlowManager() {};
    BootFlowScreen *FindScreen(const char *name);
    BootFlowScreen *FindScreenSubStr(const char *name);

    static BootFlowManager *mInstance;

    bTList<BootFlowScreen> BootFlowScreens; // offset 0x0
    BootFlowScreen *CurrentScreen;          // offset 0x8
};

#endif
