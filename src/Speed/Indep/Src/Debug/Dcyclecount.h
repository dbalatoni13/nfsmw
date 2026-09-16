#ifndef _DCYCLECOUNT_H_
#define _DCYCLECOUNT_H_ // Decl: 3

// Decl: 12
enum eProfMeter {
    kPROFInvalid = 0,
    kPROFTotalFrame = 1,
    kPROFDrawWait = 2,
    kPROFTempA = 3,
    kPROFTempB = 4,
    kPROFTempC = 5,
    kPROFRCar = 6,
    kPROFHUD = 7,
    kPROFCamera = 8,
    kPROFSimMain = 9,
    kPROFSimPhys = 10,
    kPROFSimVehicle = 11,
    kPROFSimSmack = 12,
    kPROFSimRigid = 13,
    kPROFSimCollision = 14,
    kPROFSimAIVeh = 15,
    kPROFSimTasks = 16,
    kPROFSimAI = 17,
    kPROFSimGameplay = 18,
    kPROFAudio = 19,
    kPROFUpdateFrame = 20,
    kPROFUpdateRender = 21,
    kPROFUpdateAudio = 22,
    kPROFRender = 23,
    kPROFCollider = 24,
    kPROFMessages = 25,
    kPROFEvents = 26,
    kPROFFeng = 27,
    kPROFMain = 28,
    kPROFRDebug = 29,
    kPROFRTempA = 30,
    kPROFNumMeters = 31,
};

#define PROFILE_ENABLED 0 // Decl: 47

#define DCYCLE_SCOPE(_mask_) (void)0    // Decl: 236
#define DCYCLE_RENDER() (void)0         // Decl: 237
#define DCYCLE_DRAW() (void)0           // Decl: 238
#define DCYCLE_UPDATENIGHTRUN() (void)0 // Decl: 239
#define DCYCLE_RESULT(_mask_) 0.f       // Decl: 240
#define DCYCLE_INIT(_SN_PROF_) (void)0  // Decl: 241
#define DCYCLE_NAME(_mask_) NULL        // Decl: 242
#define DCYCLE_ENDFRAME() (void)0       // Decl: 243

#endif
