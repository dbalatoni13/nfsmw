#ifndef ANIMATION_ANIMCHOOSER_H
#define ANIMATION_ANIMCHOOSER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x4
class CAnimChooser {
  public:
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

    CAnimChooser();

    virtual ~CAnimChooser();

    char *ChooseAnim(eType type, char *animName, int &camera_track_number);
};

#endif
