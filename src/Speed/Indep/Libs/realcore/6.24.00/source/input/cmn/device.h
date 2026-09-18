#ifndef REALCORE_SOURCE_INPUT_CMN_DEVICE_H
#define REALCORE_SOURCE_INPUT_CMN_DEVICE_H

#pragma interface

// RealInput::Device (ver device.cpp para el layout verificado contra el asm).

namespace RealInput {

enum Platform { PC = 0, PS2 = 1, XBOX = 2, GAMECUBE = 3 };

void *AllocateMemSize(const char *file, int line, int a, int b, int size);
void FreeMemSize(void *ptr, int size);

class Device;

class Effect {
public:
    class Info {
      public:
        unsigned int mFullStop;
        Info();
    };

    Effect();
    virtual ~Effect();
    virtual void Start();
    virtual void Stop();
    virtual int GetStatus();
    virtual Device *GetDevice();
    virtual void GetInfo(Info *info);
    virtual void SetInfo(Info *info);

    void operator delete(void *ptr) { FreeMemSize(ptr, sizeof(Effect)); }
};

class DeviceImp { // subobjeto no polimorfico de Device (0x138 bytes)
public:
    // total size: 0x14
    struct Info {
        int mPlatform;               // +0x00
        int mType;                   // +0x04
        unsigned int mJoypadID;      // +0x08
        unsigned int mControllerID;  // +0x0C
        unsigned int mPortNum;       // +0x10
    };
    // total size: 0xC
    struct Capabilities {
        unsigned int mNumDigitalButtons; // +0x00
        unsigned int mNumAnalogButtons;  // +0x04
        unsigned int mAttached : 1;      // +0x08
        unsigned int mForceFeedback : 1;
        unsigned int mUnused : 30;
    };

protected:
    int mPlatform;                 // +0x000
    int mType;                     // +0x004
    unsigned char mPad0[0xC];      // +0x008
    struct { int a, b, c; } mStat; // +0x014 (0xC bytes)
    unsigned char mData[0x118];    // +0x020 (GetData)
};

class Device : public DeviceImp {
public:
    enum Type { TYPE_UNKNOWN = 0, TYPE_KEYBOARD = 1, TYPE_MOUSE = 2, TYPE_PAD = 3 };

    Device(Platform platform, Type type);
    virtual ~Device();

    int IsPad() {
        return this->mType == TYPE_PAD;
    }

    Info *GetInfo() {
        return (Info *)this;
    }

    Capabilities *GetCapabilities() {
        return (Capabilities *)&this->mStat;
    }

    void InitData();

    virtual void *GetData();
    virtual int Acquire();
    virtual int Release();
    virtual int Update();
    virtual Effect *CreateEffect(Effect::Info *info);
    virtual Effect *GetEffect();
    virtual int GetKeyState(unsigned int key);

    void operator delete(void *ptr) { FreeMemSize(ptr, sizeof(Device)); }
};

} // namespace RealInput

#endif
