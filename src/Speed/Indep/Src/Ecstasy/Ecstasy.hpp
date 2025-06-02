#pragma once

#if TARGET_GC
#include "Speed/GameCube/Src/Ecstasy/eSolidPlat.hpp"
#include "Speed/GameCube/Src/Ecstasy/eViewPlat.hpp"

#endif
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bVector.hpp"

struct eModel : public bTNode<eModel> {
  // total size: 0x18
  unsigned int NameHash;                                     // offset 0x8, size 0x4
  struct eSolid *Solid;                                      // offset 0xC, size 0x4
  struct eReplacementTextureTable *pReplacementTextureTable; // offset 0x10, size 0x4
  int NumReplacementTextures;                                // offset 0x14, size 0x4

  int GetBoundingBox(bVector3 *min, bVector3 *max);
};

struct eSolid : public eSolidPlatInterface, public bTNode<eSolid> {
  // total size: 0xE0
  unsigned char Version;                          // offset 0xC, size 0x1
  unsigned char EndianSwapped;                    // offset 0xD, size 0x1
  unsigned short Flags;                           // offset 0xE, size 0x2
  unsigned int NameHash;                          // offset 0x10, size 0x4
  short NumPolys;                                 // offset 0x14, size 0x2
  short NumVerts;                                 // offset 0x16, size 0x2
  char NumBones;                                  // offset 0x18, size 0x1
  char NumTextureTableEntries;                    // offset 0x19, size 0x1
  char NumLightMaterials;                         // offset 0x1A, size 0x1
  char NumPositionMarkerTableEntries;             // offset 0x1B, size 0x1
  int ReferencedFrameCounter;                     // offset 0x1C, size 0x4
  float AABBMinX;                                 // offset 0x20, size 0x4
  float AABBMinY;                                 // offset 0x24, size 0x4
  float AABBMinZ;                                 // offset 0x28, size 0x4
  struct eTextureEntry *pTextureTable;            // offset 0x2C, size 0x4
  float AABBMaxX;                                 // offset 0x30, size 0x4
  float AABBMaxY;                                 // offset 0x34, size 0x4
  float AABBMaxZ;                                 // offset 0x38, size 0x4
  struct eLightMaterialEntry *LightMaterialTable; // offset 0x3C, size 0x4
  bMatrix4 PivotMatrix;                           // offset 0x40, size 0x40
  struct ePositionMarker *PositionMarkerTable;    // offset 0x80, size 0x4
  struct eNormalSmoother *NormalSmoother;         // offset 0x84, size 0x4
  struct bTList<eModel> ModelList;                // offset 0x88, size 0x8
  struct eDamageVertex *DamageVertexTable;        // offset 0x90, size 0x4
  struct eConnectivityData *ConnectivityData;     // offset 0x94, size 0x4
  float Volume;                                   // offset 0x98, size 0x4
  float Density;                                  // offset 0x9C, size 0x4
  char Name[64];                                  // offset 0xA0, size 0x40
};

struct eView : public eViewPlatInterface {
  // total size: 0x54
  EVIEW_ID ID;      // offset 0x4, size 0x4
  char Active;      // offset 0x8, size 0x1
  char LetterBox;   // offset 0x9, size 0x1
  char pad0;        // offset 0xA, size 0x1
  char pad1;        // offset 0xB, size 0x1
  float H;          // offset 0xC, size 0x4
  float NearZ;      // offset 0x10, size 0x4
  float FarZ;       // offset 0x14, size 0x4
  float FovBias;    // offset 0x18, size 0x4
  float FovDegrees; // offset 0x1C, size 0x4
  // int BlackAndWhiteMode;               // offset 0x20, size 0x4
  int PixelMinSize;                    // offset 0x24, size 0x4
  bVector3 ViewDirection;              // offset 0x28, size 0x10
  Camera *pCamera;                     // offset 0x34, size 0x4
  bTList<CameraMover> CameraMoverList; // offset 0x38, size 0x8
  unsigned int NumCopsInView;          // offset 0x40, size 0x4
  // unsigned int NumCopsTotal;           // offset 0x44, size 0x4
  // unsigned int NumCopsCherry;                     // offset 0x48, size 0x4
  struct TextureInfo *pBlendMask;                 // offset 0x4C, size 0x4
  struct eDynamicLightContext *WorldLightContext; // offset 0x50, size 0x4
  struct eRenderTarget *RenderTargetTable[1];     // offset 0x4C, size 0x4
  unsigned int pad50;

  eView();
  ~eView();

  eVisibleState GetVisibleState(const bVector3 *aabb_min, const bVector3 *aabb_max, bMatrix4 *local_world);
  eVisibleState GetVisibleState(eModel *model, bMatrix4 *local_world);
  int GetPixelSize(float radius, float distance);
  int GetPixelSize(const bVector3 *position, float radius);
  int GetPixelSize(const bVector3 *bbox_min, const bVector3 *bbox_max);

  int GetID() { return this->ID; }

  void SetID(int id) { ID = static_cast<EVIEW_ID>(id); }

  void SetRenderTarget(eRenderTarget *target, int index) { this->RenderTargetTable[index] = target; }

  void SetRenderTarget0(eRenderTarget *target) { this->SetRenderTarget(target, 0); }

  Camera *GetCamera() { return this->pCamera; }

  void SetCamera(Camera *camera) { this->pCamera = camera; }

  void SetActive(int state) {
    int prev_state = this->Active;
    this->Active = state;
  }

  CameraMover *GetCameraMover() {
    if (this->CameraMoverList.IsEmpty()) {
      return nullptr;
    }
    return this->CameraMoverList.GetHead();
  }
};

eView *eGetView(int view_id);

inline eView *eGetView(int view_id, bool doAssert) {
  if (doAssert) {
    // ?
  }
  return eGetView(view_id);
}

eRenderTarget *eGetRenderTarget(int render_target);
