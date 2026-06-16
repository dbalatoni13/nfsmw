#ifndef GUSERINCLUDES_H__
#define GUSERINCLUDES_H__

#define GMANAGER_COREVAULT "gpcore"

#define GAssert bAssert
#define GAssertMsg bAssertMsg

#define GMalloc bMalloc
#define GFree bFree
#define GMemSet bMemSet
#define GMemCpy bMemCpy
#define GMemCmp bMemCmp
#define GOverlappedMemCpy bOverlappedMemCpy

#define GStrCmp bStrCmp
#define GPrintf bPrintf
#define GSPrintf bSPrintf
#define GLocalHash bStringHash
#define GStrLen bStrLen

#define DEFINE_GRUNTIMINSTANCE_CUSTOM_FLAGS                                                                                                          \
    kFlag_TriggerSpeedTrap = 1 << 8, kFlag_TriggerEngage = 1 << 9, kFlag_TriggerFinishLine = 1 << 10, kFlag_TriggerTimeBonus = 1 << 11,              \
    kFlag_TriggerCashBag = 1 << 12, kFlag_TriggerCarLot = 1 << 13, kFlag_TriggerCustomShop = 1 << 14, kFlag_TriggerSafeHouse = 1 << 15,              \
    kFlag_TriggerShowOnExit = 1 << 3, kFlag_DefectorTrigger = 1 << 6, kFlag_AutoEffectCreation = 1 << 4,

#define DEFINE_GOBJECTBLOCK_CREATEABLE_TYPE(_type_) buffer += this->CreateObjects<_type_>(this->mVault, buffer);

#define GOBJECTBLOCK_CREATE_CUSTOM_TYPES()                                                                                                           \
    DEFINE_GOBJECTBLOCK_CREATEABLE_TYPE(GCharacter) DEFINE_GOBJECTBLOCK_CREATEABLE_TYPE(GMarker) DEFINE_GOBJECTBLOCK_CREATEABLE_TYPE(GTrigger)

#define DEFINE_GOBJECTBLOCK_DESTROYABLE_TYPE(_type_) this->DeleteObjects<_type_>();

#define GOBJECTBLOCK_DESTROY_CUSTOM_TYPES()                                                                                                          \
    DEFINE_GOBJECTBLOCK_DESTROYABLE_TYPE(GTrigger) DEFINE_GOBJECTBLOCK_DESTROYABLE_TYPE(GMarker) DEFINE_GOBJECTBLOCK_DESTROYABLE_TYPE(GCharacter)

#define DEFINE_GOBJECTBLOCK_FINDABLE_TYPE(_type_) bytesUsed += FindInstances<_type_>(vault, NULL, &objCount, NULL);

#define GOBJECTBLOCK_CALC_CUSTOM_SIZES()                                                                                                             \
    DEFINE_GOBJECTBLOCK_FINDABLE_TYPE(GCharacter) DEFINE_GOBJECTBLOCK_FINDABLE_TYPE(GMarker) DEFINE_GOBJECTBLOCK_FINDABLE_TYPE(GTrigger)

enum GameplayObjType {
    kGameplayObjType_Invalid = -1,
    kGameplayObjType_Activity = 0,
    kGameplayObjType_Character = 1,
    kGameplayObjType_Handler = 2,
    kGameplayObjType_Marker = 3,
    kGameplayObjType_State = 4,
    kGameplayObjType_Trigger = 5,
    kGameplayObjType_Count = 6,
};

#endif
