#ifndef SIM_SIMSERVER_H
#define SIM_SIMSERVER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "./SimConn.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Interfaces/IServiceable.h"

namespace Sim {

enum ConnStatus {
    CONNSTATUS_INVALID = -1,
    CONNSTATUS_CONNECTING = 0,
    CONNSTATUS_READY = 1,
    CONNSTATUS_OTHER = 4,
};

struct ConnectionData {
    IServiceable *client; // offset 0x0, size 0x4
    Packet *pkt;          // offset 0x4, size 0x4
    UCrc32 server;        // offset 0x8, size 0x4
};

class Connection : public UTL::COM::Factory<const ConnectionData &, Connection, UCrc32>, public UTL::Collections::Countable<Sim::Connection> {
  public:
    Connection(const ConnectionData &);
    ConnStatus DoStatusCheck();
    void Close();
    bool Service(Packet *pkt);

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

  protected:
    Connection();

    // Virtual functions
    virtual ~Connection();

    virtual void OnClose() = 0;
    virtual ConnStatus OnStatusCheck() = 0;

  private:
    IServiceable *mClient;
    bool mClosed;
};

HSIMSERVICE OpenService(UCrc32 server, IServiceable *client, Packet *pkt);
void SendService(UCrc32 server, Packet *pkt);
ConnStatus CheckService(HSIMSERVICE hservice);
void CloseService(HSIMSERVICE hservice);

} // namespace Sim

#endif
