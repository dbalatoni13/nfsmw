#ifndef REALCORE_SOURCE_INPUT_CMN_EVENT_H
#define REALCORE_SOURCE_INPUT_CMN_EVENT_H

// RealInput::Event (ver event.cpp para el layout verificado contra el asm).

#pragma interface

namespace RealInput {

void *AllocateMemSize(const char *file, int line, int a, int b, int size);
void FreeMemSize(void *ptr, int size);

class EventFields { // subobjeto no polimorfico de Event (0x20 bytes)
protected:
    int mDevice;              // +0x00
    int mType;                // +0x04
    int mPad8;                // +0x08
    int mPadC;                // +0x0C
    int mPad10;               // +0x10
    unsigned char mPad[0xC];  // +0x14..0x20
};

class Event : public EventFields {
public:
    Event();
    virtual ~Event();

    void operator delete(void *ptr) { FreeMemSize(ptr, sizeof(Event)); }
};

} // namespace RealInput

#endif
