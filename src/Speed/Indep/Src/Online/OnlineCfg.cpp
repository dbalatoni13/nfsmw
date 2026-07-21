#include "OnlineCfg.hpp"
#include "ConditionSimulator.hpp"
#include "InGame/CSCommon.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern int SkipMovies;
extern const char *SkipFEPlayerCar;
extern int OnlineEnabled;
extern int UseRecordable;
extern float Tweak_GameSpeed;
extern int DoScreenPrintf;
extern bool bToggleAiControl;
extern bool g_bNewCarSmoothing;
extern int OnlineIsServer;
extern int SkipFEOnlineNumExpectedRemotePlayers;
extern uint32 SkipFEOnlineHostAddr;
extern uint16 NetworkHostPort;
extern uint32 SkipFEOnlineClientAddr[3];
extern int NetworkDebug;
extern int NetworkUseLobbies;
extern char NetworkGameName[36];
extern int OnlineAutomaticLogin;

static char onlineCfgFilename[64] = "online.cfg";
static char onlinePlayerCar[64];

struct Server {
    static bool m_bForceClientShutdown;
};

extern "C" uint32 SocketInTextGetAddr(const char *text);

void OnlineCfg::ReadConfigFile(const char *filename) {
    if (!filename) {
        filename = onlineCfgFilename;
    }

    if (bFileExists(filename)) {
        bFile *f;
        register bool finished;
        uint32 i;

        finished = false;
        i = 0;
        f = bOpen(filename, 1, 1);
        char buffer[2000] = "";
        bMemSet(buffer, '\0', sizeof(buffer));
        bRead(f, buffer, bFileSize(f));
        bClose(f);

        char attribute[300] = "";
        char value[300] = "";
        char *start;
        signed char *current;
        register int space = ' ';

        current = reinterpret_cast<signed char *>(buffer);
        start = reinterpret_cast<char *>(current);
        do {
            while (*current != '=') {
                i++;
                if (i >= sizeof(buffer) + 1) {
                    finished = true;
                    break;
                }
                current = reinterpret_cast<signed char *>(&buffer[i]);
            }

            if (!finished) {
                bMemSet(attribute, '\0', sizeof(attribute));
                bStrNCpy(attribute, start, reinterpret_cast<char *>(current) - start);
                i++;
                current = reinterpret_cast<signed char *>(&buffer[i]);
                start = reinterpret_cast<char *>(current);
                if (*current != '\r' && *current != '\0') {
                    do {
                        i++;
                        current = reinterpret_cast<signed char *>(&buffer[i]);
                    } while (*current != '\r' && *current != '\0');
                }
                bMemSet(value, '\0', sizeof(value));
                bStrNCpy(value, start, reinterpret_cast<char *>(current) - start);
                ProcessSetting(attribute, value);

                do {
                    i++;
                } while (buffer[i] == space || buffer[i] == '\n' || buffer[i] == '\r');
                current = reinterpret_cast<signed char *>(&buffer[i]);
                if (i < sizeof(buffer) + 1) {
                    if (*current == '\0') {
                        finished = true;
                    }
                } else {
                    finished = true;
                }
                start = reinterpret_cast<char *>(current);
            }
        } while (!finished);
    }
}

