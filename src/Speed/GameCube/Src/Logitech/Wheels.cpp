#include "Speed/GameCube/Src/Logitech/Wheels.h"
#include "Speed/GameCube/Src/Logitech/LGDev.h"

#include <string.h>

Wheels::Wheels() {
    for (int channel = 0; channel < 4; channel++) {
        this->WheelHandles[channel] = -1;
        this->Position[channel].err = -1;
    }

    memset(this->PositionLast, 0, sizeof(this->PositionLast));
}

short Wheels::ReadAll() {
    long channel;

    for (channel = 0; channel < 4; channel++) {
        if (SIProbe(channel) == 0x8000000 && this->WheelHandles[channel] == -1) {
            if (LGOpen(channel, &this->WheelHandles[channel]) < 0) {
                OSReport("ERROR: Could not open wheel on channel %d\n", channel);
            } else {
                this->Position[channel].err = 0;
            }
            break;
        }
    }

    memcpy(this->PositionLast, this->Position, 0x28);
    LGRead(this);

    for (channel = 0; channel < 4; channel++) {
        if (this->Position[channel].err == -1 && this->WheelHandles[channel] != -1) {
            this->WheelHandles[channel] = -1;
            return channel;
        }
    }

    return -1;
}

bool Wheels::ButtonIsPressed(long channel, unsigned long buttonMask) {
    return (this->Position[channel].button & buttonMask) != 0;
}

bool Wheels::IsConnected(long channel) {
    return this->Position[channel].err == 0;
}

bool Wheels::PedalsConnected(long channel) {
    return (this->Position[channel].misc >> 3) & 1;
}
