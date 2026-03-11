struct CSTATE_Base;

namespace Sound {

struct CollisionEvent {
    char _pad[0xC8];
    CSTATE_Base *Owner;

    void SetOwner(CSTATE_Base *owner);
    void Pause(bool pause);
};

} // namespace Sound

void Sound::CollisionEvent::SetOwner(CSTATE_Base *owner) {
    Owner = owner;
}

void Sound::CollisionEvent::Pause(bool pause) {}
