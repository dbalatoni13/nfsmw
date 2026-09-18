#ifndef EATHREAD_EATHREAD_SEMAPHORE_H
#define EATHREAD_EATHREAD_SEMAPHORE_H

// PlayStation 2. Tipos del DWARF de la alpha 124 (SLES-53558-A124): la misma
// interfaz que en GameCube, pero los datos son el id del semaforo del kernel y
// dos contadores enteros, y son protegidos.

struct PathSemaphore;

// total size: 0xC
struct EASemaphoreData {
    int mSemaphore;  // offset 0x0, size 0x4
    int mnCount;     // offset 0x4, size 0x4
    int mnWaitCount; // offset 0x8, size 0x4

    EASemaphoreData();
};

namespace EA {
namespace Thread {

// total size: 0x18
struct SemaphoreParameters {
    SemaphoreParameters(int initialCount, bool, const char *);

    int mInitialCount;   // offset 0x0, size 0x4
    bool mbIntraProcess; // offset 0x4, size 0x1
    char mName[16];      // offset 0x8, size 0x10
};

typedef unsigned int ThreadTime;

// total size: 0xC
class Semaphore {
  public:
    enum Result {
        kResultError = -1,
        kResultTimeout = -2,
    };

    Semaphore() {}

    Semaphore(const SemaphoreParameters *pSemaphoreParameters, bool bDefaultParameters);

    ~Semaphore();

    bool Init(const SemaphoreParameters *pSemaphoreParameters);

    int Wait(const ThreadTime &timeoutAbsolute);

    int Post(int count);

    int GetCount() const;

  protected:
    EASemaphoreData mSemaphoreData; // offset 0x0, size 0xC

  private:
    friend struct ::PathSemaphore;
    Semaphore(int initialCount);
};

void ThreadSleep(const ThreadTime &timeout);
unsigned int GetThreadTime();

} // namespace Thread
} // namespace EA

#endif
