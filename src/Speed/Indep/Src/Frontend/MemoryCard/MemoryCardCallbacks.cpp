#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"

int MyMutex::AddRef() {
    return ++mRefcount;
}

int MyThread::AddRef() {
    return ++mRefcount;
}

void DisplayStatus(int i) {}

MemoryCard* MemcardCallbacks::GetMemcard() {
    return MemoryCard::GetInstance();
}

UIMemcardBase* MemcardCallbacks::GetScreen() {
    return MemoryCard::GetInstance()->GetScreen();
}
