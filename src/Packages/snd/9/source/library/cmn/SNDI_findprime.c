#include "snd/source/library/cmn/sndcmn.h"

int SNDI_cheapsqrt(int val) {
    int i = 0;
    int done;
    int temp;
    done = i != 0;

    while (!done) {
        temp = i * i;
        i++;

        if (temp == val)
            return i - 1;
        if (temp > val)
            return i - 2;
    }
    return 0;
}

int SNDI_findprime(int samplerate, int delay) {
    int i;
    int prime;
    int upper;
    int done;

    delay *= samplerate;
    prime = delay / 1000;

    if (prime <= 2) {
        prime = 3;
    }

    done = 0;

    while (!done) {
        upper = SNDI_cheapsqrt(prime) + 1;
        i = 2;

        while (i <= upper) {
            if (prime % i == 0) {
                prime++;
                break;
            }

            if (i == upper) {
                return prime;
            }

            i++;
        }
    }

    return 0;
}
