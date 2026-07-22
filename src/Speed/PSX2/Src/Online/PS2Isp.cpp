#include "PS2Isp.hpp"

#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <string.h>

extern int bSuperBenderConnected;
extern PS2IspAlertT PS2Isp_DefaultAlertTable[];

extern "C" {
PS2IspT *PS2IspCreate(const PS2IspAlertT *alertTable, unsigned char *elfBuffer,
                      int elfSize, int useMtap, int (*getMtapConnection)(int));
void PS2IspDestroy(PS2IspT *isp);
void PS2IspAutomatic(PS2IspT *isp, bool automaticConnect);
char *PS2IspParams(PS2IspT *isp, PS2IspContextE context);
int PS2IspSubmit(PS2IspT *isp, PS2IspContextE context, char *params);
void PS2IspAbort(PS2IspT *isp, PS2IspContextE context);
PS2IspAlertT *PS2IspAlert(PS2IspT *isp, PS2IspContextE context);
PS2IspStatusE PS2IspGetStatus(PS2IspT *isp, PS2IspContextE context);
void PS2IspSetContext(PS2IspT *isp, PS2IspContextE context);
PS2IspContextE PS2IspGetContext(PS2IspT *isp);
void PS2IspClearAlert(PS2IspT *isp, PS2IspContextE context);
PS2IspAlertE PS2IspGetAlertEnum(PS2IspT *isp, PS2IspAlertT *alert);
int sceSifMInitRpc(int mode);
int sceSifMExitRpc();
int sceMtapGetConnection(int port);
void TagFieldSetString(char *record, int recordLength, const char *name,
                       const char *value);
void TagFieldSetNumber(char *record, int recordLength, const char *name, int value);
void TagFieldSetBinary(char *record, int recordLength, const char *name,
                       const unsigned char *value, int valueLength);
char *TagFieldFind(const char *record, const char *name);
int TagFieldGetNumber(const char *field, int defaultValue);
}

PS2Isp::PS2Isp()
    : ps2isp(nullptr)
    , dirtyDnasElfBuffer(nullptr) /**/
    , dirtyDnasElfSize(0) /**/
    , dirtyDnasAuthBuffer(nullptr) /**/
    , doingRetryAttempt(false) /**/
    , sceRpcInitialized(false) {}

PS2Isp::~PS2Isp() {
    Finish();
}

PS2Isp &PS2Isp::Instance() {
    static PS2Isp the;
    return the;
}

bool PS2Isp::Start(const PS2IspAlertT *alertTable, PS2IspContextE startContext,
                   bool automaticConnect, char *params) {
    bool rc;
    if (!alertTable) {
        alertTable = PS2Isp_DefaultAlertTable;
    }

    if (IsStarted()) {
        PS2IspAbort(ps2isp, GetContext());
        doingRetryAttempt = false;
    } else {
        if (!LoadDirtyDnasFile()) {
            return false;
        }
        if (!LoadDirtyDnasAuthFile()) {
            return false;
        }
        ps2isp = PS2IspCreate(alertTable, dirtyDnasElfBuffer, dirtyDnasElfSize, 1,
                              sceMtapGetConnection);
        if (!ps2isp) {
            return false;
        }
        doingRetryAttempt = false;
    }

    rc = true;
    PS2IspAutomatic(ps2isp, automaticConnect);
    if (automaticConnect) {
        rc = DoContext(startContext, params);
    } else {
        PS2IspSetContext(ps2isp, startContext);
    }

    if (!sceRpcInitialized) {
        sceSifMInitRpc(0);
        sceRpcInitialized = true;
    }
    return rc;
}

void PS2Isp::Finish() {
    if (IsStarted()) {
        PS2IspAbort(ps2isp, GetContext());
        PS2IspDestroy(ps2isp);
        ps2isp = nullptr;
        if (dirtyDnasElfBuffer) {
            bFree(dirtyDnasElfBuffer);
            bFree(dirtyDnasAuthBuffer);
            dirtyDnasAuthBuffer = nullptr;
            dirtyDnasElfSize = 0;
            dirtyDnasElfBuffer = nullptr;
        }
        if (sceRpcInitialized) {
            sceSifMExitRpc();
            sceRpcInitialized = false;
        }
    }
}

bool PS2Isp::IsStarted() {
    return ps2isp != nullptr;
}

bool PS2Isp::IsFinished() {
    if (!IsStarted()) {
        return true;
    }
    return PS2IspGetContext(ps2isp) == PS2ISP_CONTEXT_DONE;
}

bool PS2Isp::DoContextMemcard() {
    return DoContext(PS2ISP_CONTEXT_MEMCARD, nullptr);
}

