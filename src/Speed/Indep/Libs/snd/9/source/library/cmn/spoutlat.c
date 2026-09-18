#include "./sndcmn.h"

int SNDPLATFORM_outputlatency();

#ifdef __cplusplus
extern "C" {
#endif

int SNDPROFILE_outputlatency() {
    if (sndgs.installed == 0) {
        return 0;
    }

    return SNDPLATFORM_outputlatency();
}

#ifdef __cplusplus
}
#endif
