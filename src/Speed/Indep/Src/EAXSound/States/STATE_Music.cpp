 #include "Speed/Indep/Src/EAXSound/States/STATE_Music.hpp"

CSTATE_Music::StateInfo CSTATE_Music::s_StateInfo = {
    0x01010000,
    "CSTATE_Music",
    &CSTATE_Base::s_StateInfo,
    CSTATE_Music::CreateState,
};

CSTATE_Music::StateInfo *CSTATE_Music::GetStateInfo() const {
    return &s_StateInfo;
}

const char *CSTATE_Music::GetStateName() const {
    return s_StateInfo.stateName;
}

CSTATE_Base *CSTATE_Music::CreateState(unsigned int allocator) {
    return new CSTATE_Music;
}

CSTATE_Music::CSTATE_Music()
    : CSTATE_Base() {}

CSTATE_Music::~CSTATE_Music() {}
