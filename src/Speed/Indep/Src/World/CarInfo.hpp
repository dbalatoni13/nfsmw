#ifndef __CARINFO_HPP
#define __CARINFO_HPP

#include "CarPartID.h"
#include "CarRender.hpp"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Replay.hpp"
#include "Speed/Indep/Src/Physics/CarBasics.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#define WHEEL_FL 0
#define WHEEL_FR 1
#define WHEEL_RR 2
#define WHEEL_RL 3
#define CURRENT_CARPART_PACK_VERSION 6
#define CARSLOTID_TO_MODEL_INDEX(id) (id - CARSLOTID_MODEL_FIRST)
#define CARSLOTID_TO_VINYL_LAYER_INDEX(id) (id - CARSLOTID_VINYL_LAYER_FIRST)
#define CARSLOTID_FROM_MODEL_INDEX(index) (index + CARSLOTID_MODEL_FIRST)
#define CARSLOTID_FROM_VINYL_LAYER_INDEX(index) (index + CARSLOTID_VINYL_LAYER_FIRST)
#define MAX_CUSTOM_PAINT_PARTS (2)
#define MAX_VINYL_COLORS 4
#define MAX_VINYL_LAYERS 20
#define CUSTOM_PAINT_NOT_WHITE_SATURATION (2.0 / 15.0f)
#define CUSTOM_PAINT_NOT_WHITE_VARIANCE (0.8f)
#define CUSTOM_PAINT_NOT_BLACK_VARIANCE (0.2f)
#define CUSTOM_PAINT_NOT_SILVER_SATURATION (1.0 / 15.0f)
#define CARTYPE_ENUM_ANY ((enum CarType)NUM_CARTYPES)
#define CARTYPENAME_ANY 0UL
#define CARPARTID_ANY -1L
#define CARPARTNAME_ANY 0UL
#define CARPARTLEVEL_ANY -1L
#define CARTYPE_NUM_LODS 6
#define CARTYPE_MAX_SKINS 10
#define CARTYPE_MAX_TIRES 4
#define CARTYPE_MAX_BRAKES 4
#define CARTYPE_NUM_CLASSES 4
#define CARTYPE_MAX_TEXTURES 24
#define CARTYPEINFO_GAME_ANY (0xFF)
#define CARTYPEINFO_GAME_MW (1 << 0)
#define CARTYPEINFO_GAME_UG2 (1 << 1)
#define MAX_TRAFFIC_USAGE_TYPES 5
#define CAR_CLASS_DONT_CARE -1
#define COP_DRIVER_NUMBER_START 10
#define TRAFFIC_DRIVER_NUMBER_START 50
#define PARKED_CAR_DRIVER_NUMBER_START 80
#define AI_DRIVER_NUMBER_START 100

enum CarUsageType {
    CAR_USAGE_TYPE_RACING = 0,
    CAR_USAGE_TYPE_COP = 1,
    CAR_USAGE_TYPE_TRAFFIC = 2,
    CAR_USAGE_TYPE_WHEELS = 3,
    CAR_USAGE_TYPE_UNIVERSAL = 4,
};

class CarPartIDNameMap {
  public:
    int CarPartID;    // offset 0x0, size 0x4
    const char *Name; // offset 0x4, size 0x4
    UCrc32 Crc;       // offset 0x8, size 0x4
};

class CarSlotIDNameMap {
  public:
    int CarSlotID;
    const char *Name;
};

const int TYPESLOT_NUM = 7;

// total size: 0x10
struct CarSlotTypeOverride {
    uint32 CarType;       // offset 0x0, size 0x4
    uint32 SlotId;        // offset 0x4, size 0x4
    uint32 LookupType[2]; // offset 0x8, size 0x8
};

class PresetCar : public bTNode<PresetCar> {
  public:
    char CarTypeName[32];                // offset 0x8, size 0x20
    char PresetName[32];                 // offset 0x28, size 0x20
    uint64_t FEKey;                      // offset 0x48, size 0x8
    uint64_t VehicleKey;                 // offset 0x50, size 0x8
    uint32 FilterBits;                   // offset 0x58, size 0x4
    int32 PhysicsLevel;                  // offset 0x5C, size 0x4
    int32 PartNameHashes[CARSLOTID_NUM]; // offset 0x60, size 0x22C
};

