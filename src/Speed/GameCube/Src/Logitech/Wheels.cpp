#include "LGWheels.hpp"

bool Wheels::ButtonIsPressed(int channel, unsigned long buttonMask) {
    const LGPosition *position = reinterpret_cast<const LGPosition *>(this);
    return (position[channel].button & buttonMask) != 0;
}

bool Wheels::IsConnected(int channel) {
    const LGPosition *position = reinterpret_cast<const LGPosition *>(this);
    return position[channel].err != 0;
}

bool Wheels::PedalsConnected(int channel) {
    const LGPosition *position = reinterpret_cast<const LGPosition *>(this);
    return (position[channel].misc >> 3) & 1;
}