void OnlineCfg::ProcessSetting(const char *attribute, const char *value) {
    if (!bStrICmp(attribute, "SKIPFE"))
        SkipFE = bStrToLong(value);
    if (!bStrICmp(attribute, "SKIPFEPLAYERCAR")) {
        bMemSet(onlinePlayerCar, '\0', sizeof(onlinePlayerCar));
        bStrNCpy(onlinePlayerCar, value, sizeof(onlinePlayerCar) - 1);
        SkipFEPlayerCar = onlinePlayerCar;
    }
    if (!bStrICmp(attribute, "SKIPFENUMAICARS"))
        SkipFENumAICars = bStrToLong(value);
    if (!bStrICmp(attribute, "SKIPFENUMLAPS"))
        SkipFENumLaps = bStrToLong(value);
    if (!bStrICmp(attribute, "SKIPFETRACKNUMBER"))
        SkipFETrackNumber = bStrToLong(value);
    if (!bStrICmp(attribute, "SKIPMOVIES"))
        SkipMovies = bStrToLong(value);
    if (!bStrICmp(attribute, "FORCECLIENTSHUTDOWN"))
        Server::m_bForceClientShutdown = bStrToLong(value) != 0;
    if (!bStrICmp(attribute, "ONLINEENABLED"))
        OnlineEnabled = bStrToLong(value);
    if (!bStrICmp(attribute, "USERECORDABLE"))
        UseRecordable = bStrToLong(value);
    if (!bStrICmp(attribute, "GAMESPEED"))
        Tweak_GameSpeed = bStrToLong(value) * 0.01f;
    if (!bStrICmp(attribute, "DOSCREENPRINTF"))
        DoScreenPrintf = bStrToLong(value);
    if (!bStrICmp(attribute, "ONLINEDEBUGDRAW"))
        bStrToLong(value);
    if (!bStrICmp(attribute, "AICONTROL") && bStrToLong(value))
        bToggleAiControl = true;
    if (!bStrICmp(attribute, "NEWCARSMOOTHING"))
        g_bNewCarSmoothing = bStrToLong(value) != 0;
    if (!bStrICmp(attribute, "ISSERVER"))
        OnlineIsServer = bStrToLong(value);
    if (!bStrICmp(attribute, "NUMCLIENTCARS"))
        SkipFEOnlineNumExpectedRemotePlayers = bStrToLong(value);
    if (!bStrICmp(attribute, "SERVERIP"))
        SkipFEOnlineHostAddr = SocketInTextGetAddr(value);
    if (!bStrICmp(attribute, "SERVERPORT"))
        NetworkHostPort = static_cast<uint16>(bStrToLong(value));

    for (int i = 0; i < 3; i++) {
        char key[64];
        bSPrintf(key, "CLIENTIP%d", i);
        if (!bStrICmp(attribute, key))
            SkipFEOnlineClientAddr[i] = SocketInTextGetAddr(value);
    }

    if (!bStrICmp(attribute, "FORCECLIENTSHUTDOWN"))
        Server::m_bForceClientShutdown = bStrToLong(value) != 0;
    if (!bStrICmp(attribute, "SIMCONDITION"))
        ConditionSimulator::SetSimCondition(bStrToLong(value));
    if (!bStrICmp(attribute, "SOUND")) {
        int sound = bStrToLong(value);
        IsAudioStreamingEnabled = sound;
        IsSoundEnabled &= sound;
    }
    if (!bStrICmp(attribute, "DIAGNOSTICLEVEL"))
        CSCommon::SetDiagnosticLevel(
            static_cast<DiagnosticLevelEnum>(bStrToLong(value) & 0xffff));
    if (!bStrICmp(attribute, "NetworkDebug"))
        NetworkDebug = bStrToLong(value);
    if (!bStrICmp(attribute, "NetworkUserName"))
        FEDatabase->OnlineSettings.SetLobbyAccountName(value);
    if (!bStrICmp(attribute, "NetworkUseLobbies"))
        NetworkUseLobbies = bStrToLong(value);
    if (!bStrICmp(attribute, "NetworkLobbyIP") || !bStrICmp(attribute, "LobbyServerAddr"))
        bStrCpy(cOnlineSettings::LobbyServerAddr, value);
    if (!bStrICmp(attribute, "NetworkLobbyPort") || !bStrICmp(attribute, "LobbyServerPort"))
        cOnlineSettings::LobbyServerPort = static_cast<uint16>(bStrToLong(value));
    if (!bStrICmp(attribute, "NetworkPasswd")) {
        char key[32] = "";
        bStrCpy(key, value);
        FEDatabase->OnlineSettings.SetLobbyPassword(key);
    }
    if (!bStrICmp(attribute, "NetworkPersona"))
        FEDatabase->OnlineSettings.SetLobbyPersona(value);
    if (!bStrICmp(attribute, "NetworkGameName"))
        bStrCpy(NetworkGameName, value);
    if (!bStrICmp(attribute, "AutomaticLogin"))
        OnlineAutomaticLogin = bStrToLong(value);
}

void *OnlineMemoryPool = nullptr;

void AllocOnlineMemoryPool() {
    TheTrackStreamer.MakeSpaceInPool(0x14000, true);
    OnlineMemoryPool = TheTrackStreamer.AllocateUserMemory(0x14000, "Online Buffer", 0);
    bInitMemoryPool(8, OnlineMemoryPool, 0x14000, "Online");
}

void FreeOnlineMemoryPool() {
    if (OnlineMemoryPool) {
        bCloseMemoryPool(8);
        bFree(OnlineMemoryPool);
        OnlineMemoryPool = nullptr;
    }
}

extern "C" {

void *LobbyAlloc(int iSize) {
    static int count;
    return bMalloc(iSize, "LobbyAlloc", count++, 0x40);
}

void LobbyFree(void *pMem) { bFree(pMem); }

void *NetAlloc(int iSize) {
    static int count;
    if (!OnlineMemoryPool) {
        return bMalloc(iSize, "NetAlloc (for Juice)", 0, 0);
    }
    if (iSize > 0x3e800) {
        return bMalloc(iSize, "NetAlloc", count++, 0);
    }
    return bMalloc(iSize, "NetAlloc", count++, 8);
}

void NetFree(void *pMem) { bFree(pMem); }

void *RpcAlloc(int iSize) {
    if (!OnlineMemoryPool) {
        return bMalloc(iSize, "RPC Alloc (for Juice)", 0, 0);
    }
    return bMalloc(iSize, "d:/p4_apex1666_d1001856/mw/speed/indep/src/online/OnlineMemory.cpp", 0x74, 0x48);
}

void RpcFree(void *pMem) { bFree(pMem); }

}
