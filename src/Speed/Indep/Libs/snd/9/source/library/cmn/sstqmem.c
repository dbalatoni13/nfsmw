#include "./sndcmn.h"

int SNDSTRMI_queue(int sndstreamhandle, int holdtime, char *pfilename, int offset, int type);

#ifdef __cplusplus
extern "C" {
#endif

int SNDSTRM_queuemem(int sndstreamhandle, int holdtime, void *paddr, int offset) {
    return SNDSTRMI_queue(sndstreamhandle, holdtime, (char *)paddr + offset, 0, 1);
}

#ifdef __cplusplus
}
#endif
