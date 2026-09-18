#include "./sndcmn.h"

int SNDSTRMI_create(SNDPLAYOPTS *pspo, int maxrequests, int maxchunks, void *pmem, int memsize,
                    int mastershandle, int istap);

#ifdef __cplusplus
extern "C" {
#endif

int SNDSTRM_createtap(int mastershandle, SNDPLAYOPTS *pspo, int maxrequests, int maxchunks,
                      void *pmem, int memsize) {
    return SNDSTRMI_create(pspo, maxrequests, maxchunks, pmem, memsize, mastershandle, 1);
}

#ifdef __cplusplus
}
#endif
