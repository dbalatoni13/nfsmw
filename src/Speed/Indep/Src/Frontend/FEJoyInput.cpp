#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"

#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"

void MUTEX_lock(MUTEX* m);
void MUTEX_unlock(MUTEX* m);

void MyMutex::Lock() {
    MUTEX_lock(&mMutex);
}

void MyMutex::Unlock() {
    MUTEX_unlock(&mMutex);
}

int MyMutex::AddRef() {
    return ++mRefcount;
}

int MyMutex::Release() {
    int ref = --mRefcount;
    if (ref == 0) {
        delete this;
    }
    return ref;
}

IMutex* MyMutex::CreateInstance() {
    return new MyMutex();
}

int MyThread::AddRef() {
    return ++mRefcount;
}

int MyThread::Release() {
    int ref = --mRefcount;
    if (ref == 0) {
        delete this;
    }
    return ref;
}

IThread* MyThread::CreateInstance() {
    return new MyThread();
}