bool PS2Isp::DoContextLoadCfg(bool useSharedLogin) {
    char params[64] = "";
    TagFieldSetString(params, sizeof(params), "CFGPTH", "mc:0");
    if (!useSharedLogin) {
        TagFieldSetString(params, sizeof(params), "SLOGIN", "FALSE");
    }
    return DoContext(PS2ISP_CONTEXT_LOADCFG, params);
}

bool PS2Isp::DoContextSelect(int index) {
    char params[48] = "";
    if (bSuperBenderConnected) {
        TagFieldSetNumber(params, sizeof(params), "USEEXISTING", 1);
    }
    TagFieldSetNumber(params, sizeof(params), "NETCFG", index);
    return DoContext(PS2ISP_CONTEXT_SELECT, params);
}

bool PS2Isp::DoContextDNAS(bool skipDNAS) {
    char params[256] = "";
    if (skipDNAS) {
        TagFieldSetNumber(params, sizeof(params), "DNASSKIP", 1);
    } else {
        unsigned char *pass = BuildRegion::GetDnasPassPhrase();
        TagFieldSetNumber(params, sizeof(params), "ATHPTR",
                          reinterpret_cast<int>(dirtyDnasAuthBuffer));
        TagFieldSetNumber(params, sizeof(params), "ATHLEN", 0x10000);
        TagFieldSetBinary(params, sizeof(params), "PASSPH", pass, 8);
        TagFieldSetNumber(params, sizeof(params), "PASSSZ", 8);
        TagFieldSetNumber(params, sizeof(params), "REGION", 1);
        TagFieldSetNumber(params, sizeof(params), "LANGUAGE", 2);
        TagFieldSetNumber(params, sizeof(params), "TIMEOUT", 30);
    }
    return DoContext(PS2ISP_CONTEXT_DNAS, params);
}

PS2IspContextE PS2Isp::GetContext() {
    return PS2IspGetContext(ps2isp);
}

PS2IspStatusE PS2Isp::GetStatus() {
    return PS2IspGetStatus(ps2isp, GetContext());
}

int PS2Isp::GetNumNetworkConfigs() {
    GetContext();
    int numConfigs = TagFieldGetNumber(
        TagFieldFind(PS2IspParams(ps2isp, PS2ISP_CONTEXT_SELECT), "NUMCFG"), 0);
    return numConfigs > 4 ? 4 : numConfigs;
}

NetConfigRecT *PS2Isp::GetNetworkConfigs() {
    return reinterpret_cast<NetConfigRecT *>(TagFieldGetNumber(
        TagFieldFind(PS2IspParams(ps2isp, PS2ISP_CONTEXT_SELECT), "CFGPTR"), 0));
}

bool PS2Isp::IsAlertSet() {
    return GetStatus() == PS2ISP_STATUS_ALERT;
}

PS2IspAlertT *PS2Isp::GetAlert(PS2IspContextE context) {
    PS2IspContextE alertContext;
    if (context == PS2ISP_NUMCONTEXTS) {
        alertContext = GetContext();
    } else {
        alertContext = context;
    }
    return PS2IspAlert(ps2isp, alertContext);
}

PS2IspAlertE PS2Isp::GetAlertEnum(PS2IspContextE context) {
    return PS2IspGetAlertEnum(ps2isp, GetAlert(context));
}

void PS2Isp::ClearAlert(PS2IspContextE context) {
    PS2IspContextE alertContext;
    if (context == PS2ISP_NUMCONTEXTS) {
        alertContext = GetContext();
    } else {
        alertContext = context;
    }
    PS2IspClearAlert(ps2isp, alertContext);
}

bool PS2Isp::LoadDirtyDnasFile() {
    if (!dirtyDnasElfBuffer) {
        dirtyDnasElfBuffer = static_cast<unsigned char *>(
            bGetFile("ONLINE\\dirtydnas_eu.elf", &dirtyDnasElfSize, 0x1040));
        if (!dirtyDnasElfBuffer) {
            dirtyDnasElfSize = 0;
            return false;
        }
    }
    return true;
}

bool PS2Isp::LoadDirtyDnasAuthFile() {
    bool rc = true;
    char authFile[64];
    int fileSize;
    if (!dirtyDnasAuthBuffer) {
        fileSize = 0;
        bSPrintf(authFile, "ONLINE\\%s.dat", BuildRegion::GetSlusCode());
        dirtyDnasAuthBuffer = static_cast<unsigned char *>(
            bGetFile(authFile, &fileSize, 0x1040));
        rc = dirtyDnasAuthBuffer != nullptr;
    }
    return rc;
}

bool PS2Isp::DoContext(PS2IspContextE context, char *params) {
    char emptyParams[1];
    if (context != GetContext()) {
        PS2IspSetContext(ps2isp, context);
    }
    char *contextParams = emptyParams;
    if (params) {
        contextParams = params;
    }
    memcpy(emptyParams, "", sizeof(emptyParams));
    return PS2IspSubmit(ps2isp, context, contextParams) == 0;
}
