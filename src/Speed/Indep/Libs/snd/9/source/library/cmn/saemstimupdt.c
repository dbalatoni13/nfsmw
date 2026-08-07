#include "Speed/Indep/Libs/snd/9/extern/aemsdef.h"
#include "Speed/Indep/Libs/snd/9/source/library/cmn/saemsi.h"
#include <cstddef>

void SNDAEMSI_timerupdate(void *pClientData) {
    AemsDef::TIMERCLIENT *pclient;
    if (sndaems.instreamsynctask) {
        sndaems.timerUpdatesSkipped++;
    } else {
        pclient = reinterpret_cast<AemsDef::TIMERCLIENT *>(sndaems.timerclient.GetHead());
        while (pclient != NULL) {
            CListDNode *pNode = pclient->ln.GetNext();
            pclient->pclientfn(pclient->pclientdata);
            pclient = reinterpret_cast<AemsDef::TIMERCLIENT *>(pNode);
        }
    }
}
