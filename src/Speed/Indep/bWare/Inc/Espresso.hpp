#ifndef BWARE_ESPRESSO_H
#define BWARE_ESPRESSO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct FloatVector {
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4
    float z; // offset 0x8, size 0x4
};

// TODO from debug build
inline void espEmptyLayer(const char *layername) {}

inline int espGetLayerState(const char *layername) {
    return 0;
}

inline unsigned int espCreateObject(const char *layername, const char *type, FloatVector *position) {
    return 0;
}

inline void espCreateObjectAsync(const char *layername, const char *type, FloatVector *position) {}

inline void espSetObjectPosition(unsigned int hobj, FloatVector *pvector) {}

inline void espSetAttributeString(unsigned int hobj, const char *attributename, const char *value) {}

inline void espSetAttributeFloat(unsigned int hobj, const char *attributename, float value) {}

inline void espCreateUserMesh(unsigned int hobj, int num_faces) {}

inline void espSetUserMeshFace(unsigned int hobj, int face_num, FloatVector *vertices) {}

inline void espLinkObject(unsigned int hobj, unsigned int linked_hobj) {}

// TODO where is this?
static const int RemoteCaffeinating = 0;

#endif
