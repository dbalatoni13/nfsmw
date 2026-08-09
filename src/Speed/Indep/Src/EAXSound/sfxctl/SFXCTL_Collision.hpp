//
#ifndef SFXCTL_COLLISION_HPP
#define SFXCTL_COLLISION_HPP

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"

// total size: 0x64
// Decl: 10
class SFXCTL_3DColPos : public SFXCTL_3DObjPos {
  public:
    DECLARE_CREATABLE();

    SFXCTL_3DColPos() {}           // Decl: 13
    ~SFXCTL_3DColPos() override {} // Decl: 14
};

// total size: 0x64
// Decl: 20
class SFXCTL_3DScrapePos : public SFXCTL_3DObjPos {
  public:
    DECLARE_CREATABLE();

    SFXCTL_3DScrapePos() {}           // Decl: 23
    ~SFXCTL_3DScrapePos() override {} // Decl: 24
};

#endif
