#include "./sndcmn.h"

int SNDMEM_gethighwater() {
    return (sndgs.mm->heapsize - sndgs.mm->lowmark) * 100 / sndgs.mm->heapsize;
}
