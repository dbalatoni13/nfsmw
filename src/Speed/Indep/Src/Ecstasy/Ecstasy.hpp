#ifndef ECSTASY_ECSTASY_H
#define ECSTASY_ECSTASY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "EcstasyData.hpp"
#include "EcstasyE.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Texture.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern SlotPool *eModelSlotPool;

// total size: 0x18
class eModel : public bTNode<eModel> {
    uint32 NameHash;                                           // offset 0x8, size 0x4
    eSolid *Solid;                                             // offset 0xC, size 0x4
    struct eReplacementTextureTable *pReplacementTextureTable; // offset 0x10, size 0x4
    int NumReplacementTextures;                                // offset 0x14, size 0x4

  public:
    void Init(uint32 name_hash);
    void UnInit();
    void ReconnectSolid(eSolidListHeader *solid_list_header);
    void ConnectSolid(eSolid *new_solid);
    void AttachReplacementTextureTable(eReplacementTextureTable *replacement_texture_table, int num_textures, int instance_index);
    void RestoreReplacementTextureTable(TextureInfo ***replaced_textures);
    void ApplyReplacementTextureTable(TextureInfo ***replaced_textures);
    int GetBoundingBox(bVector3 *min, bVector3 *max);
    bVector4 *GetPivotPosition();
    bMatrix4 *GetPivotMatrix();
    void ReplaceLightMaterial(uint32 old_name_hash, eLightMaterial *new_light_material);
    ePositionMarker *GetPostionMarker(ePositionMarker *prev_marker);
    ePositionMarker *GetPostionMarker(uint32 namehash);

    void *operator new(size_t size) {
        return bOMalloc(eModelSlotPool);
    }

    void operator delete(void *ptr) {
        bFree(eModelSlotPool, ptr);
    }

    eModel() {}

    eModel(uint32 name_hash) {
        NameHash = 0;
        Solid = nullptr;
        Init(name_hash);
    }

    ~eModel() {
        UnInit();
    }

    uint32 GetNameHash() {
        return NameHash;
    }

    eSolid *GetMovedSolid() {} // TODO

    eSolid *GetSolid() {
        return Solid;
    }

    int HasSolid() {
        return Solid != nullptr;
    }
};

// total size: 0x200
struct eDamageInfo {
    // Members
    float CurrentCells[64];  // offset 0x0, size 0x100
    float PreviousCells[64]; // offset 0x100, size 0x100
};

class eSolidPlatInterface {
    // total size: 0x4
    eSolidPlatInfo *PlatInfo; // offset 0x0, size 0x4

  public:
    int UnloaderPlatChunks(bChunk *chunk);
    int FixPlatInfo();
    int UnFixPlatInfo();

    eSolidPlatInfo *GetPlatInfo() {
        return this->PlatInfo;
    }

  protected:
    void SetSmoothVertex(uint32 vertex_offset, float nx, float ny, float nz);
    void ApplyDamagePlat(eDamageInfo *damage_info);
};

class eSolid : public eSolidPlatInterface, public bTNode<eSolid> {
  public:
    // total size: 0xE0
    uint8 Version;                              // offset 0xC, size 0x1
    uint8 EndianSwapped;                        // offset 0xD, size 0x1
    uint16 Flags;                               // offset 0xE, size 0x2
    uint32 NameHash;                            // offset 0x10, size 0x4
    int16 NumPolys;                             // offset 0x14, size 0x2
    int16 NumVerts;                             // offset 0x16, size 0x2
    int8 NumBones;                              // offset 0x18, size 0x1
    int8 NumTextureTableEntries;                // offset 0x19, size 0x1
    int8 NumLightMaterials;                     // offset 0x1A, size 0x1
    int8 NumPositionMarkerTableEntries;         // offset 0x1B, size 0x1
    int32 ReferencedFrameCounter;               // offset 0x1C, size 0x4
    float AABBMinX;                             // offset 0x20, size 0x4
    float AABBMinY;                             // offset 0x24, size 0x4
    float AABBMinZ;                             // offset 0x28, size 0x4
    eTextureEntry *pTextureTable;               // offset 0x2C, size 0x4
    float AABBMaxX;                             // offset 0x30, size 0x4
    float AABBMaxY;                             // offset 0x34, size 0x4
    float AABBMaxZ;                             // offset 0x38, size 0x4
    eLightMaterialEntry *LightMaterialTable;    // offset 0x3C, size 0x4
    bMatrix4 PivotMatrix;                       // offset 0x40, size 0x40
    ePositionMarker *PositionMarkerTable;       // offset 0x80, size 0x4
    eNormalSmoother *NormalSmoother;            // offset 0x84, size 0x4
    bTList<eModel> ModelList;                   // offset 0x88, size 0x8
    struct eDamageVertex *DamageVertexTable;    // offset 0x90, size 0x4
    struct eConnectivityData *ConnectivityData; // offset 0x94, size 0x4
    float Volume;                               // offset 0x98, size 0x4
    float Density;                              // offset 0x9C, size 0x4
    char Name[64];                              // offset 0xA0, size 0x40

