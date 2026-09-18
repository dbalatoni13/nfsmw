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

#ifdef EA_PLATFORM_GAMECUBE
            /* The original AEMS bank-code bridge preserves r31 explicitly,
             * together with the next node and LR, unlike an ordinary C call.
             * pclient is consumed before the call and dead afterwards. */
            asm volatile(
                "lwz 3,%2\n"
                "mflr 22\n"
                "mtlr 3\n"
                "lwz 3,%3\n"
                "mr 20,%0\n"
                "mr 21,31\n"
                "blrl\n"
                "mr 31,21\n"
                "mr %0,20\n"
                "mtlr 22"
                : "+r"(pNode), "+r"(pclient)
                : "m"(pclient->pclientfn), "m"(pclient->pclientdata)
                : "r3", "r4", "r5", "r6", "r7", "r8", "r10", "r11", "r12",
                  "r20", "r21", "r22", "ctr", "cr0", "cr1", "cr5", "cr6", "cr7",
                  "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7",
                  "fr8", "fr9", "fr10", "fr11", "fr12", "fr13", "memory");
#else
            pclient->pclientfn(pclient->pclientdata);
#endif
            pclient = reinterpret_cast<AemsDef::TIMERCLIENT *>(pNode);
        }
    }
}
