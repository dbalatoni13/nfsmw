#include "Stomper.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern StompEntry *StomperTable[8];
static Stomper *pStomper;

void *StompEntry::Clear(void *pStack) {
    float *fptr = static_cast<float *>(pStack);
    switch (type) {
    case 0:
        *reinterpret_cast<unsigned char *>(address) = *reinterpret_cast<unsigned char *>(fptr);
        fptr = fptr + 1;
        break;
    case 1:
        *reinterpret_cast<unsigned short *>(address) = *reinterpret_cast<unsigned short *>(fptr);
        fptr = fptr + 1;
        break;
    case 2:
        *reinterpret_cast<float *>(address) = *fptr;
        fptr = fptr + 1;
        break;
    case 3:
        *reinterpret_cast<float *>(address) = *fptr;
        fptr = fptr + 1;
        break;
    case 4:
        reinterpret_cast<void (*)(int, int)>(address)(0, *reinterpret_cast<int *>(fptr));
        fptr = fptr + 1;
        break;
    case 5:
        reinterpret_cast<void (*)(float, int)>(address)(*fptr, 0);
        fptr = fptr + 1;
        break;
    case 6:
        fptr = static_cast<float *>(StompEntry::ClearAll(fptr));
        break;
    }
    return fptr;
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