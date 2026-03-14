#include "Speed/Indep/Src/Misc/MD5.hpp"

void MD5::Reset() {
    uCount = 0;
    uRegs[0] = 0x67452301;
    uRegs[1] = 0xEFCDAB89;
    uRegs[2] = 0x98BADCFE;
    uRegs[3] = 0x10325476;
    computed = false;
}