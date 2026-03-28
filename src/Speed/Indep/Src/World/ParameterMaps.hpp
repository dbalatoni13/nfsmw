#ifndef WORLD_PARAMETERMAPS_H
#define WORLD_PARAMETERMAPS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"

// total size: 0x24
struct ParameterMapLayerHeader {
    uint32 NameHash;           // offset 0x0, size 0x4
    float QuadLeft;            // offset 0x4, size 0x4
    float QuadTop;             // offset 0x8, size 0x4
    float QuadRight;           // offset 0xC, size 0x4
    float QuadBottom;          // offset 0x10, size 0x4
    int NumberOfQuadNodes;     // offset 0x14, size 0x4
    int NumberOfParameterSets; // offset 0x18, size 0x4
    int SizeOfParameterSet;    // offset 0x1C, size 0x4
    int NumberOfFields;        // offset 0x20, size 0x4
};

class ParameterMapLayer;

class ParameterAccessor : public bTNode<ParameterAccessor> {
  public:
    ParameterAccessor();
    ParameterAccessor(const char *layer_name);
    virtual ~ParameterAccessor();

    virtual void CaptureData(float x, float y);
    virtual void ClearData();
    virtual float GetDataFloat(int field_index);
    virtual int GetDataInt(int field_index);

  protected:
    virtual void SetUpForNewLayer();

    ParameterMapLayer *Layer;             // offset 0x8, size 0x4
    unsigned int AutoAttachLayerNamehash; // offset 0xC, size 0x4
    const char *DebugName;                // offset 0x10, size 0x4
    void *CurrentParameterData;           // offset 0x14, size 0x4
};

// total size: 0x4
class ParameterMapQuad8 {
  public:
    // uint8 IsParent() {}

    // uint8 IsLeave() {}

    // uint8 GetChild0() {}

    // uint8 GetChild1() {}

    // uint8 GetChild2() {}

    // uint8 GetChild3() {}

    // uint8 GetData() {}

    // void SetChild0(uint8 child) {}

    // void SetChild1(uint8 child) {}

    // void SetChild2(uint8 child) {}

    // void SetChild3(uint8 child) {}

    // void SetData(uint8 data) {}

  private:
    union {
        // total size: 0x4
        struct {
            uint8 Child0; // offset 0x0, size 0x1
            uint8 Child1; // offset 0x1, size 0x1
            uint8 Child2; // offset 0x2, size 0x1
            uint8 Child3; // offset 0x3, size 0x1
        } Children;       // offset 0x0, size 0x4
        // total size: 0x2
        struct {
            uint8 IsParent; // offset 0x0, size 0x1
            uint8 Data;     // offset 0x1, size 0x1
        } Others;           // offset 0x0, size 0x2
    }; // offset 0x0, size 0x4
};

// total size: 0x8
class ParameterMapQuad16 {
  public:
    // uint16 IsParent() {}

    // uint16 IsLeave() {}

    // uint16 GetChild0() {}

    // uint16 GetChild1() {}

    // uint16 GetChild2() {}

    // uint16 GetChild3() {}

    // uint16 GetData() {}

    // void SetChild0(uint16 child) {}

    // void SetChild1(uint16 child) {}

    // void SetChild2(uint16 child) {}

    // void SetChild3(uint16 child) {}

    // void SetData(uint16 data) {}

    // void DoEndianSwap() {}

  private:
    union {
        // total size: 0x8
        struct {
            // Members
            uint16 Child0; // offset 0x0, size 0x2
            uint16 Child1; // offset 0x2, size 0x2
            uint16 Child2; // offset 0x4, size 0x2
            uint16 Child3; // offset 0x6, size 0x2
        } Children;        // offset 0x0, size 0x8
        // total size: 0x4
        struct {
            // Members
            uint16 IsParent; // offset 0x0, size 0x2
            uint16 Data;     // offset 0x2, size 0x2
        } Others;            // offset 0x0, size 0x4
    }; // offset 0x0, size 0x8
};

// total size: 0x28
class ParameterMapLayer : public bTNode<ParameterMapLayer> {
  public:
    ParameterMapLayer();

    ~ParameterMapLayer();

    void Load(bChunk **chunk);

    void Unload();

    void AddParameterAccessor(ParameterAccessor *accessor);

    void RemoveParameterAccessor(ParameterAccessor *accessor);

    void *GetParameterData(float x, float y);

    float GetDataFloat(int field_index, void *parameter_data);

    int GetDataInt(int field_index, void *parameter_data);

    // unsigned int GetNameHash() {}

    // int GetSizeOfParameterSet() {}

    // int GetNumberOfFields() {}

    // int GetFieldType(int field_index) {}

    // int GetFieldOffset(int field_index) {}

  private:
    int GetParameterSetIndexFromMapData(float x, float y);

    int GetParameterSetIndexFromQuadData8(float x, float y);

    int GetParameterSetIndexFromQuadData16(float x, float y);

    void *GetFieldPointer(int set_index, int field_index);

    ParameterMapLayerHeader *Header;              // offset 0x8, size 0x4
    int *FieldTypes;                              // offset 0xC, size 0x4
    int *FieldOffsets;                            // offset 0x10, size 0x4
    void *ParameterData;                          // offset 0x14, size 0x4
    ParameterMapQuad8 *QuadData8;                 // offset 0x18, size 0x4
    ParameterMapQuad16 *QuadData16;               // offset 0x1C, size 0x4
    bTList<ParameterAccessor> ParameterAccessors; // offset 0x20, size 0x8
};

class ParameterAccessorBlend : public ParameterAccessor {
  public:
    ParameterAccessorBlend();
    ParameterAccessorBlend(const char *layer_name);
    virtual ~ParameterAccessorBlend();

    virtual void CaptureData(float x, float y, float ratio);

    // Overrides: ParameterAccessor
    void ClearData() override;

  protected:
    // Overrides: ParameterAccessor
    void SetUpForNewLayer() override;

    void *LastData;   // offset 0x1C, size 0x4
    int HaveLastData; // offset 0x20, size 0x4

  private:
    virtual void CaptureData(float x, float y);
};

class ParameterAccessorBlendByDistance : public ParameterAccessorBlend {
  public:
    ParameterAccessorBlendByDistance();
    ParameterAccessorBlendByDistance(const char *layer_name);

    // Overrides: ParameterAccessor
    ~ParameterAccessorBlendByDistance() override;

    // Overrides: ParameterAccessor
    void CaptureData(float x, float y, float full_blend_distance) override;

  protected:
    // Overrides: ParameterAccessor
    void SetUpForNewLayer() override;

    float last_x;         // offset 0x24, size 0x4
    float last_y;         // offset 0x28, size 0x4
    int HaveLastPosition; // offset 0x2C, size 0x4

  private:
    // Overrides: ParameterAccessor
    void CaptureData(float x, float y) override;
};

extern ParameterAccessorBlendByDistance TintSunRiseAccessor[2];
extern ParameterAccessorBlendByDistance TintMiddayAccessor[2];

#endif
