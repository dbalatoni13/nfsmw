#ifndef ANIMATION_ANIMPROPERTY_H
#define ANIMATION_ANIMPROPERTY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"

enum eAnimProperty {
    eAnimProp_ControlRaceCountdown = 0,
    eAnimProp_ControlRaceCars = 1,
    eAnimProp_UnBindRaceCars = 2,
    eAnimProp_MaxAnimProperty = 3,
};

typedef int AnimHandle;

// total size: 0x14
class CAnimProperty : public bTNode<CAnimProperty> {
  public:
    CAnimProperty(eAnimProperty type, bool enabled);
    virtual ~CAnimProperty();
    eAnimProperty GetType();
    void SetEnabled(bool enabled);
    bool IsEnabled();

  private:
    eAnimProperty mType; // offset 0x8, size 0x4
    int mEnabled;        // offset 0xC, size 0x4
};

#endif