enum eCustomTuningType {
    CTT_SETTING_1 = 0,
    CTT_SETTING_2 = 1,
    CTT_SETTING_3 = 2,
    NUM_CUSTOM_TUNINGS = 3,
};

enum CARPART_LOD {
    CARPART_LOD_OFF = -1,
    CARPART_LOD_A = 0,
    CARPART_LOD_B = 1,
    CARPART_LOD_C = 2,
    CARPART_LOD_D = 3,
    CARPART_LOD_E = 4,
    CARPART_LOD_NUM = 5,
};

static const int CARPART_MODEL_NUM = 1;

static const int CARANIM_MAX_HIDE_SLOTS = 8;

enum CarAnimState {
    CARANIM_OPEN = 0,
    CARANIM_CLOSED = 1,
    CARANIM_OPENING = 2,
    CARANIM_CLOSING = 3,
};

enum CarAnimLocation {
    CARANIM_HOOD = 0,
    CARANIM_TRUNK = 1,
    CARANIM_LEFT_DOOR = 2,
    CARANIM_RIGHT_DOOR = 3,
    CARANIM_NUM = 4,
};

extern const char *CarPartStringTable;

class CarPartAttribute {
  public:
    char *GetName() {}

    uint32 GetNameHash() {
        return this->NameHash;
    };

    float GetFParam() {
        return this->Params.fParam;
    };

    int32 GetIParam() {
        return this->Params.iParam;
    };

    uint32 GetUParam() {
        return this->Params.uParam;
    };

    void EndianSwap() {
        bPlatEndianSwap(&this->Params.iParam);
        bPlatEndianSwap(&this->NameHash);
    }

    uint32 NameHash; // offset 0x0, size 0x4
    union {
        float fParam;  // offset 0x0, size 0x4
        int32 iParam;  // offset 0x0, size 0x4
        uint32 uParam; // offset 0x0, size 0x4
    } Params;
};

class CarPartAttributeTable {
  public:
    void EndianSwap() {
        bPlatEndianSwap(&this->NumAttributes);
        for (int i = 0; i < this->NumAttributes; i++) {
            bPlatEndianSwap(&this->AttributeOffsetTable[i]);
        }
    }

    uint32 GetByteSize() {
        return sizeof(*this->AttributeOffsetTable) * (this->NumAttributes) + sizeof(this->NumAttributes);
    }

    CarPartAttribute *GetAttribute() {}

    int16 NumAttributes;
    int16 AttributeOffsetTable[1];
};

// total size: 0x18
class CarPartModelTable {
  public:
    void EndianSwap() {
        bPlatEndianSwap(&this->MiddleStringOffset);

        for (int i = 0; i < 1; i++) {
            for (int j = 0; j < 5; j++) {
                // TODO 64 bit
                bPlatEndianSwap(reinterpret_cast<uint32 *>(&this->ModelNames[i][j]));
            }
        }
    }

    uint32 GetModelNameHash(uint32 base_namehash, int model_num, int lod);

    int8 TemplatedNameHashes;               // offset 0x0, size 0x1
    int8 pad;                               // offset 0x1, size 0x1
    uint16 MiddleStringOffset;              // offset 0x2, size 0x2
    char *ModelNames[CARPART_MODEL_NUM][5]; // offset 0x4, size 0x14
};

// total size: 0xE
class CarPart {
  public:
    CarPart() {}

    ~CarPart() {}

    void InPlaceInit() {}

    const char *GetName();
    CarPartAttribute *GetFirstAppliedAttribute(uint32 namehash);
    CarPartAttribute *GetLastAppliedAttribute(uint32 namehash);
    CarPartAttribute *GetNextAppliedAttribute(uint32 namehash, CarPartAttribute *prev_attribute);
    CarPartAttribute *GetPrevAppliedAttribute(uint32 namehash, CarPartAttribute *next_attribute);
    int HasAppliedAttribute(uint32 namehash);
    const char *GetAppliedAttributeString(uint32 namehash, const char *default_string);
    float GetAppliedAttributeFParam(uint32 namehash, float default_value);
    int32 GetAppliedAttributeIParam(uint32 namehash, int32 default_value);
    uint32 GetAppliedAttributeUParam(uint32 namehash, uint32 default_value);

