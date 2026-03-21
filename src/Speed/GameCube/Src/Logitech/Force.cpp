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

static inline int &ForceGetPlaying(Force *self, int channel, int forceNumber) {
    return reinterpret_cast<int *>(self)[channel * 8 + forceNumber];
}

static inline unsigned long &ForceGetEffectID(Force *self, int channel, int forceNumber) {
    return reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(self) + 0x80)[channel * 8 + forceNumber];
}

Force::Force() {
    InitVars();
}

void Force::InitVars() {
    int channel;

    for (channel = 0; channel < 4; channel++) {
        int forceNumber;

        for (forceNumber = 0; forceNumber < 8; forceNumber++) {
            ForceGetPlaying(this, channel, forceNumber) = 0;
            ForceGetEffectID(this, channel, forceNumber) = static_cast<unsigned long>(-1);
        }
    }
}

int Force::Start(long channel, long forceNumber) {
    int ret;

    ret = 0;
    if (ForceGetEffectID(this, channel, forceNumber) != static_cast<unsigned long>(-1)) {
        ret = LGStartForceEffect(ForceGetEffectID(this, channel, forceNumber));
        if (ret < 0) {
            OSReport(kStartForceError, channel);
        } else {
            ForceGetPlaying(this, channel, forceNumber) = 1;
        }
    } else {
        OSReport(kStartForceInvalidEffectId, channel);
    }

    return ret;
}

int Force::Stop(long channel, long forceNumber) {
    int ret;

    ret = 0;
    if (ForceGetEffectID(this, channel, forceNumber) != static_cast<unsigned long>(-1)) {
        ret = LGStopForceEffect(ForceGetEffectID(this, channel, forceNumber));
        if (ret < 0) {
            OSReport(kStopForceError, channel);
        } else {
            ForceGetPlaying(this, channel, forceNumber) = 0;
        }
    } else {
        OSReport(kStopForceInvalidEffectId, channel);
    }

    return ret;
}

int Force::Destroy(long channel, long forceNumber) {
    int ret;

    ret = 0;
    if (ForceGetEffectID(this, channel, forceNumber) != static_cast<unsigned long>(-1)) {
        ret = LGDestroyForceEffect(ForceGetEffectID(this, channel, forceNumber));
        if (ret < 0) {
            OSReport(kDestroyForceError, channel);
        } else {
            ForceGetPlaying(this, channel, forceNumber) = 0;
            ForceGetEffectID(this, channel, forceNumber) = static_cast<unsigned long>(-1);
        }
    } else {
        OSReport(kDestroyForceInvalidEffectId, channel);
    }

    return ret;
}
