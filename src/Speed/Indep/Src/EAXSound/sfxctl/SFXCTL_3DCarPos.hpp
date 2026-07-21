#ifndef SFXCTL_3DCARPOS_HPP
#define SFXCTL_3DCARPOS_HPP

#include "SFXCTL_3DObjPos.hpp"

// total size: 0x64
// Decl: 8
class SFXCTL_3DCarPos : public SFXCTL_3DObjPos {
  public:
    DECLARE_CREATABLE();

    SFXCTL_3DCarPos();
    ~SFXCTL_3DCarPos() override;
};

#endif