    uint32 GetBrandNameHash() {
        return GetAppliedAttributeUParam(STRINGHASH_BRAND_NAME, 0);
    }

    uint32 GetTextureNameHash() {
        return GetAppliedAttributeUParam(STRINGHASH_TEXTURE_NAME, 0);
    }

    uint32 GetLightMaterialNameHash() {
        return GetAppliedAttributeUParam(STRINGHASH_LIGHT_MATERIAL_NAME, 0);
    }

    int8 GetInnerRadius() {
        return GetAppliedAttributeIParam(STRINGHASH_INNER_RADIUS, 0);
    }

    int8 GetOuterRadius() {
        return GetAppliedAttributeIParam(STRINGHASH_OUTER_RADIUS, 0);
    }

    int8 GetSpokeCount() {
        return static_cast<char>(bAbs(GetAppliedAttributeIParam(STRINGHASH_SPOKE_COUNT, 0)));
    }

    bool GetMirrored() {}

    uint32 GetCarTypeNameHash();

    uint32 GetPartNameHash() {
        return (static_cast<unsigned int>(this->PartNameHashTop) << 16) | this->PartNameHashBot;
    }

    int8 GetPartID() {
        return PartID;
    }

    int8 GetUpgradeLevel() {
        return (this->GroupNumber_UpgradeLevel >> 5) & 7;
    }

    int8 GetGroupNumber() {
        return this->GroupNumber_UpgradeLevel & 0x1F;
    }

    uint32 GetModelNameHash(int model_num, int lod);
    void Print();

    void EndianSwap() {
        bPlatEndianSwap(&this->PartNameHashBot);
        bPlatEndianSwap(&this->PartNameHashTop);
        bPlatEndianSwap(&this->NameOffset);
        bPlatEndianSwap(&this->AttributeTableOffset);
        bPlatEndianSwap(&this->ModelNameHashTableOffset);
    }

    uint16 PartNameHashBot;          // offset 0x0, size 0x2
    uint16 PartNameHashTop;          // offset 0x2, size 0x2
    int8 PartID;                     // offset 0x4, size 0x1
    uint8 GroupNumber_UpgradeLevel;  // offset 0x5, size 0x1
    int8 BaseModelNameHashSelector;  // offset 0x6, size 0x1
    uint8 CarTypeNameHashIndex;      // offset 0x7, size 0x1
    uint16 NameOffset;               // offset 0x8, size 0x2
    uint16 AttributeTableOffset;     // offset 0xA, size 0x2
    uint16 ModelNameHashTableOffset; // offset 0xC, size 0x2

  private:
    CarPartAttribute *GetAttribute(uint32 namehash, CarPartAttribute *prev_attribute);
};

class CarPartPack : public bTNode<CarPartPack> {
  public:
    CarPartPack() {}

    ~CarPartPack() {}

    void InPlaceInit() {
        Next = this;
        Prev = this;
    }

    void EndianSwap() {
        bPlatEndianSwap(&this->Version);
        bPlatEndianSwap(&this->NumParts);
        bPlatEndianSwap(&this->NumAttributes);
        bPlatEndianSwap(&this->NumTypeNames);
        bPlatEndianSwap(&this->NumModelTables);
        bPlatEndianSwap(&this->NumAttributeTables);
    }

    uint32 Version;                             // offset 0x8, size 0x4
    char *StringTable;                          // offset 0xC, size 0x4
    uint32 StringTableSize;                     // offset 0x10, size 0x4
    CarPartAttributeTable *AttributeTableTable; // offset 0x14, size 0x4
    uint32 NumAttributeTables;                  // offset 0x18, size 0x4
    CarPartAttribute *AttributesTable;          // offset 0x1C, size 0x4
    uint32 NumAttributes;                       // offset 0x20, size 0x4
    uint32 *TypeNameTable;                      // offset 0x24, size 0x4
    uint32 NumTypeNames;                        // offset 0x28, size 0x4
    CarPartModelTable *ModelTable;              // offset 0x2C, size 0x4
    uint32 NumModelTables;                      // offset 0x30, size 0x4
    CarPart *PartsTable;                        // offset 0x34, size 0x4
    uint32 NumParts;                            // offset 0x38, size 0x4
};

