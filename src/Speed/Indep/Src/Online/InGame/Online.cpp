#include "Client.hpp"
#include "CSCommon.hpp"

extern int OnlineIsServer;

void Online::Init() {
    m_driverNumberQuantizer.Init("m_driverNumberQuantizer", 0, 3);
    if (OnlineIsServer) {
        Server::Init();
        Server::StartServerProcessing();
    } else {
        Client::Init();
        Client::StartClientProcessing();
    }
}

void Online::Close() {
    if (Client::m_state > CLIENTSTATE_INITIAL) {
        Client::Close();
    }
    if (SERVERSTATE_INITIAL < Server::m_state) {
        Server::Close();
    }
}

void Online::ReadIncomingPackets() {
    if (Client::m_state > CLIENTSTATE_INITIAL) {
        Client::ReadIncomingPackets();
    }
    if (Server::m_state > SERVERSTATE_INITIAL) {
        Server::ReadIncomingPackets();
    }
}

void Online::SendUpdates() {
    if (Client::m_state > CLIENTSTATE_INITIAL) {
        Client::SendCarSpam();
    }
}

void Online::ShowDiagnostics() {
    if (IsInitialized()) {
        if (Client::m_state > CLIENTSTATE_INITIAL) {
            Client::ShowDiagnostics();
        }
        if (Server::m_state > SERVERSTATE_INITIAL) {
            Server::ShowDiagnostics();
        }
        if (CLIENTSTATE_INITIAL < Client::m_state || SERVERSTATE_INITIAL < Server::m_state) {
            TheOnlineGame.ShowDiagnostics();
            CSCommon::ShowDiagnostics();
        }
    }
}

void Online::SignalStartClockSync() {
    if (Server::m_state > SERVERSTATE_INITIAL) {
        Server::SignalStartClockSync();
    } else {
        Client::SignalStartClockSync();
    }
}

void Online::SignalReady() {
    if (Server::m_state > SERVERSTATE_INITIAL) {
        Server::SignalReady();
    } else {
        Client::SignalReady();
    }
}

void Online::SignalRestart() {
    if (Server::m_state > SERVERSTATE_INITIAL) {
        Server::SignalRestart();
    } else {
        Client::SignalRestart();
    }
}

void Online::SignalDriverFinish(SmartBitStream &payload_data) {
    if (Server::m_state > SERVERSTATE_INITIAL) {
        Server::SignalDriverFinish(payload_data);
    } else {
        Client::SignalDriverFinish(payload_data);
    }
}

void Online::SignalSyncAnimationMessage(SmartBitStream &payload_data) {
    Server::SignalSyncAnimationMessage(payload_data);
}

void Online::SignalDataCRCMessage(SmartBitStream &payload_data) {
    if (Server::m_state > SERVERSTATE_INITIAL) {
        Server::SignalDataCRCMessage(payload_data);
    } else {
        Client::SignalDataCRCMessage(payload_data);
    }
}

void Online::SplitPacket(MessageTypesEnum type, SmartBitStream &bitstream_data,
                         SplitPacketList &splitPackets) {
    uint8 numChunks = bitstream_data.GetByteLength() / 100 + 1;
    if (bitstream_data.GetByteLength() % 100 + numChunks * 5 > 100) {
        numChunks++;
    }

    for (uint8 i = 1; i <= numChunks; i++) {
        SplitPacketNode *node = new ("Online::SplitPacket", 0) SplitPacketNode;
        node->data.AddByte(static_cast<uint8>(type));
        node->chunk = i;
        node->data.AddByte(i);
        node->totalChunks = numChunks;
        node->data.AddByte(numChunks);
        node->totalBits = i == numChunks ? bitstream_data.GetBitLengthRemaining() : 95 * 8;
        node->data.AddShort(node->totalBits);

        int32 bytesThisPacket = 95;
        if (i == numChunks) {
            bytesThisPacket = bitstream_data.GetByteLengthRemaining();
        }
        if (i == numChunks && bytesThisPacket > 0) {
            bytesThisPacket--;
        }

        char buf[100] = "";
        bitstream_data.GetRawDataWithoutSize(buf, bytesThisPacket);
        node->data.AddRawDataWithoutSize(buf, bytesThisPacket);
        if (i == numChunks) {
            int bitsRemaining = bitstream_data.GetBitLengthRemaining();
            if (bitsRemaining != 0) {
                uint32 temp = 0;
                bitstream_data.GetBits(temp, bitsRemaining);
                node->data.AddBits(temp, bitsRemaining);
            }
        }
        splitPackets.AddTail(node);
    }
}

void Online::JoinPackets(SmartBitStream &joinedPacket, SplitPacketList &splitPackets) {
    joinedPacket.Clear();
    SplitPacketNode *node = splitPackets.GetHead();
    while (node != splitPackets.EndOfList()) {
        uint8 bytesThisPacket = node->totalBits >> 3;
        char buf[100] = "";
        node->data.GetRawDataWithoutSize(buf, bytesThisPacket);
        joinedPacket.AddRawDataWithoutSize(buf, bytesThisPacket);
        if (node->chunk == node->totalChunks) {
            uint8 bitsRemaining =
                static_cast<uint8>(node->totalBits) - bytesThisPacket * 8;
            if (bitsRemaining != 0) {
                uint32 temp = 0;
                node->data.GetBits(temp, bitsRemaining);
                joinedPacket.AddBits(temp, bitsRemaining);
            }
        }
        splitPackets.RemoveHead();
        delete node;
        node = splitPackets.GetHead();
    }
}

bool Online::ReceiveChunk(SmartBitStream &bitstream_data, SplitPacketList &splitPackets) {
    SplitPacketNode *node = new ("Online::ReceiveChunk", 0) SplitPacketNode;
    node->chunk = bitstream_data.GetByte();
    node->totalChunks = bitstream_data.GetByte();
    node->totalBits = bitstream_data.GetShort();

    char buf[100] = "";
    int bitsLeft = bitstream_data.GetByteLengthRemaining();
    if (bitsLeft > 0) {
        bitstream_data.GetRawDataWithoutSize(buf, bitsLeft);
        node->data.AddRawDataWithoutSize(buf, bitsLeft);
    }

    bitsLeft = bitstream_data.GetBitLengthRemaining();
    if (bitsLeft == 8) {
        bitstream_data.GetRawDataWithoutSize(buf, 1);
        node->data.AddRawDataWithoutSize(buf, 1);
    } else if (bitsLeft > 0) {
        uint32 temp = 0;
        bitstream_data.GetBits(temp, bitsLeft);
        node->data.AddBits(temp, bitsLeft);
    }

    splitPackets.AddTail(node);
    return node->chunk == node->totalChunks;
}
