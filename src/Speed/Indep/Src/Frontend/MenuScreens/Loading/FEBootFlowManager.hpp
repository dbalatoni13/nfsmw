#ifndef FRONTEND_MENUSCREENS_LOADING_FEBOOTFLOWMANAGER_H
#define FRONTEND_MENUSCREENS_LOADING_FEBOOTFLOWMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct BootFlowManager {
    static void Init();
    static void Destroy();
    static BootFlowManager *Get();
    virtual ~BootFlowManager();
};

#endif
