#ifndef GAMEPLAY_GREFLECTED_H
#define GAMEPLAY_GREFLECTED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class GRuntimeInstance;

// total size: 0x4
struct GCollectionKey {
    GCollectionKey(unsigned int key) : mCollectionKey(key) {}

    GCollectionKey(GRuntimeInstance *inst);

    unsigned int GetCollectionKey() const {
        return mCollectionKey;
    }

    operator unsigned int() {
        return mCollectionKey;
    }

    operator GRuntimeInstance *() const;

    unsigned int mCollectionKey; // offset 0x0, size 0x4
};

#endif
