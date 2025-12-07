#ifndef ANIMATION_ANIMCHOOSER_H
#define ANIMATION_ANIMCHOOSER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x4
struct CAnimChooser {
    enum eType {
        MaxTypes = 5,
        Moment = 4,
        World = 3,
        Ending = 2,
        Arrest = 1,
        Intro = 0,
    };
    enum eTrackDirection {
        MaxTrackDirections = 3,
        BackwardDir = 2,
        ForwardDir = 1,
        NoDir = 0,
    };
};

#endif