class CarPartIndex {
  public:
    CarPartIndex() {
        this->Part = nullptr;
        this->NumParts = 0;
    }

    CarPart *Part;
    int NumParts;
};

class CarPartDatabase {
  public:
    const char *GetCarPartIDName(int32 car_part_id);

    uint32 GetCarTypeNameHash(CarType car_type);

    CarType GetCarType(uint32 car_type_name_hash);

    int GetPartIndex(uint32 cartype_namehash, int32 car_part_id, uint32 car_part_namehash);

    int GetPartIndex(CarPart *car_part);

    CarPart *GetCarPartByIndex(int index);

    int NewGetNumCarParts(CarType car_type, int32 car_slot_id, uint32 car_part_namehash, int32 upgrade_level);

    CarPart *NewGetCarPart(CarType car_type, int32 car_slot_id, uint32 car_part_namehash, CarPart *prev_part, int32 upg_level);

    CarPart *NewGetFirstCarPart(CarType car_type, int32 car_slot_id, uint32 car_part_namehash, int32 upg_level);

    CarPart *NewGetLastCarPart(CarType car_type, int32 car_slot_id, uint32 car_part_namehash, int32 upg_level);

    CarPart *NewGetPrevCarPart(CarPart *car_part, CarType car_type, int32 car_slot_id, uint32 car_part_namehash, int32 upg_level);

    CarPart *NewGetNextCarPart(CarPart *car_part, CarType car_type, int32 car_slot_id, uint32 car_part_namehash, int32 upg_level);

    bTList<CarPartPack> CarPartPackList;    // offset 0x0, size 0x8
    int NumPacks;                           // offset 0x8, size 0x4
    int NumParts;                           // offset 0xC, size 0x4
    int NumBytes;                           // offset 0x10, size 0x4
    CarPartIndex PaintPart_Gloss[3];        // offset 0x14, size 0x18
    CarPartIndex PaintPart_Metallic[3];     // offset 0x2C, size 0x18
    CarPartIndex PaintPart_Pearl[3];        // offset 0x44, size 0x18
    CarPartIndex PaintPart_Vinyl[3];        // offset 0x5C, size 0x18
    CarPartIndex PaintPart_Rims[3];         // offset 0x74, size 0x18
    CarPartIndex PaintPart_Caliper[3];      // offset 0x8C, size 0x18
    CarPartIndex VinylPart_All[3];          // offset 0xA4, size 0x18
    CarPartIndex VinylPart_Body[3];         // offset 0xBC, size 0x18
    CarPartIndex VinylPart_Hood[3];         // offset 0xD4, size 0x18
    CarPartIndex VinylPart_Side[3];         // offset 0xEC, size 0x18
    CarPartIndex VinylPart_Manufacturer[3]; // offset 0x104, size 0x18
};

int32 GetCarPartIDFromCrc(UCrc32 crc);

// total size: 0xD0
class CarTypeInfo {
  public:
    char *GetName() {
        return this->CarTypeName;
    }

    char *GetCarTypeName() {
        return this->CarTypeName;
    };

    char *GetBaseModelName() {
        return this->BaseModelName;
    }

    CarType GetCarType() {
        return this->Type;
    }

    CarUsageType GetCarUsageType() {
        return this->UsageType;
    };

    bool HasArtwork() {
        return (this->GeometryFilename[0] & 0x20) == 0;
    }

    // int IsConvertable() {}

    bVector3 *GetHeadlightPosition() {
        return &this->HeadlightPosition;
    }

    float GetHeadlightFOV() {
        return this->HeadlightFOV;
    }

    // bool IsUG2Car() {}

    // bool IsMWCar() {}

    uint32 GetDefaultBasePaint() {
        return this->DefaultBasePaint;
    }

