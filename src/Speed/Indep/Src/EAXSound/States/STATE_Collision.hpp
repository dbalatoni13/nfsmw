#ifndef COLLISIONSTATE_HPP
#define COLLISIONSTATE_HPP

#include "Speed/Indep/Src/EAXSound/SndBase.hpp"

// total size: 0x48
// Decl: 11
class CSTATE_Collision : public CSTATE_Base {
  public:
    DECLARE_STATETYPE();

    CSTATE_Collision();
    ~CSTATE_Collision() override;

    // Overrides: CSTATE_Base
    void Attach(void *pAttachment) override;
    bool Detach() override;

    Sound::CollisionEvent *m_pCollisionEvent; // offset 0x44, size 0x4, Decl: 21
};

#endif
