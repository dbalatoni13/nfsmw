#ifndef FENGINTERFACEFEOBJECTS_H
#define FENGINTERFACEFEOBJECTS_H

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"

FEObject *FEngFindObject(const char *pkg_name, uint32 obj_hash);
void FEngSetInvisible(FEObject *obj);
void FEngSetVisible(FEObject *obj);

inline void FEngSetInvisible(const char *pkg_name, uint32 obj_hash) {
    FEngSetInvisible(FEngFindObject(pkg_name, obj_hash));
}

inline void FEngSetVisible(const char *pkg_name, uint32 obj_hash) {
    FEngSetVisible(FEngFindObject(pkg_name, obj_hash));
}

void FEngGetSize(FEObject *object, float &x, float &y);

inline float FEngGetSizeX(struct FEObject *obj) {
    float x, y;
    FEngGetSize(obj, x, y);
    return x;
};

void FEngGetTopLeft(FEObject *object /* r31 */, float &x /* r26 */, float &y /* r25 */);

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

void FEngSetScript(FEObject *object, uint32 script_hash, bool start_at_beginning);

void FEngSetScript(const char *pkg_name, uint32 obj_hash, uint32 script_hash, bool start_at_beginning);

FEColor FEngGetObjectColor(FEObject *obj /* r4 */);

inline uint32 FEngGetColor(FEObject *obj) {
    return FEngGetObjectColor(obj);
};

inline void FEngDisableButton(const char *pkg_name, uint32 button_hash) {}

void FEngSetScript(FEObject *object, uint32 script_hash, bool start_at_beginning);

void FEngSetScript(const char *pkg_name, uint32 obj_hash, uint32 script_hash, bool start_at_beginning);

inline void FEngSetCurrentButton(const char *pkg_name, FEObject *obj) {}

void FEngSetTopLeft(struct FEObject *object /* r31 */, float x /* f29 */, float y /* f28 */);

void FEngSetSize(struct FEObject *object /* r3 */, float x /* f1 */, float y /* f2 */);

inline void FEngSetTopLeftY(struct FEObject *obj, float y);

void FEngSetVisible(FEObject *obj);
void FEngSetInvisible(FEObject *obj);
void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
void FEngGetCenter(FEObject *object, float &x, float &y);
void FEngGetSize(FEObject *object, float &x, float &y);
void FEngSetCenter(FEObject *object, float x, float y);
void FEngGetTopLeft(FEObject *object, float &x, float &y);
void FEngSetTopLeft(FEObject *object, float x, float y);
void FEngSetCurrentButton(const char *pkg_name, unsigned int hash);

bool FEngIsScriptSet(const char *pkg_name /* r3 */, unsigned int obj_hash /* r4 */, unsigned int script_hash /* r30 */);
bool FEngIsScriptSet(struct FEObject *obj /* r3 */, unsigned int script_hash /* r4 */);

void FEngSetRotationZ(struct FEObject *obj /* r31 */, float angle_degrees /* f1 */);

void FEngSetColor(struct FEObject *object /* r3 */, struct bVector4 *color /* r4 */);

// Range: 0x80131F40 -> 0x80131F84
void FEngSetColor(struct FEObject *object /* r31 */, unsigned int color /* r4 */);

// Range: 0xFFFFFFFF -> 0x80131F84
void FEngSetColor(struct FEObject *object /* r3 */, struct FEColor &c /* r4 */);

FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);

inline void FEngSetColor(const char *pkg_name, unsigned int obj, unsigned int color) {
    FEngSetColor(FEngFindObject(pkg_name, obj), color);
}

// Range: 0x80130E74 -> 0x80130EA8
bool FEngIsScriptRunning(const char *pkg_name /* r3 */, unsigned int obj_hash /* r4 */, unsigned int script_hash /* r30 */);

// Range: 0x80130EA8 -> 0x80130EE4
bool FEngIsScriptRunning(struct FEObject *object /* r3 */, unsigned int script_hash /* r4 */);

void FEngSetMultiImageRot(struct FEMultiImage *image /* r3 */, float angle_degrees /* f1 */);

void FEngGetBottomRight(struct FEObject *object /* r31 */, float &x /* r26 */, float &y /* r25 */);

inline float FEngGetBottomRightX(FEObject *obj) {
    float x, y;
    FEngGetBottomRight(obj, x, y);
    return x;
}

void FEngSetBottomRight(struct FEObject *object /* r31 */, float x /* f31 */, float y /* f30 */);

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

void FEngSetAllObjectsInPackageVisibility(const char *pkg_name /* r0 */, bool visible /* r31 */);
void FEngSetInvisible(FEObject *obj);

void FEngSetBottomRightUV(struct FEImage *img /* r31 */, float u /* f31 */, float v /* f30 */);
void FEngGetBottomRightUV(struct FEImage *img /* r31 */, float &u /* r30 */, float &v /* r29 */);

void FEngSetScaleX(struct FEObject *object /* r30 */, float x /* f31 */);
void FEngSetScaleY(struct FEObject *object /* r30 */, float y /* f31 */);

float FEngGetScaleX(struct FEObject *object /* r3 */);
float FEngGetScaleY(struct FEObject *object /* r3 */);

bool FEngTestForIntersection(float xPos /* f31 */, float yPos /* f30 */, struct FEObject *obj /* r3 */);
bool FEngTestForIntersection(const float xPos /* f31 */, const float yPos /* f30 */, const struct bVector2 &top_left /* r3 */,
                             const struct bVector2 &size /* r4 */);

FEGroup *FEngFindGroup(const char *pkg_name /* r3 */, uint32 grp_hash /* r4 */);

#endif
