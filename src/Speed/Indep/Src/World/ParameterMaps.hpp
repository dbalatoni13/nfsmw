#ifndef WORLD_PARAMETERMAPS_H
#define WORLD_PARAMETERMAPS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

struct ParameterMapLayerHeader {
    unsigned int NameHash;   // offset 0x0, size 0x4
    float QuadLeft;          // offset 0x4, size 0x4
    float QuadTop;           // offset 0x8, size 0x4
    float QuadRight;         // offset 0xC, size 0x4
    float QuadBottom;        // offset 0x10, size 0x4
    int NumberOfQuadNodes;   // offset 0x14, size 0x4
    int NumberOfParameterSets; // offset 0x18, size 0x4
    int SizeOfParameterSet;  // offset 0x1C, size 0x4
    int NumberOfFields;      // offset 0x20, size 0x4
};

union ParameterMapQuad8 {
    struct {
        unsigned char Child0;
        unsigned char Child1;
        unsigned char Child2;
        unsigned char Child3;
    } Children;
    struct {
        unsigned char IsParent;
        unsigned char Data;
    } Others;
};

union ParameterMapQuad16 {
    struct {
        unsigned short Child0;
        unsigned short Child1;
        unsigned short Child2;
        unsigned short Child3;
    } Children;
    struct {
        unsigned short IsParent;
        unsigned short Data;
    } Others;
};

class ParameterAccessor;

class ParameterMapLayer : public bTNode<ParameterMapLayer> {
  public:
    ParameterMapLayer();
    ~ParameterMapLayer();

    unsigned int GetNameHash() {
        return this->Header != 0 ? this->Header->NameHash : 0;
    }

    int GetSizeOfParameterSet() {
        return this->Header != 0 ? this->Header->SizeOfParameterSet : 0;
    }

    int GetNumberOfFields() {
        return this->Header != 0 ? this->Header->NumberOfFields : 0;
    }

    int GetFieldType(int field_index) {
        return this->FieldTypes[field_index];
    }

    int GetFieldOffset(int field_index) {
        return this->FieldOffsets[field_index];
    }

    void Load(bChunk **chunk);
    void Unload();
    void AddParameterAccessor(ParameterAccessor *accessor);
    void RemoveParameterAccessor(ParameterAccessor *accessor);
    void *GetParameterData(float x, float y);
    float GetDataFloat(int field_index, void *parameter_data);
    int GetDataInt(int field_index, void *parameter_data);

  private:
    int GetParameterSetIndexFromMapData(float x, float y);
    int GetParameterSetIndexFromQuadData8(float x, float y);
    int GetParameterSetIndexFromQuadData16(float x, float y);
    void *GetFieldPointer(int set_index, int field_index);

    ParameterMapLayerHeader *Header;         // offset 0x8, size 0x4
    int *FieldTypes;                         // offset 0xC, size 0x4
    int *FieldOffsets;                       // offset 0x10, size 0x4
    void *ParameterData;                     // offset 0x14, size 0x4
    ParameterMapQuad8 *QuadData8;            // offset 0x18, size 0x4
    ParameterMapQuad16 *QuadData16;          // offset 0x1C, size 0x4
    bTList<ParameterAccessor> ParameterAccessors; // offset 0x20, size 0x8

    friend class ParameterAccessor;
    friend class ParameterMapsManager;
};

class ParameterAccessor : public bTNode<ParameterAccessor> {
  public:
    ParameterAccessor(const char *layer_name = 0);
    virtual ~ParameterAccessor();

    int IsValid() {
        return this->CurrentParameterData != 0;
    }

    unsigned int GetAutoAttachLayerNamehash() {
        return this->AutoAttachLayerNamehash;
    }

    const char *GetDebugName() {
        return this->DebugName;
    }

    void SetLayer(ParameterMapLayer *layer);
    void ClearLayer();
    virtual void CaptureData(float x, float y);
    virtual void ClearData();
    virtual float GetDataFloat(int field_index);
    virtual int GetDataInt(int field_index);
    virtual void SetUpForNewLayer();

    ParameterMapLayer *Layer;         // offset 0x8, size 0x4
    unsigned int AutoAttachLayerNamehash; // offset 0xC, size 0x4
    const char *DebugName;            // offset 0x10, size 0x4
    void *CurrentParameterData;       // offset 0x14, size 0x4
};

class ParameterAccessorBlend : public ParameterAccessor {
  public:
    ParameterAccessorBlend(const char *layer_name = 0);
    ~ParameterAccessorBlend() override;

    virtual void CaptureData(float x, float y, float ratio);

    void ClearData() override;
    void SetUpForNewLayer() override;
    void CaptureData(float x, float y) override;

    void *LastData;  // offset 0x1C, size 0x4
    int HaveLastData; // offset 0x20, size 0x4
};

class ParameterAccessorBlendByDistance : public ParameterAccessorBlend {
  public:
    ParameterAccessorBlendByDistance(const char *layer_name = 0);
    ~ParameterAccessorBlendByDistance() override;

    void CaptureData(float x, float y, float full_blend_distance) override;
    void SetUpForNewLayer() override;
    void CaptureData(float x, float y) override;

    float last_x;          // offset 0x24, size 0x4
    float last_y;          // offset 0x28, size 0x4
    int HaveLastPosition;  // offset 0x2C, size 0x4
};

class ParameterMapsManager {
  public:
    ParameterMapsManager();
    ~ParameterMapsManager();

    void AddLayer(ParameterMapLayer *new_layer);
    void UnloadAllLayers();
    int GetDataForLayer(unsigned int layer_name_hash, ParameterAccessor *accessor, int warning_if_not_found);
    int GetDataForLayer(const char *layer_name, ParameterAccessor *accessor, int warning_if_not_found);

    bTList<ParameterMapLayer> ParameterMapLayers; // offset 0x0, size 0x8
};

ParameterMapsManager &GetParameterMapsManager();
bTList<ParameterAccessor> &GetAutoParameterAccessors();
int LoaderParameterMaps(bChunk *chunk);
int UnloaderParameterMaps(bChunk *chunk);
void DumpAutoParameterAccessorsList();

#endif
