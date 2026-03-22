#ifndef GAMEPLAY_GTRIGGER_H
#define GAMEPLAY_GTRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GRuntimeInstance.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/World/WTrigger.h"

struct GIcon;

DECLARE_CONTAINER_TYPE(ID_SimObjList);

// total size: 0x10
struct EventList {
    unsigned int fNumEvents; // offset 0x0, size 0x4
    unsigned int fPad[3];    // offset 0x4, size 0xC
};

// total size: 0x10
struct EventStaticData {
    unsigned int fEventID;   // offset 0x0, size 0x4
    unsigned int fEventSize; // offset 0x4, size 0x4
    unsigned int fDataOffset; // offset 0x8, size 0x4
    unsigned int fPad;       // offset 0xC, size 0x4
};

// total size: 0xCC
class GTrigger : public GRuntimeInstance {
  public:
    GTrigger(const Attrib::Key &triggerKey);

    ~GTrigger() override;

    GameplayObjType GetType() const override {
        return kGameplayObjType_Trigger;
    }

    void Disable() {
        Enable(false);
    }

    const UMath::Vector3 &GetDirection() const {
        return mDirection;
    }

    bool IsEnabled() const {
        return mEnabled;
    }

    bool GetTriggerEnabled() const {
        return mTriggerEnabled != 0;
    }

    GIcon *GetIcon() const {
        return mIcon;
    }

    GActivity *GetTargetActivity();

    void AddActivationReference();

    void RemoveActivationReference();

    EmitterGroup *CreateParticleEffect(const char *effectName, UMath::Vector3 &pos);

    void CreateAllParticleEffects();

    void ClearParticleEffects();

    void EnableParticleEffects(bool enabled);

    void RefreshParticleEffects();

    static void NotifyEmitterGroupDelete(void *obj, EmitterGroup *group);

    void Enable(bool setEnabled);

    void GetPosition(UMath::Vector3 &pos);

    float GetRadius();

    void NotifySimableTrigger(ISimable *isim, int triggerStimulus);

    void Reset();

    void ShowIcon();

    void HideIcon();

    void MarkAsInside(ISimable *simable);

    void MarkAsOutside(ISimable *simable);

    bool IsInside(ISimable *simable);

    void Update(float dT);

  private:
    WTrigger mWorldTrigger;                                      // offset 0x28, size 0x40
    UMath::Vector3 mDirection;                                   // offset 0x68, size 0xC
    unsigned int mTriggerEnabled;                                // offset 0x74, size 0x4
    UTL::Std::vector<ISimable *, _type_ID_SimObjList> mSimObjInside; // offset 0x78, size 0x10
    EventList mEventList;                                        // offset 0x88, size 0x10
    EventStaticData mEventStaticData;                            // offset 0x98, size 0x10
    unsigned char mTriggerEventData[16];                         // offset 0xA8, size 0x10
    EmitterGroup *mParticleEffect[2];                            // offset 0xB8, size 0x8
    GIcon *mIcon;                                                // offset 0xC0, size 0x4
    bool mEnabled;                                               // offset 0xC4, size 0x1
    int mActivationReferences;                                   // offset 0xC8, size 0x4
};

#endif
