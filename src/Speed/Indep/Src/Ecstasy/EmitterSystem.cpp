#include "EmitterSystem.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

unsigned int CrappyStuffThatCantShip[3];

bool IsAShittyEffect(unsigned int group_key) {
    int num_shitty_effects;
    for (int i = 0; i < 3; i++) {
        unsigned int crap_group_key = CrappyStuffThatCantShip[i];
        if (group_key == crap_group_key) {
            return true;
        }
    }
    return false;
}

void ExpandVector(const smVector3 *sv, bVector3 *bv) {
    bv->x = sv->magnitude * (1.0f / 127 * sv->x);
    bv->y = sv->magnitude * (1.0f / 127 * sv->y);
    bv->z = sv->magnitude * (1.0f / 127 * sv->z);
}

void CompressVector(const bVector3 *bv, smVector3 *sv) {
    bVector3 norm;

    float magn = bLength(bv);
    bScale(&norm, &norm, magn * 127.0f);

    sv->x = norm.x;
    sv->y = norm.y;
    sv->z = norm.z;
    sv->magnitude = magn;
}

void NotifyLibOfDeletion(void *lib /* r3 */, EmitterGroup *grp /* r4 */) {
    EmitterLibrary *emlib = reinterpret_cast<EmitterLibrary *>(lib);
    emlib->mGroup = nullptr;
}

// UNSOLVED typical cmpw bne blr instead of b
unsigned int EmitterGroup::GetNumParticles() {
    if (this->mNumEmitters == 0) {
        return 0;
    }
    unsigned int cnt = 0;
    for (Emitter *em = this->mEmitters.GetHead(); em != this->mEmitters.EndOfList(); em = em->GetNext()) {
        cnt += em->GetNumParticles();
    }
    return cnt;
}

// UNSOLVED typical cmpw bne blr instead of b
void EmitterSystem::OrphanParticlesFromThisEmitter(Emitter *em) {
    if (em->HasOrphanedParticles()) {
        return;
    }
    bTList<EmitterParticle> &plist = em->GetParticles();

    for (EmitterParticle *particle = plist.GetHead(); particle != plist.EndOfList();) {
        EmitterParticle *next = particle->GetNext();
        particle->mFlags |= 1;
        particle = next;
    }
    em->SetOrphanedParticlesFlag();
}

// UNSOLVED typical cmpw bne blr instead of b
void EmitterSystem::KillParticlesFromThisEmitter(Emitter *em /* r31 */) {
    bTList<EmitterParticle> &plist = em->GetParticles();

    for (EmitterParticle *particle = plist.GetHead(); particle != plist.EndOfList();) {
        EmitterParticle *next = particle->GetNext();
        this->KillParticle(em, particle);
        particle = next;
    }
}

void EmitterSystem::KillEverything() {
    EmitterGroup *pEVar1;
    EmitterGroup *grp;
    EmitterGroup *local_r3_72;
    EmitterGroup *pEVar2;

    // pEVar1 = (EmitterGroup *)(this->mEmitterGroups).__base.HeadNode.Next;
    // while (grp = pEVar1, grp != (EmitterGroup *)&this->mEmitterGroups) {
    //     pEVar2 = (EmitterGroup *)(grp->__base).__base.Next;
    //     pEVar1 = grp;
    //     if (((grp->mFlags & 2) == 0) && (pEVar1 = pEVar2, grp != (EmitterGroup *)0x0)) {
    //         EmitterGroup(grp, 3);
    //     }
    // }
    // local_r3_72 = (EmitterGroup *)GetNumParticleTextures();
    // SetParticleSystemStats(0, 0x400, 0, (int)local_r3_72, gEmitterSystem.mNumEmitters, 500, gEmitterSystem.mNumEmitterGroups, 200);
}
