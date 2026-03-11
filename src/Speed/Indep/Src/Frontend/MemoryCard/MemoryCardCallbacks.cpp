#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"

void DisplayStatus(int i) {}

MemoryCard* MemcardCallbacks::GetMemcard() { return MemoryCard::GetInstance(); }
UIMemcardBase* MemcardCallbacks::GetScreen() { return MemoryCard::GetInstance()->GetScreen(); }
