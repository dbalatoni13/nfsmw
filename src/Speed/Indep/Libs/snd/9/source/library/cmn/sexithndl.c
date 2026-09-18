#include "./sndcmn.h"

void SNDREAL_exithandler() {
    Snd::System::Restore();
}
