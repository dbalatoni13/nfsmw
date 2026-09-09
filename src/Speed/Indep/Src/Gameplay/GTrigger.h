#ifndef GAMEPLAY_GTRIGGER_H
#define GAMEPLAY_GTRIGGER_H

#include "GRuntimeInstance.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Gameplay/GActivity.h"
#include "Speed/Indep/Src/World/Common/WGridNode.h"
#include "Speed/Indep/Src/World/WTrigger.h"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

// TODO do these go here?
DECLARE_CONTAINER_TYPE(ID_SimObjList);

// Decl: 25
typedef UTL::Std::vector<ISimable *, _type_ID_SimObjList> SimObjList;

// total size: 0xCC
// Decl: 31
class GTrigger : public GRuntimeInstance {
  public:
    GTrigger(const Attrib::Key &triggerKey);
    ~GTrigger() override;

    // Overrides: GRuntimeInstance
    // Decl: 36
    GameplayObjType GetType() const override {
        return kGameplayObjType_State;
    }

    void NotifySimableTrigger(ISimable *isim, int triggerStimulus);

    void GetPosition(struct UMath::Vector3 &pos);

    void Update(float dT);

    void AddActivationReference();

    void RemoveActivationReference();

    GActivity *GetTargetActivity();

    void Enable(bool setEnabled);

    // Decl: 48
    // void Disable() {}

    void Reset();

    void ShowIcon();
    void HideIcon();

    void EnableParticleEffects(bool enabled);

    void RefreshParticleEffects();

    // const UMath::Vector3 &GetDirection() const {} // Decl: 57

    float GetRadius();

    // Decl: 60
    bool IsEnabled() const {
        return mTriggerEnabled != 0;
    }

    // struct GIcon *GetIcon() const {} // Decl: 61

  private:
    void MarkAsInside(ISimable *simable);
    void MarkAsOutside(ISimable *simable);
    bool IsInside(ISimable *simable);

    EmitterGroup *CreateParticleEffect(const char *effectName, struct UMath::Vector3 &pos);
    void CreateAllParticleEffects();
    void ClearParticleEffects();

    static void NotifyEmitterGroupDelete(void *obj, EmitterGroup *group);

  private:
    WTrigger mWorldTrigger;                 // offset 0x28, size 0x40, Decl: 79
    struct UMath::Vector3 mDirection;       // offset 0x68, size 0xC, Decl: 80
    WGridNodeElemTag mTriggerEnabled;       // offset 0x74, size 0x4, Decl: 81
    SimObjList mSimObjInside;               // offset 0x78, size 0x10, Decl: 82
    char mTriggerEventData[16];             // offset 0xA8, size 0x10, Decl: 84
    EmitterGroup *mParticleEffect[2];       // offset 0xB8, size 0x8, Decl: 85
    struct GIcon *mIcon;                    // offset 0xC0, size 0x4, Decl: 86
    bool mEnabled;                          // offset 0xC4, size 0x1, Decl: 88
    int mActivationReferences;              // offset 0xC8, size 0x4, Decl: 89
    CARP::EventList mEventList;             // offset 0x88, size 0x10
    CARP::EventStaticData mEventStaticData; // offset 0x98, size 0x10
};

#endif
