#include "./sndcmn.h"

void Snd::Util::ReallocBuf(void **buffer, int *curSize, int newMinSize, int sizeofDataType) {
    int cursize = *curSize;

    if (cursize < newMinSize) {
        if (cursize == 0) {
            *curSize = newMinSize;
        } else if (cursize < newMinSize) {
            int newsize = cursize;

            do {
                newsize *= 2;
            } while (newsize < newMinSize);

            *curSize = newsize;
        }

        void *pold = *buffer;
        void *pnew = SNDMEMI_allocz(*curSize * sizeofDataType);

        if (pold != NULL) {
            Snd::Util::MemCpy(pnew, pold, cursize * sizeofDataType);
            SNDMEMI_free(pold);
        }

        *buffer = pnew;
    }
}
