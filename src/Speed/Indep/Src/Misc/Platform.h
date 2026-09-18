#ifndef MISC_PLATFORM_H
#define MISC_PLATFORM_H

// I just made this up to store declarations for Platform.cpp
// TODO where to put these?

void InitPlatform();
void InitDisplaySystem();
void ServicePlatform();
extern "C" int bDoWithStack(void *function, void *stack_pointer, int arg1, int arg2);
void EnableInterrupts();
void DVDErrorTask(void *, int); // TODO remove and put where needed
void FlushCaches();
void FinishedRenderingFEngLayer();
int DVDValidErrorState(int state);
int GC_GetOSLanguage();
void CheckReset(int reset);

// total size: 0x1C
struct VMStats {
    VMStats();
    void Init();

    unsigned int mNumPageFaults;        // offset 0x0, size 0x4
    unsigned int mNumWritebacks;        // offset 0x4, size 0x4
    float mElapsedTime;                 // offset 0x8, size 0x4
    unsigned int mServiceTimeMicroSecs; // offset 0xC, size 0x4
    unsigned int mServiceTimeMin;       // offset 0x10, size 0x4
    unsigned int mServiceTimeMax;       // offset 0x14, size 0x4
    float mServiceTimeAvg;              // offset 0x18, size 0x4
};

// El constructor va antes que Init: la llama fuera de linea (Init__7VMStats),
// mientras que VMStatsManager::Init, definida despues, la inlinea.
inline VMStats::VMStats() {
    Init();
}

inline void VMStats::Init() {
    this->mNumPageFaults = 0;
    this->mNumWritebacks = 0;
    this->mElapsedTime = 0.0f;
    this->mServiceTimeMicroSecs = 0;
    this->mServiceTimeMin = -1;
    this->mServiceTimeMax = 0;
    this->mServiceTimeAvg = 0.0f;
}

// total size: 0x54
struct VMStatsManager {
    VMStatsManager(const char *name);
    void Init(const char *name);

    bool mInitialized;                   // offset 0x0, size 0x1
    unsigned long long mFrameCounter;    // offset 0x8, size 0x8
    VMStats mFrameStats;                 // offset 0x10, size 0x1C
    float mElapsedTime;                  // offset 0x2C, size 0x4
    unsigned int mAccumService_us;       // offset 0x30, size 0x4
    unsigned int mAccumNumFaults;        // offset 0x34, size 0x4
    float mMinServicePercentPerFrame;    // offset 0x38, size 0x4
    float mMaxServicePercentPerFrame;    // offset 0x3C, size 0x4
    unsigned int mMinNumServicesPerFrame; // offset 0x40, size 0x4
    unsigned int mMaxNumServicesPerFrame; // offset 0x44, size 0x4
    float mMinFrameTime;                 // offset 0x48, size 0x4
    float mMaxFrameTime;                 // offset 0x4C, size 0x4
    const char *DebugName;               // offset 0x50, size 0x4
};

// Igual que en VMStats: el cuerpo de Init (Platform_G.cpp) llega despues, asi
// que el constructor la llama fuera de linea.
inline VMStatsManager::VMStatsManager(const char *name) {
    Init(name);
    this->mInitialized = false;
}

void FinishedRenderingFEngLayer();

#endif
