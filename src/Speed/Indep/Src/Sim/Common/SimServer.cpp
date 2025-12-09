#include "../SimServer.h"

namespace Sim {

Connection::Connection(const ConnectionData &data)
    : mClient(data.client), //
      mClosed(false) {}

Connection::~Connection() {}

ConnStatus Connection::DoStatusCheck() {
    if (!mClosed) {
        return OnStatusCheck();
    } else {
        return CONNSTATUS_INVALID;
    }
}

void Connection::Close() {
    if (mClosed != true) {
        mClosed = true;
        OnClose();
    }
}

bool Connection::Service(Packet *pkt) {
    if (mClient) {
        return mClient->OnService(reinterpret_cast<HSIMSERVICE>(this), pkt);
    } else {
        return false;
    }
}

HSIMSERVICE OpenService(UCrc32 server, IServiceable *client, Packet *pkt) {
    ConnectionData data;
    data.client = client;
    data.pkt = pkt;
    data.server = server;

    Connection *con = UTL::COM::Factory<ConnectionData const &, Connection, UCrc32>::CreateInstance(pkt->ConnectionClass(), data);

    // big brain
    if (con) {
        return reinterpret_cast<HSIMSERVICE>(con);
    } else {
        return nullptr;
    }
}

void SendService(UCrc32 server, Packet *pkt) {
    UTL::COM::Factory<Packet *, int, UCrc32>::CreateInstance(pkt->ConnectionClass(), pkt);
}

ConnStatus CheckService(HSIMSERVICE hservice) {
    if (hservice) {
        Connection *con = reinterpret_cast<Connection *>(hservice);
        return con->DoStatusCheck();
    } else {
        return CONNSTATUS_INVALID;
    }
}

void CloseService(HSIMSERVICE hservice) {
    if (hservice) {
        Connection *con = reinterpret_cast<Connection *>(hservice);
        con->Close();
    }
}

}; // namespace Sim
