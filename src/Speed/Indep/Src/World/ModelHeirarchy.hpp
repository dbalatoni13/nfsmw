#ifndef WORLD_MODELHEIRARCHY_H
#define WORLD_MODELHEIRARCHY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

struct ModelHeirarchy {
    struct Node {
        unsigned int mNameHash;
        unsigned int mPad0;
        unsigned int mModel;
        unsigned short mPad1;
        unsigned char mNumChildren;
        unsigned char mFirstChild;
    };

    unsigned int mNameHash;
    unsigned char mNumNodes;
    unsigned char mFlags;
    unsigned short pad;

    const Node *GetNodes() const {
        return reinterpret_cast< const Node * >(this + 1);
    }
};

#endif