    int GetNumAvailableSkins();
    int GetAvailableSkin(int index);
    int GetRandomSkin(unsigned int *pseed);

    int GetDefaultSkinNumber() {
        return this->DefaultSkinNumber;
    }

    int GetSkinnable() {
        return this->Skinnable;
    }

    int GetWheelOuterRadius() {
        return this->WheelOuterRadius;
    }

    int GetWheelInnerRadiusMin() {
        return this->WheelInnerRadiusMin;
    }

    int GetWheelInnerRadiusMax() {
        return this->WheelInnerRadiusMax;
    }

    char CarTypeName[16];                       // offset 0x0, size 0x10
    char BaseModelName[16];                     // offset 0x10, size 0x10
    char GeometryFilename[32];                  // offset 0x20, size 0x20
    char ManufacturerName[16];                  // offset 0x40, size 0x10
    uint32 CarTypeNameHash;                     // offset 0x50, size 0x4
    float HeadlightFOV;                         // offset 0x54, size 0x4
    int8 padHighPerformance;                    // offset 0x58, size 0x1
    int8 NumAvailableSkinNumbers;               // offset 0x59, size 0x1
    int8 WhatGame;                              // offset 0x5A, size 0x1
    int8 ConvertableFlag;                       // offset 0x5B, size 0x1
    int8 WheelOuterRadius;                      // offset 0x5C, size 0x1
    int8 WheelInnerRadiusMin;                   // offset 0x5D, size 0x1
    int8 WheelInnerRadiusMax;                   // offset 0x5E, size 0x1
    int8 pad0;                                  // offset 0x5F, size 0x1
    bVector3 HeadlightPosition;                 // offset 0x60, size 0x10
    bVector3 DriverRenderingOffset;             // offset 0x70, size 0x10
    bVector3 InCarSteeringWheelRenderingOffset; // offset 0x80, size 0x10
    CarType Type;                               // offset 0x90, size 0x4
    CarUsageType UsageType;                     // offset 0x94, size 0x4
    uint32 CarMemTypeHash;                      // offset 0x98, size 0x4
    int8 MaxInstances[5];                       // offset 0x9C, size 0x5
    int8 WantToKeepLoaded[5];                   // offset 0xA1, size 0x5
    int8 pad4[2];                               // offset 0xA6, size 0x2
    float MinTimeBetweenUses[5];                // offset 0xA8, size 0x14
    char AvailableSkinNumbers[10];              // offset 0xBC, size 0xA
    int8 DefaultSkinNumber;                     // offset 0xC6, size 0x1
    int8 Skinnable;                             // offset 0xC7, size 0x1
    int32 Padding;                              // offset 0xC8, size 0x4
    int32 DefaultBasePaint;                     // offset 0xCC, size 0x4
};

extern CarTypeInfo *CarTypeInfoArray;

struct FECarRecord;

// total size: 0x310
class RideInfo {
  public:
    RideInfo() {
        Init(CARTYPE_NONE, CarRenderUsage_Player, 0, 0);
    }

    RideInfo(CarType type, int skin_number, int has_dash, int can_be_vertex_damaged) {}

    CARPART_LOD GetMinLodLevel() const {
        return this->mMinLodLevel;
    }

    CARPART_LOD GetMaxLodLevel() const {
        return this->mMaxLodLevel;
    }

    CARPART_LOD GetMinFELodLevel() const {
        return this->mMinFELodLevel;
    }

    CARPART_LOD GetMaxFELodLevel() const {
        return this->mMaxFELodLevel;
    }

    CARPART_LOD GetMaxLicenseLodLevel() const {
        return this->mMaxLicenseLodLevel;
    }

    CARPART_LOD GetMinTrafficDiffuseLodLevel() const {
        return this->mMinTrafficDiffuseLodLevel;
    }

    CARPART_LOD GetMinReflectionLodLevel() const {
        return this->mMinReflectionLodLevel;
    }

    CARPART_LOD GetMinShadowLodLevel() const {
        return this->mMinShadowLodLevel;
    }

    CARPART_LOD GetMaxShadowLodLevel() const {
        return this->mMaxShadowLodLevel;
    }