    const char *GetName();
    void ChangeName(const char *new_name);
    void EndianSwap() {}
    void GetBoundingBox(bVector3 *min, bVector3 *max);
    void GetBoundingBox(bVector4 *min, bVector4 *max);
    void FixTextureTable();
    void FixLightMaterialTable();
    bool NotifyTextureLoading(TexturePack *texture_pack);
    bool NotifyTextureUnloading(TexturePack *texture_pack);
    void NotifyTextureMoving(TexturePack *texture_pack);
    void NotifyTextureMoving(TexturePack *texture_pack, TextureInfo *texture_info);
    void ReplaceLightMaterial(uint32 old_name_hash, uint32 new_name_hash);
    void ReplaceLightMaterial(uint32 old_name_hash, eLightMaterial *new_light_material);
    ePositionMarker *GetPostionMarker(ePositionMarker *prev_marker);
    ePositionMarker *GetPostionMarker(unsigned int namehash);
    int GetMemoryImageSize();
    void ApplyDamage(struct eDamageInfo *damage_info);
    void SmoothNormals(struct eSmoothVertex **smooth_vertex_table, int num_smooth_verts);
};

// total size: 0x68
class eView : public eViewPlatInterface {
  public:
    EVIEW_ID ID;                               // offset 0x4, size 0x4
    int8 Active;                               // offset 0x8, size 0x1
    int8 LetterBox;                            // offset 0x9, size 0x1
    int8 pad0;                                 // offset 0xA, size 0x1
    int8 pad1;                                 // offset 0xB, size 0x1
    float H;                                   // offset 0xC, size 0x4
    float NearZ;                               // offset 0x10, size 0x4
    float FarZ;                                // offset 0x14, size 0x4
    float FovBias;                             // offset 0x18, size 0x4
    float FovDegrees;                          // offset 0x1C, size 0x4
    int BlackAndWhiteMode;                     // offset 0x20, size 0x4
    int PixelMinSize;                          // offset 0x24, size 0x4
    bVector3 ViewDirection;                    // offset 0x28, size 0x10
    class Camera *pCamera;                     // offset 0x38, size 0x4
    bTList<class CameraMover> CameraMoverList; // offset 0x3C, size 0x8
    uint32 NumCopsInView;                      // offset 0x44, size 0x4
#ifndef EA_BUILD_A124
    uint32 NumCopsTotal;  // offset 0x48, size 0x4
    uint32 NumCopsCherry; // offset 0x4C, size 0x4
#endif
    TextureInfo *pBlendMask;                        // offset 0x50, size 0x4
    struct eDynamicLightContext *WorldLightContext; // offset 0x54, size 0x4
    eRenderTarget *RenderTargetTable[1];            // offset 0x58, size 0x4
    struct ScreenEffectDB *ScreenEffects;           // offset 0x5C, size 0x4
    struct Rain *Precipitation;                     // offset 0x60, size 0x4
    struct FacePixelation *facePixelation;          // offset 0x64, size 0x4

    eView();
    ~eView();

    eVisibleState GetVisibleState(const bVector3 *aabb_min, const bVector3 *aabb_max, bMatrix4 *local_world);
    eVisibleState GetVisibleState(eModel *model, bMatrix4 *local_world);
    int GetPixelSize(float radius, float distance);
    int GetPixelSize(const bVector3 *position, float radius);
    int GetPixelSize(const bVector3 *bbox_min, const bVector3 *bbox_max);
    void BiasMatrixForZSorting(bMatrix4 *pL2W, float zBias);
    void AttachCameraMover(CameraMover *camera_mover);
    void UnattachCameraMover(CameraMover *camera_mover);

