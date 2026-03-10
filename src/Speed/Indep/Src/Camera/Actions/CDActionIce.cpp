#include "Speed/Indep/Src/Camera/Actions/CDActionIce.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"

extern ICEManager TheICEManager;
extern int Tweak_ForceICEReplay;
#include "Speed/Indep/Src/Camera/ICE/ICEMover.hpp"

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionIce("ICE", CDActionIce::Construct);

const Attrib::StringKey &CDActionIce::GetName() const {
    static Attrib::StringKey name("ICE");
    return name;
}

Attrib::StringKey CDActionIce::GetNext() const {
    if (!mDone) {
        return Attrib::StringKey("");
    }
    return mPrev;
}

CameraMover *CDActionIce::GetMover() {
    return mMover;
}

bool CDActionIce::Attach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Attach(pOther);
}

bool CDActionIce::Detach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Detach(pOther);
}

bool CDActionIce::IsAttached(const UTL::COM::IUnknown *pOther) const {
    return mAttachments->IsAttached(pOther);
}

const IAttachable::List *CDActionIce::GetAttachments() const {
    return &mAttachments->GetList();
}

CameraAI::Action *CDActionIce::Construct(CameraAI::Director *director) {
    IPlayer *player = nullptr;
    {
        const UTL::Vector<IPlayer *, 16> &list = IPlayer::GetList(PLAYER_LOCAL);
        for (IPlayer *const *iter = list.begin(); iter != list.end(); ++iter) {
            IPlayer *ip = *iter;
            if (ip->GetRenderPort() == director->GetViewID()) {
                player = ip;
                break;
            }
        }
    }
    if (!player) return nullptr;
    if (!player->GetSettings()) return nullptr;
    ISimable *isimable = player->GetSimable();
    if (!isimable) return nullptr;
    {
        unsigned int world_id = isimable->GetWorldID();
        if (!world_id) return nullptr;
        bool have_a_track = TheICEManager.ChooseCameraPlaybackTrack();
        if (!have_a_track && !Tweak_ForceICEReplay) return nullptr;
        return new ((const char *)nullptr) CDActionIce(director, player);
    }
}

CDActionIce::CDActionIce(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(), //
      IAttachable(this),   //
      mActionQ(false) {
    // TODO
}

CDActionIce::~CDActionIce() {
    // TODO
}

void CDActionIce::OnDetached(IAttachable *pOther) {
    // TODO
}

void CDActionIce::ReleaseCar(bool detach) {
    // TODO
}

void CDActionIce::AquireCar() {
    // TODO
}

void CDActionIce::Update(float dT) {
    // TODO
}
