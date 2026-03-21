#include "LGWheels.hpp"

#include <string.h>

extern "C" {
int LGDownloadForceEffect(unsigned long handle, unsigned long *effectId, LGForceEffect *force);
int LGUpdateForceEffect(unsigned long effectId, LGForceEffect *force);
void OSReport(const char *fmt, ...);
}

static const char kDownloadPeriodicForceError[] = "ERROR: DownloadForce(periodic force) on channel %d returned %d\n";
static const char kDownloadPeriodicForceInvalidWheel[] = "ERROR: Trying to download a periodic force to channel %d but wheel has not been opened.\n";
static const char kUpdatePeriodicForceError[] = "ERROR: UpdateForce(periodic force) on channel %d returned %d\n";

static inline unsigned long &PeriodicGetEffectID(Force *self, int channel, int forceNumber) {
    char *base = reinterpret_cast<char *>(self) + 0x80;
    return *reinterpret_cast<unsigned long *>(base + channel * 32 + forceNumber * 4);
}

Periodic::Periodic() : Force() {}

int Periodic::DownloadForce(long channel, long forceNumber, unsigned long & handle, unsigned char type, unsigned long duration, unsigned long startDelay, unsigned char magnitude, unsigned short direction, unsigned short period, unsigned short phase, short offset, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel) {
    LGForceEffect force;
    int ret;

    ret = 0;
    if (PeriodicGetEffectID(this, channel, forceNumber) != static_cast<unsigned long>(-1)) {
        Destroy(channel, forceNumber);
    }

    if (handle != static_cast<unsigned long>(-1)) {
        memset(&force, 0, sizeof(force));
        force.type = type;
        force.duration = duration;
        force.startDelay = startDelay;
        force.p.periodic.magnitude = magnitude;
        force.p.periodic.direction = direction;
        force.p.periodic.period = period;
        force.p.periodic.phase = phase;
        force.p.periodic.offset = offset;
        force.p.periodic.envelope.attackTime = attackTime;
        force.p.periodic.envelope.fadeTime = fadeTime;
        force.p.periodic.envelope.attackLevel = attackLevel;
        force.p.periodic.envelope.fadeLevel = fadeLevel;

        ret = LGDownloadForceEffect(handle, &PeriodicGetEffectID(this, channel, forceNumber), &force);
        if (ret < 0) {
            OSReport(kDownloadPeriodicForceError, channel, ret);
            PeriodicGetEffectID(this, channel, forceNumber) = static_cast<unsigned long>(-1);
        }
    } else {
        OSReport(kDownloadPeriodicForceInvalidWheel, channel);
    }

    return ret;
}

int Periodic::UpdateForce(long channel, long forceNumber, unsigned char type, unsigned long duration, unsigned long startDelay, unsigned char magnitude, unsigned short direction, unsigned short period, unsigned short phase, short offset, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel) {
    LGForceEffect force;
    char *effectIDBase;
    int ret;

    memset(&force, 0, sizeof(force));
    force.type = type;
    force.duration = duration;
    force.startDelay = startDelay;
    force.p.periodic.magnitude = magnitude;
    force.p.periodic.direction = direction;
    force.p.periodic.period = period;
    force.p.periodic.phase = phase;
    force.p.periodic.offset = offset;
    force.p.periodic.envelope.attackTime = attackTime;
    force.p.periodic.envelope.fadeTime = fadeTime;
    force.p.periodic.envelope.attackLevel = attackLevel;
    force.p.periodic.envelope.fadeLevel = fadeLevel;

    effectIDBase = reinterpret_cast<char *>(this) + 0x80;
    ret = LGUpdateForceEffect(*reinterpret_cast<unsigned long *>(effectIDBase + forceNumber * 4 + channel * 32), &force);
    if (ret < 0) {
        OSReport(kUpdatePeriodicForceError, channel, ret);
        *reinterpret_cast<unsigned long *>(effectIDBase + forceNumber * 4 + channel * 32) =
            static_cast<unsigned long>(-1);
    }

    return ret;
}
