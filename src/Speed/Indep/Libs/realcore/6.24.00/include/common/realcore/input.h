#ifndef REALCORE_COMMON_INPUT_H
#define REALCORE_COMMON_INPUT_H

#include "Allocator/iallocator.h"
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

void FreeMemSize(void *pBlock, int size);

struct Effect {
    struct Info {
        unsigned int mFullStop;

        Info();
    };

    enum Status {
        STATUS_STOPPED = 0,
        STATUS_PLAYING = 1,
    };

    static inline void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, size);
    }

    Effect();
    virtual ~Effect();
    virtual void Start();
    virtual void Stop();
    virtual Status GetStatus();
    virtual Device *GetDevice();
    virtual void GetInfo(Info *info);
    virtual void SetInfo(Info *info);
};

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

    Capabilities *GetCapabilities() {
        return &this->mCapabilities;
    }

    Info *GetInfo() {
        return &this->mInfo;
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

enum Type {
    TYPE_CONNECTED = 0,
    TYPE_DISCONNECTED = 1,
    TYPE_PRESSED = 2,
    TYPE_RELEASED = 3,
    TYPE_CHANGE = 4,
};

enum Object {
    OBJECT_UNKNOWN = 0,
    OBJECT_BUTTON = 1,
    OBJECT_MDIRECTION = 2,
    OBJECT_DIRECTIONLEFT = 3,
    OBJECT_DIRECTIONRIGHT = 4,
};

struct Event {
    Event();
    virtual ~Event();

    static inline void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, size);
    }

    Device *mpDevice;
    Type mType;
    Object mObject;
    unsigned int mObjectIndex;
    unsigned int mTimestamp;
    unsigned int mVirtKey;
    unsigned int mASCIIKey;
    unsigned int mKeyState;
};

struct ConfigOptions;
struct DeviceImp;
struct EventQueue;
struct GcEffect;

struct Interface;

void SetAllocator(EA::Allocator::IAllocator *allocator);
void ReleaseAllocator();
void *AllocateMemSize(const char *name, int size, int align, int alignoffset, int flag);

struct ConfigOptions {
    ConfigOptions()
        : mAllocator(nullptr)
        , mpEnumDevicesCallback(nullptr)
        , mEnumDevicesCallbackUserData(0)
        , mEventQueueSize(0x20)
        , mMaxNumEffects(0) {
    }

    EA::Allocator::IAllocator *mAllocator;
    int (*mpEnumDevicesCallback)(Device *, unsigned int, Interface *);
    unsigned int mEnumDevicesCallbackUserData;
    unsigned int mEventQueueSize;
    unsigned int mMaxNumEffects;
};

struct Interface {
    static Interface *CreateInstance(const ConfigOptions &options);

    virtual int AddRef();
    virtual int Release();
    virtual void Update();
    virtual Device *GetPad();
    virtual Device *GetMouse();
    virtual Device *GetKeyboard();
    virtual Event *GetEvent();
    virtual ~Interface() {}
};

struct ITimer {
    static unsigned int GetTime();
};

} // namespace RealInput

#endif
