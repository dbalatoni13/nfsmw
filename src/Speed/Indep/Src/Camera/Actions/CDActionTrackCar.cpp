#include "Speed/Indep/Src/Camera/Actions/CDActionTrackCar.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionTrackCar("TRACKCAR", CDActionTrackCar::Construct);

const Attrib::StringKey &CDActionTrackCar::GetName() const {
    static Attrib::StringKey name("TRACKCAR");
    return name;
}

Attrib::StringKey CDActionTrackCar::GetNext() const {
    return Attrib::StringKey();
}

bool CDActionTrackCar::Attach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Attach(pOther);
}

bool CDActionTrackCar::Detach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Detach(pOther);
}

bool CDActionTrackCar::IsAttached(const UTL::COM::IUnknown *pOther) const {
    return mAttachments->IsAttached(pOther);
}

const IAttachable::List *CDActionTrackCar::GetAttachments() const {
    return &mAttachments->GetList();
}

CameraAI::Action *CDActionTrackCar::Construct(CameraAI::Director *director) {
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
        return new ((const char *)nullptr) CDActionTrackCar(director, player);
    }
}

CDActionTrackCar::CDActionTrackCar(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(), //
      IAttachable(this) {
    // TODO
}

CDActionTrackCar::~CDActionTrackCar() {
    // TODO
}

void CDActionTrackCar::OnDetached(IAttachable *pOther) {
    // TODO
}

void CDActionTrackCar::OnCarDetached() {
    // TODO
}

void CDActionTrackCar::AquireCar() {
    // TODO
}

void CDActionTrackCar::Update(float dT) {
    // TODO
}

bool CDActionTrackCar::GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) {
    // TODO
    return false;
}
