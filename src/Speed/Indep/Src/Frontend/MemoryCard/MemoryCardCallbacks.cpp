#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"

void DisplayStatus(int i) {}

MemoryCard* MemcardCallbacks::GetMemcard() {
    return MemoryCard::GetInstance();
}

UIMemcardBase* MemcardCallbacks::GetScreen() {
    return MemoryCard::GetInstance()->GetScreen();
}
