#include "LGWheels.hpp"

extern "C" {
int LGStartForceEffect(unsigned long effectId);
int LGStopForceEffect(unsigned long effectId);
int LGDestroyForceEffect(unsigned long effectId);
void OSReport(const char *fmt, ...);
}

static const char kStartForceError[] = "ERROR: Failed to start force effect on channel %d\n";
static const char kStartForceInvalidEffectId[] = "ERROR: Trying to start force effect on channel %d but we have an invalid effectid\n";
static const char kStopForceError[] = "ERROR: Failed to stop force effect on channel %d\n";
static const char kStopForceInvalidEffectId[] = "ERROR: Trying to stop force effect on channel %d but we have an invalid effectid\n";
static const char kDestroyForceError[] = "ERROR: Failed to destroy force effect on channel %d\n";
static const char kDestroyForceInvalidEffectId[] = "ERROR: Trying to destroy force effect on channel %d but we have an invalid effectid\n";

Force::Force() {
    InitVars();
}

void Force::InitVars() {
    int channel;

    for (channel = 0; channel < 4; channel++) {
        int forceNumber;

        for (forceNumber = 0; forceNumber < 8; forceNumber++) {
            Playing[channel][forceNumber] = 0;
            EffectID[channel][forceNumber] = static_cast<unsigned long>(-1);
        }
    }
}

int Force::Start(long channel, long forceNumber) {
    int ret;

    ret = 0;
    if (EffectID[channel][forceNumber] != static_cast<unsigned long>(-1)) {
        ret = LGStartForceEffect(EffectID[channel][forceNumber]);
        if (ret < 0) {
            OSReport(kStartForceError, channel);
        } else {
            Playing[channel][forceNumber] = 1;
        }
    } else {
        OSReport(kStartForceInvalidEffectId, channel);
    }

    return ret;
}

int Force::Stop(long channel, long forceNumber) {
    int ret;

    ret = 0;
    if (EffectID[channel][forceNumber] != static_cast<unsigned long>(-1)) {
        ret = LGStopForceEffect(EffectID[channel][forceNumber]);
        if (ret < 0) {
            OSReport(kStopForceError, channel);
        } else {
            Playing[channel][forceNumber] = 0;
        }
    } else {
        OSReport(kStopForceInvalidEffectId, channel);
    }

    return ret;
}

int Force::Destroy(long channel, long forceNumber) {
    int ret;

    ret = 0;
    if (EffectID[channel][forceNumber] != static_cast<unsigned long>(-1)) {
        ret = LGDestroyForceEffect(EffectID[channel][forceNumber]);
        if (ret < 0) {
            OSReport(kDestroyForceError, channel);
        } else {
            Playing[channel][forceNumber] = 0;
            EffectID[channel][forceNumber] = static_cast<unsigned long>(-1);
        }
    } else {
        OSReport(kDestroyForceInvalidEffectId, channel);
    }

    return ret;
}
