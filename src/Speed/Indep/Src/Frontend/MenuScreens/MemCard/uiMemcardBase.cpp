#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"

void FEngSetScript(const char* pkg_name, unsigned int obj_hash, unsigned int script_hash,
                   bool start_at_beginning);

void UIMemcardKeyboard::Abort() {}

void UIMemcardKeyboard::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                                             unsigned long param2) {
    const unsigned long FEObj_PC_NAME_ENTRY = 0xC9D30688;
    if (msg == FEObj_PC_NAME_ENTRY) {
        FEngSetScript(GetPackageName(), 0x47FF4E7C, 0x03D8EABC, true);
    }
}

void UIMemcardBase::DoSelect(const char* pFileName) {}

unsigned int UIMemcardBase::GetAutoSaveWarning() {
    return 0xb39899c2;
}

unsigned int UIMemcardBase::GetAutoSaveWarning2() {
    return 0x2386f454;
}