    int32 GetID() {
        return this->ID;
    }

    void SetID(int32 id) {
        ID = static_cast<EVIEW_ID>(id);
    }

    void SetRenderTarget(eRenderTarget *target, int index) {
        this->RenderTargetTable[index] = target;
    }

    void SetRenderTarget0(eRenderTarget *target) {
        this->SetRenderTarget(target, 0);
    }

    Camera *GetCamera() {
        return this->pCamera;
    }

    void SetCamera(Camera *camera) {
        this->pCamera = camera;
    }

    int32 SetActive(int32 state) {
        int prev_state = this->Active;
        this->Active = state;

        return prev_state;
    }

    int32 IsActive() const {
        return Active;
    }

    int32 IsLetterBoxed();
    int32 SetLetterBox();

    CameraMover *GetCameraMover() {
        if (!this->CameraMoverList.IsEmpty()) {
            return this->CameraMoverList.GetHead();
        }
        return nullptr;
    }
};

extern eView eViews[22];

eView *eGetView(int view_id);

inline eView *eGetView(int view_id, bool doAssert) {
    // if (doAssert) {
    //     // ?
    // }
    return &eViews[view_id];
}

inline void eSwizzleWorldVector(const bVector3 &inVec, bVector3 &outVec) {
    bConvertFromBond(outVec, inVec);
}

inline void eUnSwizzleWorldVector(const bVector3 &inVec, bVector3 &outVec) {
    bConvertToBond(outVec, inVec);
}

eRenderTarget *eGetRenderTarget(int render_target);
void eUpdateViewMode(void);

struct ePoly {
    // total size: 0x94
    bVector3 Vertices[4];        // offset 0x0, size 0x40
    float UVs[2][4];             // offset 0x40, size 0x20
    float UVsMask[2][4];         // offset 0x60, size 0x20
    unsigned char Colours[4][4]; // offset 0x80, size 0x10
    unsigned char flags;         // offset 0x90, size 0x1
    unsigned char Flailer;       // offset 0x91, size 0x1

    ePoly();

    void *operator new(size_t size) {}

    void operator delete(void *ptr) {}

    void SetFlags(unsigned char i) {}

    void SetFlailer(unsigned char i) {}

    unsigned char GetFlags() {}

    unsigned char GetFlailer() {}
};

enum CurtainStatus {
    CT_OVERIDE = 3,
    CT_TURNON = 2,
    CT_ACTIVE = 1,
    CT_INACTIVE = 0,
};

struct FacePixelation {
    // total size: 0xC
    struct eView *MyView; // offset 0x0, size 0x4
    float mScreenX;       // offset 0x4, size 0x4
    float mScreenY;       // offset 0x8, size 0x4

    FacePixelation(eView *view);
    ~FacePixelation();
    static void SetLocation(bVector3 &worldPos);
    void GetData(float *x, float *y, float *width, float *height);
    void Render();

    bool IsEnabled() {}

    void Update(float x, float y) {}

    static void SetDimensions(float width, float height) {}

    static void Enable() {}

    static void Disable() {}
};

struct LoadedTable {
    // total size: 0x2004
    int NumLoaded;              // offset 0x0, size 0x4
    unsigned char Counts[8192]; // offset 0x4, size 0x2000

    LoadedTable() {}

    int IsLoaded(unsigned int hash) {
        return *this->GetPtr(hash) == 0;
    }

    void SetLoaded(unsigned int hash) {
        this->NumLoaded++;
        unsigned char *p = this->GetPtr(hash);
        *p = *p + 1;
    }

    void SetUnloaded(unsigned int hash) {
        this->NumLoaded--;
        unsigned char *p = this->GetPtr(hash);
        *p = *p - 1;
    }

    unsigned char *GetPtr(unsigned int hash) {
        return &this->Counts[hash & 0x1FFF];
    }
};