    CARPART_LOD GetMaxTireLodLevel() const {
        return this->mMaxTireLodLevel;
    }

    CARPART_LOD GetMaxBrakeLodLevel() const {
        return this->mMaxBrakeLodLevel;
    }

    CARPART_LOD GetMaxSpoilerLodLevel() const {
        return this->mMaxSpoilerLodLevel;
    }

    CARPART_LOD GetMaxRoofScoopLodLevel() const {
        return this->mMaxRoofScoopLodLevel;
    }

    void Init(CarType type, CarRenderUsage usage, int has_dash, int can_be_vertex_damaged);

    int GetSpecialLODRangeForCarSlot(int slot_id, CARPART_LOD *special_minimum, CARPART_LOD *special_maximum, bool in_front_end);

    void SetCarLoaderHandle(CarLoaderHandle car_loader_handle) {
        this->mMyCarLoaderHandle = car_loader_handle;
    }

    CarLoaderHandle GetCarLoaderHandle() {
        return this->mMyCarLoaderHandle;
    }

    CarRenderUsage GetCarRenderUsage() {
        return this->mMyCarRenderUsage;
    }

    void SetUpgradePart(CAR_SLOT_ID car_slot_id, int upg_level);

    void SetStockParts();

    void SetRandomPart(CAR_SLOT_ID slot, int upgrade_level);

    void SetRandomParts();

    void SetRandomPaint();

    CarPart *GetPart(int32 car_slot_id) const;

    CarPart *SetPart(int32 car_slot_id, CarPart *car_part, bool update_enabled);

    void UpdatePartsEnabled();

    int IsPartEnabled(int32 car_part_id);

    void DisablePart(int32 car_part_id) {}

    void EnablePart(int32 car_part_id) {}

    void SetPreviewPart(CarPart *part) {}

    CarPart *GetPreviewPart() {
        return this->PreviewPart;
    }

    uint8 NumbersInstalled();

    uint8 VinylsInstalled();

    float DecalsInstalledPercent();

    uint32 GetSkinNameHash();

    void SetCompositeNameHash(int skin_number);

    uint32 GetCompositeSkinNameHash();

    void SetCompositeSkinNameHash(uint32 namehash);

    uint32 GetCompositeWheelNameHash();

    void SetCompositeWheelNameHash(uint32 namehash);

    uint32 GetCompositeSpinnerNameHash();

    void SetCompositeSpinnerNameHash(uint32 namehash);

    int IsUsingCompositeSkin();

    uint32 GetCollisionVolumeNameHash();

    uint32 GetDefaultCollisionVolumeNameHash();

    void Print();

    void DumpForPreset(FECarRecord *car);

    void FillWithPreset(uint32 preset_name_hash);

    CarType Type;            // offset 0x0, size 0x4
    int8 InstanceIndex;      // offset 0x4, size 0x1
    int8 HasDash;            // offset 0x5, size 0x1
    int8 CanBeVertexDamaged; // offset 0x6, size 0x1
    int8 SkinType;           // offset 0x7, size 0x1

  private:
    CARPART_LOD mMinLodLevel;               // offset 0x8, size 0x4
    CARPART_LOD mMaxLodLevel;               // offset 0xC, size 0x4
    CARPART_LOD mMinFELodLevel;             // offset 0x10, size 0x4
    CARPART_LOD mMaxFELodLevel;             // offset 0x14, size 0x4
    CARPART_LOD mMaxLicenseLodLevel;        // offset 0x18, size 0x4
    CARPART_LOD mMinTrafficDiffuseLodLevel; // offset 0x1C, size 0x4
    CARPART_LOD mMinShadowLodLevel;         // offset 0x20, size 0x4
    CARPART_LOD mMaxShadowLodLevel;         // offset 0x24, size 0x4
    CARPART_LOD mMaxTireLodLevel;           // offset 0x28, size 0x4
    CARPART_LOD mMaxBrakeLodLevel;          // offset 0x2C, size 0x4
    CARPART_LOD mMaxSpoilerLodLevel;        // offset 0x30, size 0x4
    CARPART_LOD mMaxRoofScoopLodLevel;      // offset 0x34, size 0x4
    CARPART_LOD mMinReflectionLodLevel;     // offset 0x38, size 0x4
    uint32 mCompositeSkinHash;              // offset 0x3C, size 0x4
    uint32 mCompositeWheelHash;             // offset 0x40, size 0x4
    uint32 mCompositeSpinnerHash;           // offset 0x44, size 0x4
    CarPart *mPartsTable[CARSLOTID_NUM];    // offset 0x48, size 0x22C
    int8 mPartsEnabled[CARSLOTID_NUM];      // offset 0x274, size 0x8B
    CarPart *PreviewPart;                   // offset 0x300, size 0x4
    CarLoaderHandle mMyCarLoaderHandle;     // offset 0x304, size 0x4
    CarRenderUsage mMyCarRenderUsage;       // offset 0x308, size 0x4
    uint8 mSpecialLODBehavior;              // offset 0x30C, size 0x1
};

