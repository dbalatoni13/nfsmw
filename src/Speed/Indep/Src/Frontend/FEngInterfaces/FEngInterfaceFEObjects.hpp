#ifndef FENGINTERFACEFEOBJECTS_H
#define FENGINTERFACEFEOBJECTS_H

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"

#include "Speed/Indep/bWare/Inc/bMath.hpp"

FEObject *FEngFindObject(const char *pkg_name, uint32 obj_hash);
void FEngSetInvisible(FEObject *obj);
void FEngSetVisible(FEObject *obj);

inline void FEngSetInvisible(const char *pkg_name, uint32 obj_hash) {
    FEngSetInvisible(FEngFindObject(pkg_name, obj_hash));
}

inline void FEngSetVisible(const char *pkg_name, uint32 obj_hash) {
    FEngSetVisible(FEngFindObject(pkg_name, obj_hash));
}

inline void FEngSetVisibility(FEObject *obj, bool visible) {
    if (visible) {
        FEngSetVisible(obj);
    } else {
        FEngSetInvisible(obj);
    }
}

void FEngGetSize(FEObject *object, float &x, float &y);

inline float FEngGetSizeX(FEObject *obj) {
    float x, y;
    FEngGetSize(obj, x, y);
    return x;
};

void FEngGetTopLeft(FEObject *object, float &x, float &y);

inline float FEngGetTopLeftX(FEObject *obj) {
    float x, y;
    FEngGetTopLeft(obj, x, y);
    return x;
}

inline float FEngGetSizeY(FEObject *obj) {
    float x, y;
    FEngGetSize(obj, x, y);
    return y;
};

FEScript *FEngGetScript(FEObject *object, uint32 script_hash);

FEScript *FEngGetScript(const char *pkg_name, uint32 obj_hash, uint32 script_hash);

void FEngSetScript(FEObject *object, uint32 script_hash, bool start_at_beginning);

void FEngSetScript(const char *pkg_name, uint32 obj_hash, uint32 script_hash, bool start_at_beginning);

FEColor FEngGetObjectColor(FEObject *obj);

inline uint32 FEngGetColor(FEObject *obj) {
    return FEngGetObjectColor(obj);
};

void FEngSetScript(FEObject *object, uint32 script_hash, bool start_at_beginning);

void FEngSetScript(const char *pkg_name, uint32 obj_hash, uint32 script_hash, bool start_at_beginning);

void FEngSetTopLeft(FEObject *object, float x, float y);

void FEngSetSize(FEObject *object, float x, float y);

inline void FEngSetTopLeftY(FEObject *obj, float y);

void FEngSetVisible(FEObject *obj);
void FEngSetInvisible(FEObject *obj);
inline bool FEngIsVisible(FEObject *obj) {
    return obj != nullptr && !(obj->Flags & FENG_OBJECT_INVISIBLE);
}
void FEngSetScript(FEObject *object, uint32 script_hash, bool start_at_beginning);
void FEngGetCenter(FEObject *object, float &x, float &y);
inline float FEngGetCenterX(FEObject *obj) {
    float x;
    float y;
    FEngGetCenter(obj, x, y);
    return x;
}

inline float FEngGetCenterY(FEObject *obj) {
    float x;
    float y;
    FEngGetCenter(obj, x, y);
    return y;
}

void FEngGetSize(FEObject *object, float &x, float &y);
void FEngSetCenter(FEObject *object, float x, float y);

inline void FEngSetCenterY(FEObject *obj, float y) {
    float x = FEngGetCenterX(obj);
    FEngSetCenter(obj, x, y);
}
void FEngGetTopLeft(FEObject *object, float &x, float &y);
void FEngSetTopLeft(FEObject *object, float x, float y);
void FEngSetCurrentButton(const char *pkg_name, uint32 hash);

inline void FEngSetCurrentButton(const char *pkg_name, FEObject *obj) {
    FEngSetCurrentButton(pkg_name, obj->NameHash);
}

