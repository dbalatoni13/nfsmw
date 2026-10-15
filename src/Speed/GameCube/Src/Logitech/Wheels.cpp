#include "LGWheels.hpp"

#include <string.h>

extern "C" {
int LGOpen(int channel, unsigned long *wheelHandle);
void LGRead(Wheels *wheels);
void OSReport(const char *fmt, ...);
}

static const char kOpenWheelError[] = "ERROR: Could not open wheel on channel %d\n";

Wheels::Wheels() {
    int channel;

    for (channel = 0; channel < 4; channel++) {
        WheelHandles[channel] = static_cast<unsigned long>(-1);
        Position[channel].err = -1;
    }

    memset(PositionLast, 0, sizeof(LGPosition) * 4);
}

short Wheels::ReadAll() {
    int channel;
    short wheelUnplugged;
    int ret;

    for (channel = 0; channel < 4; channel++) {
        if (SIProbe(channel) == 0x08000000 && WheelHandles[channel] == static_cast<unsigned long>(-1)) {
            ret = LGOpen(channel, &WheelHandles[channel]);
            if (ret < 0) {
                OSReport(kOpenWheelError, channel);
            } else {
                Position[channel].err = 0;
            }

            break;
        }
    }

    memcpy(PositionLast, Position, sizeof(LGPosition) * 4);
    LGRead(this);

    for (channel = 0; channel < 4; channel++) {
        if (Position[channel].err == -1 && WheelHandles[channel] != static_cast<unsigned long>(-1)) {
            WheelHandles[channel] = static_cast<unsigned long>(-1);
            wheelUnplugged = static_cast<short>(channel);
            return wheelUnplugged;
        }
    }

    wheelUnplugged = -1;
    return wheelUnplugged;
}

bool Wheels::ButtonIsPressed(long channel, unsigned long buttonMask) {
    const LGPosition *channelPosition = &Position[channel];
    return (channelPosition->button & buttonMask) != 0;
}

bool Wheels::IsConnected(long channel) {
    const LGPosition *channelPosition = &Position[channel];
    return !channelPosition->err;
}

bool Wheels::PedalsConnected(long channel) {
    const LGPosition *channelPosition = &Position[channel];
    return (channelPosition->misc >> 3) & 1;
}
