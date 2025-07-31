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
    int server_hash;
    int result;
    bStringHash(server_name);
    server_hash = bFunkCallSync("CODEINE", 0x16, server_name, bStrLen(server_name) + 1, &result, 4);
    int return_code = 0;
    if (server_hash == 4) {
        return_code = result;
    }
    return return_code;
}

void bRefreshTweaker() {
    bFunkCallASync("CODEINE", 0x28, nullptr, 0);
}
