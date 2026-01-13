#ifndef ANIMATION_ANIMCHOOSER_H
#define ANIMATION_ANIMCHOOSER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x4
struct CAnimChooser {
    enum eType {
        Intro = 0,
        Arrest = 1,
        Ending = 2,
        World = 3,
        Moment = 4,
        MaxTypes = 5,
    };
    enum eTrackDirection {
        NoDir = 0,
        ForwardDir = 1,
        BackwardDir = 2,
        MaxTrackDirections = 3,
    };
};

#endif