struct eReplacementTextureTable {
    // total size: 0xC
    unsigned int hOldNameHash; // offset 0x0, size 0x4
    unsigned int hNewNameHash; // offset 0x4, size 0x4
    TextureInfo *pTextureInfo; // offset 0x8, size 0x4

    eReplacementTextureTable()
        : hOldNameHash(0), //
          hNewNameHash(0), //
          pTextureInfo(reinterpret_cast<TextureInfo *>(-1)) {}

    void InvalidateTexture() {
        this->pTextureInfo = reinterpret_cast<TextureInfo *>(-1);
    }

    unsigned int GetNewNameHash() {
        return hNewNameHash;
    }

    unsigned int GetOldNameHash() {
        return hOldNameHash;
    }

    void SetOldNameHash(unsigned int name_hash) {
        hOldNameHash = name_hash;
    }

    void SetNewNameHash(unsigned int name_hash) {
        if (name_hash == this->hNewNameHash)
            return;

        hNewNameHash = name_hash;
        pTextureInfo = (TextureInfo *)-1;
    }

    void SetExplicit(unsigned int name_hash, TextureInfo *pRepTextureInfo) {}

    TextureInfo *GetCurrentTexture() {
        return pTextureInfo;
    }

    void SetCurrentTexture(TextureInfo *texture_info) {
        this->pTextureInfo = texture_info;
    }
};

void eInitModels();
void NotifySolidLoader(eSolidListHeader *solid_list_header);
void NotifySolidUnloader(eSolid *solid);

void eFixupReplacementTexturesAfterUnloading(TextureInfo *texture_info);
void eNotifyTextureLoading(TexturePack *texture_pack, TextureInfo *texture_info, bool loading);
TextureInfo *eGetRenderTargetTextureInfo(int name_hash);
TextureInfo *eGetOtherEcstacyTexture(unsigned int name_hash);
int eLoadSolidListPlatChunks(bChunk *chunk);
float GetSunIntensity(eView *view);
int eInitEngine();
void eFixUpTables();
bool eIsWidescreen();
int eLoadStreamingSolidPack(const char *filename, void (*callback_function)(void *), void *callback_param, int memory_pool_num);
void eWaitForStreamingSolidPackLoading(const char *filename);

void eInitSolids();
void eSolidNotifyTextureLoading(TexturePack *texture_pack, TextureInfo *texture_info, bool loading);
eSolid *eFindSolid(unsigned int name_hash, eSolidListHeader *solid_list_header);
int eSmoothNormals(eSolid **solid_table, int num_solids);

void SetDuplicateTextureWarning(BOOL enabled);

extern eLoadedSolidStats LoadedSolidStats;
extern unsigned int eFrameCounter;
extern int WaitUntilRenderingDoneDisabled;

inline unsigned int eGetFrameCounter() {
    return eFrameCounter;
}

inline int eLoadStreamingSolidPack(const char *filename) {
    return eLoadStreamingSolidPack(filename, nullptr, nullptr, 0);
}

inline void DisableWaitUntilRenderingDone() {
    WaitUntilRenderingDoneDisabled = 1;
}

inline void EnableWaitUntilRenderingDone() {
    WaitUntilRenderingDoneDisabled = 0;
}

extern unsigned int FrameMallocFailed;
extern unsigned int FrameMallocFailAmount;

static inline bMatrix4 *eFrameMallocMatrix(int num_matrices) {
    unsigned int size = num_matrices * sizeof(bMatrix4);
    unsigned char *address = CurrentBufferPos;
    if (address + size < CurrentBufferEnd) {
        CurrentBufferPos += size;
    } else {
        FrameMallocFailed = 1;
        FrameMallocFailAmount += size;
        return nullptr;
    }
    return reinterpret_cast<bMatrix4 *>(address);
}

extern bool WaitForFrameBufferSwapDisabled;

inline void DisableWaitForFrameBufferSwap() {
    WaitForFrameBufferSwapDisabled = true;
}

inline void EnableWaitForFrameBufferSwap() {
    WaitForFrameBufferSwapDisabled = false;
}

extern int AllowDuplicateSolids;

inline void eAllowDuplicateSolids(bool enable) {
    if (enable) {
        AllowDuplicateSolids++;
    } else {
        AllowDuplicateSolids--;
    }
}

#endif
