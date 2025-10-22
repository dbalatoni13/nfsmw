#ifndef SIM_SIMSERVER_H
#define SIM_SIMSERVER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "./SimTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Interfaces/IServiceable.h"

namespace Sim {

struct ConnectionData {
    IServiceable *client; // offset 0x0, size 0x4
    Packet *pkt; // offset 0x4, size 0x4
    UCrc32 server; // offset 0x8, size 0x4
};

class Connection : public UTL::COM::Factory<const ConnectionData &, Connection, UCrc32> {
    IServiceable *mClient;
    bool mClosed;

public:
    Connection(const ConnectionData &);
    void Close();
    bool Service(Packet *pkt);
};

} // namespace Sim

#endif
