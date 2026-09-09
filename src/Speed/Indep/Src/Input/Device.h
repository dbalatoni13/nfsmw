#ifndef INPUT_DEVICE_H
#define INPUT_DEVICE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

namespace EA { namespace Allocator { class IAllocator; } }

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

struct Device;
struct Event;
struct Interface;

// total size: 0x14
struct ConfigOptions {
    EA::Allocator::IAllocator* mAllocator; // offset 0x0, size 0x4
    int (*mpEnumDevicesCallback)(Device*, unsigned int, Interface*); // offset 0x4, size 0x4
    unsigned int mEnumDevicesCallbackUserData; // offset 0x8, size 0x4
    unsigned int mEventQueueSize; // offset 0xC, size 0x4
    unsigned int mMaxNumEffects; // offset 0x10, size 0x4

    ConfigOptions() {
        mAllocator = nullptr;
        mpEnumDevicesCallback = nullptr;
        mEnumDevicesCallbackUserData = 0;
    }
};

// total size: 0x4
struct Interface {
    static Interface* CreateInstance(const ConfigOptions& options);

    virtual int AddRef();

    virtual int Release();

    virtual void Update();

    virtual Device* GetPad();

    virtual Device* GetMouse();

    virtual Device* GetKeyboard();

    virtual Event* GetEvent();

    virtual ~Interface();
};

// total size: 0x4
struct Effect {
    // total size: 0x4
    struct Info {
        unsigned int mFullStop; // offset 0x0, size 0x4

        Info(); // 0x803998C8
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

    virtual Device* GetDevice();

    virtual void GetInfo(Info* info);

    virtual void SetInfo(Info* info);
};

struct Capabilities {
    uint32_t mNumDigitalButtons; // offset 0x0, size 0x4
    uint32_t mNumAnalogButtons; // offset 0x4, size 0x4
    unsigned int mAttached : 1; // offset 0x8, size 0x4
    unsigned int mForceFeedback : 1; // offset 0x8, size 0x4
    unsigned int mUnused : 30; // offset 0x8, size 0x4
};

// UNSOLVED
// The three payload structs are only known by size, taken from the union's
// members in the original DWARF. Replace with the real layouts when found --
// only the 0x118 total matters here, it is what puts _vptr.Device at 0x138.
struct Pad {
    uint8_t mUnknown[0x118]; // offset 0x0, size 0x118
};
struct Keyboard {
    uint8_t mUnknown[0x100]; // offset 0x0, size 0x100
};
struct Mouse {
    uint8_t mUnknown[0x14]; // offset 0x0, size 0x14
};

union Data {
    Pad mPad; // offset 0x0, size 0x118
    Keyboard mKeyboard; // offset 0x0, size 0x100
    Mouse mMouse; // offset 0x0, size 0x14
};

// total size: 0x13C
struct Device {
    enum Type {
        TYPE_UNKNOWN = 0,
        TYPE_KEYBOARD = 1,
        TYPE_MOUSE = 2,
        TYPE_PAD = 3,
    };

    // total size: 0x14
    struct Info {
        Platform mPlatform; // offset 0x0, size 0x4
        Type mType; // offset 0x4, size 0x4
        uint32_t mJoypadID; // offset 0x8, size 0x4
        uint32_t mControllerID; // offset 0xC, size 0x4
        uint32_t mPortNum; // offset 0x10, size 0x4
    };

    Device();

    virtual ~Device();

    virtual Data* GetData();

    virtual RiResult Acquire();

    virtual RiResult Release();

    virtual RiResult Update();

    virtual Effect* CreateEffect(Effect::Info* info);

    virtual Effect* GetEffect();

    virtual unsigned int GetKeyState(unsigned int key);

    int32_t IsKeyboard() { return mInfo.mType == TYPE_KEYBOARD; }

    int32_t IsMouse() { return mInfo.mType == TYPE_MOUSE; }

    int32_t IsPad() { return mInfo.mType == TYPE_PAD; }

    Info* GetInfo() { return &mInfo; }

    Capabilities* GetCapabilities() { return &mCapabilities; }

private:
    void InitData();

protected:
    Info mInfo; // offset 0x0, size 0x14
    Capabilities mCapabilities; // offset 0x14, size 0xC
    Data mData; // offset 0x20, size 0x118
};

} // namespace RealInput

#endif
