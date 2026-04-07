 #include "Speed/Indep/Src/EAXSound/States/STATE_Main.hpp"

CSTATE_Main::StateInfo CSTATE_Main::s_StateInfo = {
    0x00000000,
    "CSTATE_Main",
    &CSTATE_Base::s_StateInfo,
    CSTATE_Main::CreateState,
};

CSTATE_Main::StateInfo *CSTATE_Main::GetStateInfo() const {
    return &s_StateInfo;
}

const char *CSTATE_Main::GetStateName() const {
    return s_StateInfo.stateName;
}

CSTATE_Base *CSTATE_Main::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return new (AudioMemBase::operator new(sizeof(CSTATE_Main), GetStaticStateInfo()->stateName, false)) CSTATE_Main;
    }
    return new (AudioMemBase::operator new(sizeof(CSTATE_Main), GetStaticStateInfo()->stateName, true)) CSTATE_Main;
}

CSTATE_Main::CSTATE_Main()
    : CSTATE_Base() {}

CSTATE_Main::~CSTATE_Main() {}
