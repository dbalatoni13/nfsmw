#include "FEJoyInput.hpp"

#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"

void MyMutex::Lock() {
    MUTEX_lock(&mMutex);
}

void MyMutex::Unlock() {
    MUTEX_unlock(&mMutex);
}

void MyThread::Sleep(int ticks) {
    THREAD_sleep(ticks);
}
