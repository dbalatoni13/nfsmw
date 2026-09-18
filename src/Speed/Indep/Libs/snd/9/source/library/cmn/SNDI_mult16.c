#include "./sndcmn.h"

int SNDI_findprime(int samplerate, int delay);

int SNDI_findmult16(int val, int mult16) {
    return SNDI_findprime(val, mult16 / 16) << 4;
}
