#include "Stomper.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern StompEntry *StomperTable[8];
static Stomper *pStomper;

void *StompEntry::Clear(void *pStack) {
    switch (type) {
    case 0:
        *reinterpret_cast<unsigned char *>(address) = *static_cast<unsigned char *>(pStack);
        return static_cast<char *>(pStack) + 4;
    case 1:
        *reinterpret_cast<unsigned short *>(address) = *static_cast<unsigned short *>(pStack);
        return static_cast<char *>(pStack) + 4;
    case 2:
        *reinterpret_cast<int *>(address) = *static_cast<int *>(pStack);
        return static_cast<char *>(pStack) + 4;
    case 3:
        *reinterpret_cast<float *>(address) = *static_cast<float *>(pStack);
        return static_cast<char *>(pStack) + 4;
    case 4:
        reinterpret_cast<void (*)(int, int)>(address)(0, *static_cast<int *>(pStack));
        return static_cast<char *>(pStack) + 4;
    case 5:
        reinterpret_cast<void (*)(float, int)>(address)(*static_cast<float *>(pStack), 0);
        return static_cast<char *>(pStack) + 4;
    case 6:
        return reinterpret_cast<StompEntry *>(address)->ClearAll(pStack);
    }
    return pStack;
}

void *StompEntry::ClearAll(void *pStack) {
    StompEntry *entry = this;
    while (entry->type != 7) {
        pStack = entry->Clear(pStack);
        entry = entry + 1;
    }
    return pStack;
}

Stomper::Stomper() {
    StomperType = 1;
    pData = operator new[](0x400);
    CurrentTable = 0;
}

void Stomper::Clear() {
    if (static_cast<unsigned int>(CurrentTable - 1) < 6) {
        StomperTable[CurrentTable]->ClearAll(pData);
    }
    CurrentTable = 0;
}

void InitStomper() {
    pStomper = new Stomper();
}

void ResetRenderEggs() {
    if (pStomper != nullptr) {
        pStomper->Clear();
    }
}

void ActivateAnyRenderEggs() {}