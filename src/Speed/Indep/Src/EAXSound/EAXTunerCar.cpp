struct bVector3;
struct eView;

extern eView eViews[];

struct EAXTunerCar : public EAXCar {
    float m_fCarVolume;          // offset 0x118
    bool BottomOutPlay;          // offset 0x11C
    int BottomOutIntensity;      // offset 0x120
    bool TrunkBouncePlay;        // offset 0x124
    int TrunkBounceInstensity;   // offset 0x128
    bool PlayBackFire;           // offset 0x12C
    bool bFirstUpdate;           // offset 0x130

    EAXTunerCar();
    virtual ~EAXTunerCar();
    virtual void PreLoadAssets() override;
    virtual void UpdateParams(float t) override;
    virtual StateInfo *GetStateInfo(void) const override;
    virtual char *GetStateName(void) const override;
    virtual void ProcessSoundSphere(unsigned int unamehash, int nparamid, bVector3 *pv3pos, float fradius) override;
    virtual int SFXMessage(eSFXMessageType SFXMessageType, unsigned int param1, unsigned int param2) override;
    virtual void UpdatePov() override;
    virtual int UpdateRotation() override;
    virtual int Play(void *peventst);

    void FirstUpdate(float t);

    static CSTATE_Base *CreateState(unsigned int allocator);

    void *operator new(size_t, void *p) { return p; }

    static StateInfo s_StateInfo;
};

CSTATE_Base::StateInfo EAXTunerCar::s_StateInfo = {
    0x00020000,
    "EAXTunerCar",
    &EAXCar::s_StateInfo,
    reinterpret_cast<CSTATE_Base *(*)(void)>(&EAXTunerCar::CreateState),
};

CSTATE_Base::StateInfo *EAXTunerCar::GetStateInfo(void) const {
    return &s_StateInfo;
}

char *EAXTunerCar::GetStateName(void) const {
    return s_StateInfo.stateName;
}

CSTATE_Base *EAXTunerCar::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXTunerCar), s_StateInfo.stateName, false)) EAXTunerCar;
    } else {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXTunerCar), s_StateInfo.stateName, true)) EAXTunerCar;
    }
}

EAXTunerCar::EAXTunerCar()
    : EAXCar() //
{
    bFirstUpdate = true;
    TrunkBounceInstensity = 0.0f;
    BottomOutPlay = false;
    TrunkBouncePlay = false;
    PlayBackFire = false;
}

EAXTunerCar::~EAXTunerCar() {}

void EAXTunerCar::PreLoadAssets() {}

void EAXTunerCar::ProcessSoundSphere(unsigned int unamehash, int nparamid, bVector3 *pv3pos, float fradius) {}

int EAXTunerCar::SFXMessage(eSFXMessageType SFXMessageType, unsigned int param1, unsigned int param2) {
    switch (SFXMessageType) {
    case SFX_NONE:
        break;
    case SFX_SHIFT_UP:
    case SFX_SHIFT_DOWN:
    case SFX_NITROUS:
        break;
    case SFX_BOTTOMOUT:
        BottomOutPlay = true;
        BottomOutIntensity = param1 >> 8;
        break;
    case SFX_TRUNKBOUNCE: {
        int t = param1;
        TrunkBouncePlay = true;
        TrunkBounceInstensity = t;
        break;
    }
    case SFX_CHANGEGEAR:
        return 0;
    default:
        break;
    }
    return EAXCar::SFXMessage(SFXMessageType, param1, param2);
}

int EAXTunerCar::UpdateRotation() {
    int val = 0;
    *(int *)&_pad_eaxcar[0xC0 - 0x44] = val;
    if (val > 0x400) {
        val = 0x400;
        *(int *)&_pad_eaxcar[0xC0 - 0x44] = val;
    }
    return val;
}

void EAXTunerCar::UpdatePov() {
    char *view1 = (char *)&eViews[1];
    void **pNext = (void **)(view1 + 0x3C);
    void *sentinel = (void *)(view1 + 0x3C);
    char *cm = 0;
    if (*pNext != sentinel) {
        cm = (char *)*pNext;
    }
    if (cm == 0) {
        *(int *)&_pad_eaxcar[0xBC - 0x44] = 0;
    } else {
        int *vtable2 = *(int **)(cm + 0x8);
        short vthis_off = *(short *)((char *)vtable2 + 0x28);
        int (*vfunc)(char *) = (int (*)(char *))(*(int *)((char *)vtable2 + 0x2C));
        char *anchor = (char *)vfunc(cm + vthis_off);

        *(int *)&_pad_eaxcar[0xBC - 0x44] = (*(int *)(cm + 0xC) == 1);

        if (anchor == 0) {
            *(int *)&_pad_eaxcar[0xB8 - 0x44] = 7;
        } else {
            *(int *)&_pad_eaxcar[0xB8 - 0x44] = static_cast<int>(*(short *)(anchor + 0xD8));
        }
    }
}

void EAXTunerCar::FirstUpdate(float t) {
    bFirstUpdate = false;
}

void EAXTunerCar::UpdateParams(float t) {
    EAXCar::UpdateParams(t);
    if (m_pCar != 0) {
        if (bFirstUpdate) {
            FirstUpdate(t);
        }
        UpdatePov();
        UpdateRotation();
    }
}

int EAXTunerCar::Play(void *peventst) {
    return 0;
}

void DebugPrintSkidBar(int Horz, int Vert, char *Str, int Value) {}