enum CarLightStateEnum {
    CAR_LIGHT_STATE_OFF = 0,
    CAR_LIGHT_STATE_ON = 1,
    CAR_LIGHT_STATE_DAMAGED = 2,
    NUM_CAR_LIGHT_STATES = 3,
};

enum CarLightTypeEnum {
    CAR_LIGHT_HEADLIGHT_LEFT = 0,
    CAR_LIGHT_HEADLIGHT_RIGHT = 1,
    CAR_LIGHT_BRAKELIGHT_LEFT = 2,
    CAR_LIGHT_BRAKELIGHT_RIGHT = 3,
    CAR_LIGHT_BRAKELIGHT_CENTRE = 4,
    CAR_LIGHT_HEADLIGHT_GLASS_LEFT = 5,
    CAR_LIGHT_HEADLIGHT_GLASS_RIGHT = 6,
    CAR_LIGHT_BRAKELIGHT_GLASS_LEFT = 7,
    CAR_LIGHT_BRAKELIGHT_GLASS_RIGHT = 8,
    CAR_LIGHT_BRAKELIGHT_GLASS_CENTRE = 9,
    CAR_LIGHT_REVERSELIGHT = 10,
    NUM_CAR_LIGHT_TYPES = 11,
};

struct UsedCarTextureInfo {
    uint32 TexturesToLoadPerm[87];        // offset 0x0, size 0x15C
    uint32 TexturesToLoadTemp[87];        // offset 0x15C, size 0x15C
    int NumTexturesToLoadPerm;            // offset 0x2B8, size 0x4
    int NumTexturesToLoadTemp;            // offset 0x2BC, size 0x4
    uint32 MappedSkinHash;                // offset 0x2C0, size 0x4
    uint32 MappedSkinBHash;               // offset 0x2C4, size 0x4
    uint32 MappedGlobalHash;              // offset 0x2C8, size 0x4
    uint32 MappedWheelHash;               // offset 0x2CC, size 0x4
    uint32 MappedSpinnerHash;             // offset 0x2D0, size 0x4
    uint32 MappedBadging;                 // offset 0x2D4, size 0x4
    uint32 MappedSpoilerHash;             // offset 0x2D8, size 0x4
    uint32 MappedRoofScoopHash;           // offset 0x2DC, size 0x4
    uint32 MappedDashSkinHash;            // offset 0x2E0, size 0x4
    uint32 MappedLightHash[11];           // offset 0x2E4, size 0x2C
    uint32 MappedTireHash;                // offset 0x310, size 0x4
    uint32 MappedRimHash;                 // offset 0x314, size 0x4
    uint32 MappedRimBlurHash;             // offset 0x318, size 0x4
    uint32 MappedLicensePlateHash;        // offset 0x31C, size 0x4
    uint32 ReplaceSkinHash;               // offset 0x320, size 0x4
    uint32 ReplaceSkinBHash;              // offset 0x324, size 0x4
    uint32 ReplaceGlobalHash;             // offset 0x328, size 0x4
    uint32 ReplaceWheelHash;              // offset 0x32C, size 0x4
    uint32 ReplaceSpinnerHash;            // offset 0x330, size 0x4
    uint32 ReplaceSpoilerHash;            // offset 0x334, size 0x4
    uint32 ReplaceRoofScoopHash;          // offset 0x338, size 0x4
    uint32 ReplaceDashSkinHash;           // offset 0x33C, size 0x4
    uint32 ReplaceHeadlightHash[3];       // offset 0x340, size 0xC
    uint32 ReplaceHeadlightGlassHash[3];  // offset 0x34C, size 0xC
    uint32 ReplaceBrakelightHash[3];      // offset 0x358, size 0xC
    uint32 ReplaceBrakelightGlassHash[3]; // offset 0x364, size 0xC
    uint32 ReplaceReverselightHash[3];    // offset 0x370, size 0xC
    uint32 ShadowHash;                    // offset 0x37C, size 0x4
};

