#include "./sndcmn.h"

int SNDSTRMI_queue(int sndstreamhandle, int holdtime, char *pfilename, int offset, int type);

#ifdef __cplusplus
extern "C" {
#endif

int SNDSTRM_queuerequestid(int sndstreamhandle, int holdtime, int id) {
    return SNDSTRMI_queue(sndstreamhandle, holdtime, NULL, id, 2);
}

#ifdef __cplusplus
}
#endif
