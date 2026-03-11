extern void SNDAEMS_removemodulebank(int handle);
extern void SubscribeEventSys();

void EAXAemsManager::ResolvePendingAsyncLoads() {}

void *EAXAemsManager::GetCallbackEventSys() {
    return m_pEvtSystems_start[m_nCallbackEvtSys];
}

void EAXAemsManager::EvtSysLoadCallback(int param, int error_status) {
    gAEMSMgr.m_nCallbackEvtSys = param;
    SubscribeEventSys();
}

void EAXAemsManager::RemoveAEMSBank() {
    int mbhandle = m_pCurUNLOADSDLP->Handle;
    if (mbhandle != -1) {
        SNDAEMS_removemodulebank(mbhandle);
    }
}