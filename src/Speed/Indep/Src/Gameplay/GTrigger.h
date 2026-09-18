#ifndef GAMEPLAY_GTRIGGER_H
#define GAMEPLAY_GTRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GRuntimeInstance.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/World/WTrigger.h"

class GActivity;
class GIcon;

DECLARE_CONTAINER_TYPE(ID_SimObjList);

// total size: 0xCC
class GTrigger : public GRuntimeInstance {
  public:
    GTrigger(const unsigned int &triggerKey);
    ~GTrigger() override;

    GameplayObjType GetType() const {
        return kGameplayObjType_Trigger;
    }

    GActivity *GetTargetActivity();
    void AddActivationReference();
    void RemoveActivationReference();
    EmitterGroup *CreateParticleEffect(const char *effectName, UMath::Vector3 &position);
    void CreateAllParticleEffects();
    void ClearParticleEffects();
    void EnableParticleEffects(bool enable);
    void RefreshParticleEffects();
    static void NotifyEmitterGroupDelete(void *context, EmitterGroup *group);
    void Enable(bool enable);
    void GetPosition(UMath::Vector3 &position);
    void NotifySimableTrigger(ISimable *simable, int eventType);
    void Reset();
    void ShowIcon();
    void HideIcon();
    void MarkAsInside(ISimable *simable);
    void MarkAsOutside(ISimable *simable);
    bool IsInside(ISimable *simable);

    void Disable() {
        Enable(false);
    }

    const UMath::Vector3 &GetDirection() const {
        return mDirection;
    }

    bool IsEnabled() const {
        return mEnabled;
    }

    bool IsWorldTriggerEnabled() const {
        return mTriggerEnabled != 0;
    }

    GIcon *GetIcon() const {
        return mIcon;
    }

  private:
    WTrigger mWorldTrigger;                                          // offset 0x28, size 0x40
    UMath::Vector3 mDirection;                                       // offset 0x68, size 0xC
    unsigned int mTriggerEnabled;                                    // offset 0x74, size 0x4
    UTL::Std::vector<ISimable *, _type_ID_SimObjList> mSimObjInside; // offset 0x78, size 0x10
    CARP::EventList mEventList;                                      // offset 0x88, size 0x10
    CARP::EventStaticData mEventStaticData;                          // offset 0x98, size 0x10
    unsigned char mTriggerEventData[16];                             // offset 0xA8, size 0x10
    EmitterGroup *mParticleEffect[2];                                // offset 0xB8, size 0x8
    GIcon *mIcon;                                                    // offset 0xC0, size 0x4
    bool mEnabled;                                                   // offset 0xC4, size 0x1
    int mActivationReferences;                                       // offset 0xC8, size 0x4
};

#endif
