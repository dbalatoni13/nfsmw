#include "LGWheels.hpp"

#include <string.h>

extern "C" {
int LGDownloadForceEffect(unsigned long handle, unsigned long *effectId, LGForceEffect *force);
int LGUpdateForceEffect(unsigned long effectId, LGForceEffect *force);
void OSReport(const char *fmt, ...);
}

static const char kDownloadConditionForceError[] = "ERROR: DownloadForce(condition force) on channel %d returned %d\n";
static const char kDownloadConditionForceInvalidWheel[] = "ERROR: Trying to download a condition force to channel %d but wheel has not been opened.\n";
static const char kUpdateConditionForceError[] = "ERROR: UpdateForce(condition force) on channel %d returned %d\n";

static inline unsigned long &ConditionGetEffectID(Force *self, int channel, int forceNumber) {
    char *base = reinterpret_cast<char *>(self) + 0x80;
    return *reinterpret_cast<unsigned long *>(base + channel * 32 + forceNumber * 4);
}

Condition::Condition() : Force() {}

int Condition::DownloadForce(long channel, long forceNumber, unsigned long & handle, unsigned char type, unsigned long duration, unsigned long startDelay, signed char offset, unsigned char deadband, unsigned char satNeg, unsigned char satPos, short coeffNeg, short coeffPos) {
    LGForceEffect force;
    int ret;

    ret = 0;
    if (ConditionGetEffectID(this, channel, forceNumber) != static_cast<unsigned long>(-1)) {
        Destroy(channel, forceNumber);
    }

    if (handle != static_cast<unsigned long>(-1)) {
        memset(&force, 0, sizeof(force));
        force.type = type;
        force.duration = duration;
        force.startDelay = startDelay;
        force.p.condition[0].offset = offset;
        force.p.condition[0].deadband = deadband;
        force.p.condition[0].saturationNeg = satNeg;
        force.p.condition[0].saturationPos = satPos;
        force.p.condition[0].coefficientNeg = coeffNeg;
        force.p.condition[0].coefficientPos = coeffPos;
        force.p.condition[1] = force.p.condition[0];

        ret = LGDownloadForceEffect(handle, &ConditionGetEffectID(this, channel, forceNumber), &force);
        if (ret < 0) {
            OSReport(kDownloadConditionForceError, channel, ret);
            ConditionGetEffectID(this, channel, forceNumber) = static_cast<unsigned long>(-1);
        }
    } else {
        OSReport(kDownloadConditionForceInvalidWheel, channel);
    }

    return ret;
}

int Condition::UpdateForce(long channel, long forceNumber, unsigned char type, unsigned long duration, unsigned long startDelay, signed char offset, unsigned char deadband, unsigned char satNeg, unsigned char satPos, short coeffNeg, short coeffPos) {
    LGForceEffect force;
    int ret;

    memset(&force, 0, sizeof(force));
    force.type = type;
    force.duration = duration;
    force.startDelay = startDelay;
    force.p.condition[0].offset = offset;
    force.p.condition[0].deadband = deadband;
    force.p.condition[0].saturationNeg = satNeg;
    force.p.condition[0].saturationPos = satPos;
    force.p.condition[0].coefficientNeg = coeffNeg;
    force.p.condition[0].coefficientPos = coeffPos;
    force.p.condition[1] = force.p.condition[0];

    ret = LGUpdateForceEffect(ConditionGetEffectID(this, channel, forceNumber), &force);
    if (ret < 0) {
        OSReport(kUpdateConditionForceError, channel, ret);
        ConditionGetEffectID(this, channel, forceNumber) = static_cast<unsigned long>(-1);
    }

    return ret;
}