enum DriverTypeEnum {
    DRIVER_TYPE_NONE,
    DRIVER_TYPE_PLAYER,
    DRIVER_TYPE_AI_RACER,
    DRIVER_TYPE_TRAFFIC_CAR,
    DRIVER_TYPE_PARKED_CAR,
    DRIVER_TYPE_COP,
    DRIVER_TYPE_SUPER_COP,
};

// total size: 0x34C
class DriverInfo {
  public:
    DriverInfo() {};
    DriverInfo(int unique_driver_number, DriverTypeEnum driver_type, RideInfo *ride_info, const char *character_name, int skill_level,
               int personality_type, int starting_position);
    void DoSnapshot(ReplaySnapshot *snapshot);
    const char *GetName() {}
    void SetRideInfo(RideInfo *new_ride_info) {}
    RideInfo *GetRideInfo() {}
    CarType GetCarType() {}
    bool IsPlayer() {}
    bool IsAI() {}
    bool IsTraffic() {}
    bool IsParkedCar() {}
    bool IsCop() {}
    bool IsSuperCop() {}
    bool IsOnlineDriver() {}
    int GetEliminatedRank() {}
    bool IsEliminated() {}
    bool StillRacing() {}
    void SetEliminatedRank(int nRank) {}
    void ClearEliminated() {}
    int8 GetSkillLevel();
    int8 GetPersonalityType() {}
    float GetReputationStars() {}
    void SetReputationStars(float s) {}

    int8 nEliminatedRank;      // offset 0x0, size 0x1
    int8 UniqueDriverNumber;   // offset 0x1, size 0x1
    int8 Pad;                  // offset 0x2, size 0x1
    int8 StartingPosition;     // offset 0x3, size 0x1
    DriverTypeEnum DriverType; // offset 0x4, size 0x4
    RideInfo CurrentRideInfo;  // offset 0x8, size 0x310
    int8 bOnlineDriver;        // offset 0x318, size 0x1
    int8 SkillLevel;           // offset 0x319, size 0x1
    int8 PersonalityType;      // offset 0x31A, size 0x1
    float ReputationStars;     // offset 0x31C, size 0x4
    CarType DesiredCarType;    // offset 0x320, size 0x4
    char DriverName[16];       // offset 0x324, size 0x10
    bool UseStartCoordinate;   // offset 0x334, size 0x1
    bVector3 StartCoordinate;  // offset 0x338, size 0x10
    bAngle StartAngle;         // offset 0x348, size 0x2
};

CarTypeInfo *GetCarTypeInfoFromHash(uint32 car_type_hash);
char *GetCarTypeName(CarType car_type);

extern CarPartDatabase CarPartDB;

inline CarTypeInfo *GetCarTypeInfo(CarType car_type) {
    return &CarTypeInfoArray[car_type];
}

bool CarInfo_IsSkinned(CarType type);
unsigned int CarInfo_GetResourceCost(CarType type, bool is_player, bool split_screen);
void GetUsedCarTextureInfo(UsedCarTextureInfo *info, RideInfo *ride_info, int front_end_only);
int UsedCarTextureAddToTable(uint32 *used_textures, int num_used_textures, int max_textures, uint32 texture_name_hash);

// TODO these are in CarPartNames, but parts don't have a separate header
int GetNumCarSlotIDNames();
const char *GetCarSlotNameFromID(int car_slot_id);

#endif
