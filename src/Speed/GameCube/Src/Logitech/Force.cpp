#include "Speed/GameCube/Src/Logitech/Force.h"
#include "Speed/GameCube/Src/Logitech/LGDev.h"

#include "dolphin.h"

Force::Force() {
    this->InitVars();
}

void Force::InitVars() {
    for (int channel = 0; channel < 4; channel++) {
        for (int effect = 0; effect < 8; effect++) {
            this->Playing[channel][effect] = 0;
            this->EffectID[channel][effect] = -1;
        }
    }
}

long Force::Start(long channel, long effect) {
    long ret = 0;

    if (this->EffectID[channel][effect] != -1) {
        ret = LGStartForceEffect(this->EffectID[channel][effect]);
        if (ret < 0) {
            OSReport("ERROR: Failed to start force effect on channel %d\n", channel);
        } else {
            this->Playing[channel][effect] = 1;
        }
    } else {
        OSReport("ERROR: Trying to start force effect on channel %d but we have an invalid effectid\n", channel);
    }

    return ret;
}

long Force::Stop(long channel, long effect) {
    long ret = 0;

    if (this->EffectID[channel][effect] != -1) {
        ret = LGStopForceEffect(this->EffectID[channel][effect]);
        if (ret < 0) {
            OSReport("ERROR: Failed to stop force effect on channel %d\n", channel);
        } else {
            this->Playing[channel][effect] = 0;
        }
    } else {
        OSReport("ERROR: Trying to stop force effect on channel %d but we have an invalid effectid\n", channel);
    }

    return ret;
}

long Force::Destroy(long channel, long effect) {
    long ret = 0;

    if (this->EffectID[channel][effect] != -1) {
        ret = LGDestroyForceEffect(this->EffectID[channel][effect]);
        if (ret < 0) {
            OSReport("ERROR: Failed to destroy force effect on channel %d\n", channel);
        } else {
            this->Playing[channel][effect] = 0;
            this->EffectID[channel][effect] = -1;
        }
    } else {
        OSReport("ERROR: Trying to destroy force effect on channel %d but we have an invalid effectid\n", channel);
    }

    return ret;
}
