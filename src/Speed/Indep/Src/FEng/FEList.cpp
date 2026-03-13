#include "Speed/Indep/Src/FEng/FEList.h"

char FEUpperCase(char val) {
    if (static_cast<unsigned int>(val - 'a') > 25) {
        return val;
    }

    return static_cast<char>(val - 0x20);
}

unsigned long FEHash(const char* String) {
    unsigned long hash = 0xFFFFFFFF;

    if (String) {
        unsigned char c = *reinterpret_cast<const unsigned char*>(String);

        while (c != 0) {
            hash = c + hash * 33;
            String++;
            c = *reinterpret_cast<const unsigned char*>(String);
        }
    }

    return hash;
}

unsigned long FEHashUpper(const char* String) {
    unsigned long hash = 0xFFFFFFFF;

    if (String) {
        char c = *String;

        while (c != '\0') {
            unsigned long uc = FEUpperCase(c);

            String++;
            hash = hash * 33 + (uc & 0xFF);
            c = *String;
        }
    }

    return hash;
}
