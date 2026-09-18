#ifndef GREFLECTED_H__
#define GREFLECTED_H__

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class GRuntimeInstance;

// total size: 0x4
struct GCollectionKey {
    GCollectionKey(unsigned int key) : mCollectionKey(key) {}

    GCollectionKey(GRuntimeInstance *instance);

    operator GRuntimeInstance *() const;

    operator unsigned int() const {
        return mCollectionKey;
    }

    unsigned int GetCollectionKey() const {
        return mCollectionKey;
    }

    unsigned int mCollectionKey; // offset 0x0, size 0x4
};

#endif
