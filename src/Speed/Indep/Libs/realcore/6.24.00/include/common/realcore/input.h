#ifndef REALCORE_COMMON_INPUT_H
#define REALCORE_COMMON_INPUT_H

#include "types.h"

namespace RealInput {

enum Platform {
    PLATFORM_WIN = 0,
    PLATFORM_MAC = 1,
    PLATFORM_PS2 = 2,
    PLATFORM_XBOX = 3,
    PLATFORM_GC = 4,
    PLATFORM_XENON = 5,
    PLATFORM_PSP = 6,
    PLATFORM_MAX = 7,
};

enum RiResult {
    RI_OK = 0,
    RI_EVENT_QUEUE_FULL = 1,
    RI_EVENT_NOT_FOUND = 2,
    RI_FUNCTION_NOT_IMPLEMENTED = 3,
    RI_ACQUIRE_FAILED = 4,
    RI_NO_DIRECTINPUT_DEVICE = 5,
    RI_FAILED_GETTING_DEVICE_STATE = 6,
    RI_NO_EFFECT = 7,
};

struct AXES {
    int mX;
    int mY;
    int mZ;
};

struct Device;

struct Effect {
    struct Info {
        unsigned int mFullStop;
    };

    enum Status {
        STATUS_STOPPED = 0,
        STATUS_PLAYING = 1,
    };

    Effect();
    virtual ~Effect();
    virtual void Start();
    virtual void Stop();
    virtual Status GetStatus();
    virtual Device *GetDevice();
    virtual void GetInfo(Info *info);
    virtual void SetInfo(Info *info);
};

void FreeMemSize(void *pBlock, int size);

struct Device {
    enum Type {
        TYPE_UNKNOWN = 0,
        TYPE_KEYBOARD = 1,
        TYPE_MOUSE = 2,
        TYPE_PAD = 3,
    };

    struct Info {
        Platform mPlatform;
        Type mType;
        unsigned int mJoypadID;
        unsigned int mControllerID;
        unsigned int mPortNum;
    };

    struct Capabilities {
        unsigned int mNumDigitalButtons;
        unsigned int mNumAnalogButtons;
        unsigned int mAttached : 1;
        unsigned int mForceFeedback : 1;
        unsigned int mUnused : 30;
    };

    struct Pad {
        AXES mDirection[2];
        int mButtons[64];
    };

    struct Keyboard {
        unsigned char mMap[256];
    };

    struct Mouse {
        int mX;
        int mY;
        int mZ;
        unsigned char mButtons[8];
    };

    union Data {
        Pad mPad;
        Keyboard mKeyboard;
        Mouse mMouse;
    };

    Device();
    Device(Platform platform, Type type);
    virtual ~Device();

    virtual Data *GetData() {
        return &this->mData;
    }

    virtual RiResult Acquire() {
        return RI_OK;
    }

    virtual RiResult Release() {
        return RI_OK;
    }

    virtual RiResult Update() {
        return RI_OK;
    }

    virtual Effect *CreateEffect(Effect::Info *) {
        return nullptr;
    }

    virtual Effect *GetEffect() {
        return nullptr;
    }

    virtual unsigned int GetKeyState(unsigned int) {
        return 0;
    }

    static void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, size);
    }

  private:
    void InitData();

  protected:
    Info mInfo;
    Capabilities mCapabilities;
    Data mData;
};

} // namespace RealInput

#endif
