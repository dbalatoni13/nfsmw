#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"

#include <types.h>

bList bFunkServerList;

void bFunkCallASync(const char *server_name, int function_num, const void *data, int size) {
    bFunkPacketHeader header;
}

int bFunkCallSync(const char *server_name, int function_num, const void *data, int size, void *return_data, int max_return_size) {
    volatile int return_code;
    bFunkPacketHeader header;

    return -5;
}

int bFunkDoesServerExist(const char *server_name) {
    uint32 server_hash = bStringHash(server_name);
    int result;
    int return_code = bFunkCallSync("CODEINE", 0x16, server_name, bStrLen(server_name) + 1, &result, 4);

    if (return_code == 4) {
        return result;
    }

    return 0;
}

void bRefreshTweaker() {
    bFunkCallASync("CODEINE", 0x28, nullptr, 0);
}
