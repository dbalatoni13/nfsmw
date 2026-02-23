#ifndef EAGL4ANIM_EVENT_H
#define EAGL4ANIM_EVENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace EAGL4Anim {

// total size: 0x10
struct Event {
    int eventId;       // offset 0x0, size 0x4
    float triggerTime; // offset 0x4, size 0x4
    float duration;    // offset 0x8, size 0x4
    float parameter;   // offset 0xC, size 0x4
};

}; // namespace EAGL4Anim

#endif
