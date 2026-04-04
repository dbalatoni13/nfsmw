struct EAXTrafficCar : public CSTATE_Base {
    bool IsLargeTrafficCar; // offset 0x44

    EAXTrafficCar();
    virtual ~EAXTrafficCar();
    virtual void Attach(void *pAttachment) override;
    virtual bool Detach() override;
    virtual StateInfo *GetStateInfo(void) const override;
    virtual const char *GetStateName(void) const override;

    static CSTATE_Base *CreateState(unsigned int allocator);

    void *operator new(size_t, void *p) { return p; }

    static StateInfo s_StateInfo;
};

CSTATE_Base::StateInfo EAXTrafficCar::s_StateInfo = {
    0x00050000,
    "EAXTrafficCar",
    &CSTATE_Base::s_StateInfo,
    reinterpret_cast<CSTATE_Base *(*)(unsigned int)>(&EAXTrafficCar::CreateState),
};

CSTATE_Base::StateInfo *EAXTrafficCar::GetStateInfo(void) const {
    return &s_StateInfo;
}

const char *EAXTrafficCar::GetStateName(void) const {
    return s_StateInfo.stateName;
}

CSTATE_Base *EAXTrafficCar::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXTrafficCar), s_StateInfo.stateName, false)) EAXTrafficCar;
    } else {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXTrafficCar), s_StateInfo.stateName, true)) EAXTrafficCar;
    }
}

EAXTrafficCar::EAXTrafficCar() : CSTATE_Base() {}

EAXTrafficCar::~EAXTrafficCar() {}

void EAXTrafficCar::Attach(void *pAttachment) {
    IsLargeTrafficCar = false;
    m_pCar = static_cast<EAX_CarState *>(pAttachment);
    CSTATE_Base::Attach(pAttachment);
}

bool EAXTrafficCar::Detach() {
    CSTATE_Base::Detach();
    m_pCar = nullptr;
    return true;
}
