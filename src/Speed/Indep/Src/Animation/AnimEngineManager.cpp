#include "AnimEngineManager.hpp"
#include "AnimPlayer.hpp"
#include "Speed/Indep/Src/EAGL4Anim/System.h"
#include "Speed/Indep/Src/EAGL4Anim/eagl4supportbase.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#ifdef EA_BUILD_A124
// STRIPPED
void DumpNFSAnimMemUsage() {}
#endif

void InitNFSAnimEngine() {
    CAnimEngineManager::Init();
    TheAnimPlayer.Init();
}

// STRIPPED
void CloseNFSAnimEngine() {}

// STRIPPED
void ReportNFSAnimMemoryUsage() {}

// STRIPPED
void ResetNFSAnimMemory() {}

// STRIPPED
CAnimEngineManager::CAnimEngineManager() {}

// STRIPPED
CAnimEngineManager::~CAnimEngineManager() {}

// STRIPPED
void *MyEAGLNewOverride(size_t size, const char *name) {}

// STRIPPED
void MyEAGLDeleteOverride(void *ptr, size_t) {}

void *MyEAGLMallocOverride(size_t size, const char *name) {
#ifdef EA_BUILD_A124
    char *buf = reinterpret_cast<char *>(bMalloc(size, name ? name : "Unnamed MyEAGLNewOverride", 0, 0));
#else
    bool is_main_pool = name ? bStrCmp(name, "EAGL4Anim Memory Pool") == 0 : false;
    char *buf = reinterpret_cast<char *>(bMalloc(size, name, 0, is_main_pool ? GetVirtualMemoryAllocParams() : 0));
#endif

    return buf;
}

void MyEAGLFreeOverride(void *ptr, size_t) {
    bFree(ptr);
}

void CAnimEngineManager::Init() {
    EAGL4Internal::SetMallocOverride(&MyEAGLMallocOverride);
    EAGL4Internal::SetFreeOverride(&MyEAGLFreeOverride);
#ifdef EA_BUILD_A124
    EAGL4Anim::Init(0xC800, true);
#else
    EAGL4Anim::Init(0x14000, false);
#endif
}

// STRIPPED
void CAnimEngineManager::Purge() {}
