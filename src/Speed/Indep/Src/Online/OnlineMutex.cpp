#include "NetworkMutex.hpp"

#include "Speed/Indep/bWare/Inc/Strings.hpp"

class OnlineMutex : public NetworkMutex {
  public:
    OnlineMutex(const char *mutexname);

    void Lock(const char *logtext);
    void Unlock(const char *logtext);
    bool IsLocked() const;
    void ShowLogText();
    void HideLogText();

  private:
    /* 0x04 */ int32 lockCount;
    /* 0x08 */ bool logTextEnabled;
    /* 0x0c */ char mutexName[64];
};

OnlineMutex::OnlineMutex(const char *mutexname)
    : lockCount(0), //
      logTextEnabled(false) {
    mutexName[0] = '\0';
    if (mutexname) {
        bStrNCpy(mutexName, mutexname, 63);
    }
}

void OnlineMutex::Lock(const char *logtext) {
    NetworkMutex::Lock();
    volatile int32 &count = lockCount;
    count++;
}

void OnlineMutex::Unlock(const char *logtext) {
    volatile int32 &count = lockCount;
    count--;
    NetworkMutex::Unlock();
}

bool OnlineMutex::IsLocked() const { return lockCount > 0; }