bool FEngIsScriptSet(const char *pkg_name, uint32 obj_hash, uint32 script_hash);
bool FEngIsScriptSet(FEObject *obj, uint32 script_hash);

void FEngSetRotationZ(FEObject *obj, float angle_degrees);

void FEngSetColor(FEObject *object, bVector4 *color);

// Range: 0x80131F40 -> 0x80131F84
void FEngSetColor(FEObject *object, uint32 color);

// Range: 0xFFFFFFFF -> 0x80131F84
void FEngSetColor(FEObject *object, FEColor &c);

FEObject *FEngFindObject(const char *pkg_name, uint32 obj_hash);

inline void FEngSetColor(const char *pkg_name, uint32 obj, uint32 color) {
    FEngSetColor(FEngFindObject(pkg_name, obj), color);
}

// Range: 0x80130E74 -> 0x80130EA8
bool FEngIsScriptRunning(const char *pkg_name, uint32 obj_hash, uint32 script_hash);

// Range: 0x80130EA8 -> 0x80130EE4
bool FEngIsScriptRunning(FEObject *object, uint32 script_hash);

void FEngSetMultiImageRot(FEMultiImage *image, float angle_degrees);

void FEngGetBottomRight(FEObject *object, float &x, float &y);

inline float FEngGetBottomRightX(FEObject *obj) {
    float x, y;
    FEngGetBottomRight(obj, x, y);
    return x;
}

void FEngSetBottomRight(FEObject *object, float x, float y);

inline float FEngGetTopLeftY(FEObject *obj) {
    float x, y;
    FEngGetTopLeft(obj, x, y);
    return y;
}

inline float FEngGetBottomRightY(FEObject *obj) {
    float x, y;
    FEngGetBottomRight(obj, x, y);
    return y;
}

inline void FEngSetSizeX(FEObject *obj, float x) {
    float y = FEngGetSizeY(obj);
    FEngSetSize(obj, x, y);
}

inline void FEngSetSizeY(FEObject *obj, float y) {
    float x = FEngGetSizeX(obj);
    FEngSetSize(obj, x, y);
}

inline void FEngSetTopLeftY(FEObject *obj, float y) {
    float x = FEngGetTopLeftX(obj);
    FEngSetTopLeft(obj, x, y);
}

inline void FEngSetTopLeftX(FEObject *obj, float x) {
    float y = FEngGetTopLeftY(obj);
    FEngSetTopLeft(obj, x, y);
}

inline void FEngSetBottomRightX(FEObject *obj, float x) {
    float y = FEngGetBottomRightY(obj);
    FEngSetBottomRight(obj, x, y);
}

void FEngSetAllObjectsInPackageVisibility(const char *pkg_name, bool visible);
void FEngSetInvisible(FEObject *obj);

void FEngSetBottomRightUV(FEImage *img, float u, float v);
void FEngGetBottomRightUV(FEImage *img, float &u, float &v);

inline float FEngGetBottomRightV(FEImage *img) {
    float u, v;
    FEngGetBottomRightUV(img, u, v);
    return v;
}

inline void FEngSetBottomRightU(FEImage *img, float u) {
    float v = FEngGetBottomRightV(img);
    FEngSetBottomRightUV(img, u, v);
}

void FEngSetScaleX(FEObject *object, float x);
void FEngSetScaleY(FEObject *object, float y);

float FEngGetScaleX(FEObject *object);
float FEngGetScaleY(FEObject *object);

bool FEngTestForIntersection(float xPos, float yPos, FEObject *obj);
bool FEngTestForIntersection(const float xPos, const float yPos, const bVector2 &top_left, const bVector2 &size);

FEGroup *FEngFindGroup(const char *pkg_name, uint32 grp_hash);

void FEngSetMultiImageBottomRightUVs(FEMultiImage *image, FEVector2 &bottomRightUVs, int textureNumber);

#endif
