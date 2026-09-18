#ifndef PSX2_ONLINE_PS2_ISP_HPP
#define PSX2_ONLINE_PS2_ISP_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

struct PS2IspT;

enum PS2IspAlertE {
    PS2ISP_ALERT_NOANSWER = 0,
    PS2ISP_ALERT_USERAUTH = 1,
    PS2ISP_ALERT_BUSY = 2,
    PS2ISP_ALERT_CARRIER = 3,
    PS2ISP_ALERT_CONNECT = 4,
    PS2ISP_ALERT_LOCATE = 5,
    PS2ISP_ALERT_DNSFAIL = 6,
    PS2ISP_ALERT_HWINIT = 7,
    PS2ISP_ALERT_DIALTONE = 8,
    PS2ISP_ALERT_ISPTIME = 9,
    PS2ISP_ALERT_HWINVALID = 10,
    PS2ISP_ALERT_FIRST_MEMC = 11,
    PS2ISP_ALERT_DIFFCFG = 11,
    PS2ISP_ALERT_MCREMOVED = 12,
    PS2ISP_ALERT_NETCFG = 13,
    PS2ISP_ALERT_NOCONFIG = 14,
    PS2ISP_ALERT_SOMECONFIG = 15,
    PS2ISP_ALERT_NOMEMCARD = 16,
    PS2ISP_ALERT_NOFORMAT = 17,
    PS2ISP_ALERT_NOSLOGIN = 18,
    PS2ISP_ALERT_BADSLOGIN = 19,
    PS2ISP_ALERT_LAST_MEMC = 19,
    PS2ISP_ALERT_DIFFCFG_MTAP = 20,
    PS2ISP_ALERT_MCREMOVED_MTAP = 21,
    PS2ISP_ALERT_NETCFG_MTAP = 22,
    PS2ISP_ALERT_NOCONFIG_MTAP = 23,
    PS2ISP_ALERT_SOMECONFIG_MTAP = 24,
    PS2ISP_ALERT_NOMEMCARD_MTAP = 25,
    PS2ISP_ALERT_NOFORMAT_MTAP = 26,
    PS2ISP_ALERT_NOSLOGIN_MTAP = 27,
    PS2ISP_ALERT_BADSLOGIN_MTAP = 28,
    PS2ISP_ALERT_ISPFAIL = 29,
    PS2ISP_NUMALERTS = 30
};

enum PS2IspContextE {
    PS2ISP_CONTEXT_OFFLINE = 0,
    PS2ISP_CONTEXT_MEMCARD = 1,
    PS2ISP_CONTEXT_LOADCFG = 2,
    PS2ISP_CONTEXT_SELECT = 3,
    PS2ISP_CONTEXT_DNAS = 4,
    PS2ISP_CONTEXT_DONE = 5,
    PS2ISP_NUMCONTEXTS = 6
};

enum PS2IspStatusE {
    PS2ISP_STATUS_IDLE = 0,
    PS2ISP_STATUS_NETWORK = 1,
    PS2ISP_STATUS_MEMCARD = 2,
    PS2ISP_STATUS_ALERT = 3,
    PS2ISP_STATUS_GOTO = 4,
    PS2ISP_NUMSTATUSSTATES = 5
};

struct PS2IspAlertT {
    int uFlags;
    char *pTitle;
    char *pBody;
    char *pFooter;
};

struct NetConfigRecT {
    int iIdent;
    int iType;
    char strDev[62];
    char strIsp[62];
    char strLoad[123];
    unsigned char uFlags;
};

class PS2Isp {
  private:
    PS2IspT *ps2isp;
    unsigned char *dirtyDnasElfBuffer;
    int dirtyDnasElfSize;
    unsigned char *dirtyDnasAuthBuffer;
    bool doingRetryAttempt;
    bool sceRpcInitialized;

  public:
    PS2Isp();
    ~PS2Isp();
    static PS2Isp &Instance();
    bool Start(const PS2IspAlertT *alertTable, PS2IspContextE startContext,
               bool automaticConnect, char *params);
    void Finish();
    bool IsStarted();
    bool IsFinished();
    bool DoContextMemcard();
    bool DoContextLoadCfg(bool useSharedLogin);
    bool DoContextSelect(int index);
    bool DoContextDNAS(bool skipDNAS);
    PS2IspContextE GetContext();
    PS2IspStatusE GetStatus();
    bool IsAlertSet();
    PS2IspAlertT *GetAlert(PS2IspContextE context);
    PS2IspAlertE GetAlertEnum(PS2IspContextE context);
    void ClearAlert(PS2IspContextE context);
    int GetNumNetworkConfigs();
    NetConfigRecT *GetNetworkConfigs();
    bool IsRetryAttempt() { return doingRetryAttempt; }

  private:
    bool LoadDirtyDnasFile();
    bool LoadDirtyDnasAuthFile();
    bool DoContext(PS2IspContextE context, char *params);
};

#endif
