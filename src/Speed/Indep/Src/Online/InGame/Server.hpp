#ifndef ONLINE_INGAME_SERVER_HPP
#define ONLINE_INGAME_SERVER_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

class SmartBitStream;
class Timer;
struct ConnApiRefT;
struct ConnApiCbInfoT;
struct Online;
struct OnlineCfg;

enum ServerStateEnum {
    SERVERSTATE_INITIAL = 0,
    SERVERSTATE_WELCOME = 1,
    SERVERSTATE_WAITJOINS = 2,
    SERVERSTATE_LOADING = 3,
    SERVERSTATE_NIS = 4,
    SERVERSTATE_WAITLOADS = 5,
    SERVERSTATE_READY = 6,
    SERVERSTATE_RACING = 7
};

struct Server {
  public:
    static void Close();
    static void StartServerProcessing();

  private:
    friend struct Online;
    friend struct OnlineCfg;

    static void Init();
    static void SetState(ServerStateEnum new_state);
    static int FindRandomAvailableDriverNumber();
    static void ConnectionCoreCB(ConnApiRefT *connapi, ConnApiCbInfoT *cbinfo, void *context);
    static void ReadIncomingPackets();
    static void Think();
    static void HandleIncomingPacket(int client_id, char *data, int num_bytes, bool reliable);
    static void SendQuitMessage();
    static void SendWelcomeMessage(int client_id);
    static void SendCarDescriptionMessage(int client_id, int driver_number);
    static void ShowDiagnostics();
    static void SignalStartClockSync();
    static void SignalReady();
    static void SignalRestart();
    static void SignalDriverFinish(SmartBitStream &payload_data);
    static void SignalScoreMessage(SmartBitStream &payload_data);
    static void SignalSyncAnimationMessage(SmartBitStream &payload_data);
    static void SignalDataCRCMessage(SmartBitStream &payload_data);

    static ServerStateEnum m_state;
    static int m_driverNumber;
    static bool m_bForceClientShutdown;
    static Timer m_readyStateStartTime;
    static uint32 m_syncAnimationSendId;
    static int32 m_ticker;
};

#endif
