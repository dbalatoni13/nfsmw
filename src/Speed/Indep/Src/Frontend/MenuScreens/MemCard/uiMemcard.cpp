#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"

struct UIMemcardBoot : public UIMemcardBase {
    UIMemcardBoot(ScreenConstructorData* sd) : UIMemcardBase(sd) {}
    ~UIMemcardBoot() override {}

    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                             unsigned long param2) override;
    eMenuSoundTriggers NotifySoundMessage(unsigned long msg,
                                          eMenuSoundTriggers maybe) override;
};

eMenuSoundTriggers UIMemcardBoot::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    return maybe;
}
