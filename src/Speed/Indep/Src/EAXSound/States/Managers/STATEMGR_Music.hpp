#ifndef STATEMGR_MUSIC_HPP
#define STATEMGR_MUSIC_HPP

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"

// total size: 0x1C
// Decl: 8
class CSTATEMGR_Music : public CSTATEMGR_Base {
  public:
    CSTATEMGR_Music();

    // Overrides: AudioMemBase
    ~CSTATEMGR_Music() override;

    // Overrides: CSTATEMGR_Base
    void EnterWorld(eSndGameMode esgm) override;
};

#endif
