#ifndef SUPPORT_MISC_CARP_H
#define SUPPORT_MISC_CARP_H

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UGroup.hpp"
#include "Speed/Indep/Tools/EventSys/Runtime/Common/eventsysdata.h"

struct WCollisionArticle;

namespace CARP {

struct Map {};

// total size: 0x10
struct EventStaticData {
    // char *StaticData() {}

    const char *StaticData() const {
        return reinterpret_cast<const char *>(this) + fDataOffset;
    }

    // EventStaticData *NextEvent() {}

    // const EventStaticData *NextEvent() const {}

    unsigned int fEventID;    // offset 0x0, size 0x4
    unsigned int fEventSize;  // offset 0x4, size 0x4
    unsigned int fDataOffset; // offset 0x8, size 0x4
    unsigned int fPad;        // offset 0xC, size 0x4
};

// total size: 0x10
struct EventList {
    EventStaticData *Event() {
        return reinterpret_cast<EventStaticData *>(&this[1]);
    }

    const EventStaticData *Event() const {
        return reinterpret_cast<const EventStaticData *>(&this[1]);
    }

    // const EventStaticData *Event() const {}

    unsigned int fNumEvents; // offset 0x0, size 0x4
    unsigned int fPad[3];    // offset 0x4, size 0xC
};

// total size: 0x4
struct TagReference {
    unsigned int fData; // offset 0x0, size 0x4

    TagReference() {}
    TagReference(unsigned int tag) {}

    TagReference(const UGroup *context);
};

// total size: 0x40
struct CollisionInstance {
    bVector4 fInvMatRow0Width;                          // offset 0x0, size 0x10
    unsigned short fIterStamp;                          // offset 0x10, size 0x2
    mutable unsigned short fFlags;                      // offset 0x12, size 0x2
    float fHeight;                                      // offset 0x14, size 0x4
    unsigned short fGroupNumber;                        // offset 0x18, size 0x2
    unsigned short fRenderInstanceInd;                  // offset 0x1A, size 0x2
    mutable const WCollisionArticle *fCollisionArticle; // offset 0x1C, size 0x4
    bVector4 fInvMatRow2Length;                         // offset 0x20, size 0x10
    bVector4 fInvPosRadius;                             // offset 0x30, size 0x10
};

struct CollisionSurface {
    unsigned char fSurface;
    unsigned char fFlags;
};

// total size: 0x70
struct CollisionObject {
    UMath::Vector4 fPosRadius;         // offset 0x0, size 0x10
    UMath::Vector4 fDimensions;        // offset 0x10, size 0x10
    unsigned char fType;               // offset 0x20, size 0x1
    unsigned char fShape;              // offset 0x21, size 0x1
    unsigned short fFlags;             // offset 0x22, size 0x2
    unsigned short fRenderInstanceInd; // offset 0x24, size 0x2
    CollisionSurface fSurface;         // offset 0x26, size 0x2
    float fPAD[2];                     // offset 0x28, size 0x8
    UMath::Matrix4 fMat;               // offset 0x30, size 0x40
};

// total size: 0x40
struct Trigger {
    bool ValidateMatrix() const;
    void MakeMatrix(UMath::Matrix4 &m, bool addXLate) const;

    UMath::Vector4 fMatRow0Width;  // offset 0x0, size 0x10
    unsigned int fType : 4;        // offset 0x10, size 0x4
    unsigned int fShape : 4;       // offset 0x10, size 0x4
    unsigned int fFlags : 24;      // offset 0x10, size 0x4
    float fHeight;                 // offset 0x14, size 0x4
    CARP::EventList *fEvents;      // offset 0x18, size 0x4
    unsigned short fIterStamp;     // offset 0x1C, size 0x2
    unsigned short fFingerprint;   // offset 0x1E, size 0x2
    UMath::Vector4 fMatRow2Length; // offset 0x20, size 0x10
    UMath::Vector4 fPosRadius;     // offset 0x30, size 0x10
};

union ExprValType { // 0x4
	/* 0x0 */ float f;
	/* 0x0 */ unsigned int u;
	/* 0x0 */ int i;
	/* 0x0 */ bool b;
};

struct StimulusFilter {
    unsigned int mNumQueries; // offset 0x0, size 0x4
    unsigned int mNumInputs; // offset 0x4, size 0x4
    unsigned int mExpressionSize; // offset 0x8, size 0x4
    unsigned int mPad; // offset 0xC, size 0x4

    QueryDesc * GetQueries() {}

    struct Expression * GetExpression() {}

    char * GetStaticData() {}

    const QueryDesc *GetQueries() const {
        return reinterpret_cast<const QueryDesc *>(&this[1]);
    }

    const struct Expression * GetExpression() const {}

    const char * GetStaticData() const {}
};

unsigned int ResolveTagReferences(const UGroup *g, unsigned int deltaAddress);

}; // namespace CARP

#endif
