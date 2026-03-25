#include "./CarInfo.hpp"
#include "./CarLoader.hpp"
#include "./CarRender.hpp"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include <cstring>

static inline int CarLoader_GetMemoryPoolSize(CarLoader *car_loader) {
    return *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(car_loader) + 0x64);
}

struct CarPartAttribute;
struct CarPartModelTable;

struct CarPartPackListCtor {
    CarPartPackListCtor *Next;
    CarPartPackListCtor *Prev;

    CarPartPackListCtor()
        : Next(this), //
          Prev(this) {}
};

struct CarPartPackLayout {
    CarPartPackLayout *Next;
    CarPartPackLayout *Prev;
    unsigned int Version;
    const char *StringTable;
    unsigned int StringTableSize;
    short *AttributeTableTable;
    unsigned int NumAttributeTables;
    void *AttributesTable;
    unsigned int NumAttributes;
    unsigned int *TypeNameTable;
    unsigned int NumTypeNames;
    void *ModelTable;
    unsigned int NumModelTables;
    CarPart *PartsTable;
    int NumParts;
};

struct CarPartAttributeLayout {
    unsigned int NameHash;

    union {
        float fParam;
        int iParam;
        unsigned int uParam;
    } Params;
};

struct CarPartIndexCtor {
    CarPart *Part;
    int NumParts;

    CarPartIndexCtor()
        : Part(0), //
          NumParts(0) {}
};

struct CarPartDatabase {
    CarPartPackListCtor CarPartPackList;
    int NumPacks;
    int NumParts;
    int NumBytes;
    CarPartIndexCtor PaintPart_Gloss[3];
    CarPartIndexCtor PaintPart_Metallic[3];
    CarPartIndexCtor PaintPart_Pearl[3];
    CarPartIndexCtor PaintPart_Vinyl[3];
    CarPartIndexCtor PaintPart_Rims[3];
    CarPartIndexCtor PaintPart_Caliper[3];
    CarPartIndexCtor VinylPart_All[3];
    CarPartIndexCtor VinylPart_Body[3];
    CarPartIndexCtor VinylPart_Hood[3];
    CarPartIndexCtor VinylPart_Side[3];
    CarPartIndexCtor VinylPart_Manufacturer[3];

    CarPartDatabase();
    CarType GetCarType(unsigned int car_type_name_hash);
    int GetPartIndex(CarPart *car_part);
    CarPart *GetCarPartByIndex(int index);
    int NewGetNumCarParts(CarType car_type, int car_slot_id, unsigned int car_part_namehash, int upgrade_level);
    CarPart *NewGetCarPart(CarType car_type, int car_slot_id, unsigned int car_part_namehash, CarPart *prev_part, int upgrade_level);
    CarPart *NewGetFirstCarPart(CarType car_type, int car_slot_id, unsigned int car_part_namehash, int upgrade_level);
    CarPart *NewGetNextCarPart(CarPart *car_part, CarType car_type, int car_slot_id, unsigned int car_part_namehash, int upgrade_level);
};
struct MissingCarPart {
    short CarType;
    short Slot;
    unsigned int PartNameHash;
};
struct PresetCar : public bTNode<PresetCar> {
    char CarTypeName[0x20];
    char PresetName[0x38];
    unsigned int PartNameHashes[139];
};
struct CarSlotTypeOverride;
struct SlotTypeOverrideLayout {
    unsigned int CarType;
    unsigned int SlotId;
    unsigned int LookupType[2];
};
struct PresetCarListHead : public bTList<PresetCar> {};

CarTypeInfo *CarTypeInfoArray;
CarPartDatabase CarPartDB;
extern int iRam8047ff04;
extern MissingCarPart MissingCarPartTable[0x14A];
extern const char *CarPartStringTable;
extern unsigned int *CarPartTypeNameHashTable;
extern unsigned int CarPartTypeNameHashTableSize;
extern CAR_PART_ID CarPartSlotMap[];
extern CarPartPackLayout *MasterCarPartPackLayout asm("MasterCarPartPack");
extern unsigned int *DefaultSlotTypeNameTable;
extern CarSlotTypeOverride *SlotTypeOverrideTable;
extern int NumSlotTypeOverrides;
extern unsigned int TempSlotTable[2];
extern PresetCarListHead PresetCarList;

int UsedCarTextureAddToTable(unsigned int *table, int num_used, int max_textures, unsigned int texture_hash = 0);
int GetTempCarSkinTextures(unsigned int *table, int num_used, int max_textures, RideInfo *ride_info);
int GetIsCollectorsEdition();
void bMemCpy(void *dest, const void *src, unsigned int numbytes);
unsigned int *GetTypesFromSlot(CAR_SLOT_ID slot, CarType car_type);
unsigned char MapCarTypeNameHashToIndex(unsigned int car_type_namehash);
void *ScanHashTableKey8(unsigned char key_value, void *table_start, int table_length, int entry_key_offset, int entry_size);
unsigned int CarPartModelTable_GetModelNameHash(CarPartModelTable *model_table, unsigned int base_namehash, int model_num, int lod)
    asm("GetModelNameHash__17CarPartModelTableUiii");
CAR_PART_ID GetCarPartFromSlot(CAR_SLOT_ID slot);
CarPart *FindPartWithLevel(CarType type, CAR_SLOT_ID slot, int upg_level);
int GetNumCarSlotIDNames();
const char *GetCarTypeName(CarType car_type);
CarTypeInfo *GetCarTypeInfoFromHash(unsigned int car_type_hash);
PresetCar *FindFEPresetCar(unsigned int preset_name_hash);
const char *GetCarSlotNameFromID(int car_slot_id);

struct UsedCarTextureInfoLayout {
    unsigned int TexturesToLoadPerm[87];
    unsigned int TexturesToLoadTemp[87];
    int NumTexturesToLoadPerm;
    int NumTexturesToLoadTemp;
    unsigned int MappedSkinHash;
    unsigned int MappedSkinBHash;
    unsigned int MappedGlobalHash;
    unsigned int MappedWheelHash;
    unsigned int MappedSpinnerHash;
    unsigned int MappedBadging;
    unsigned int MappedSpoilerHash;
    unsigned int MappedRoofScoopHash;
    unsigned int MappedDashSkinHash;
    unsigned int MappedLightHash[11];
    unsigned int MappedTireHash;
    unsigned int MappedRimHash;
    unsigned int MappedRimBlurHash;
    unsigned int MappedLicensePlateHash;
    unsigned int ReplaceSkinHash;
    unsigned int ReplaceSkinBHash;
    unsigned int ReplaceGlobalHash;
    unsigned int ReplaceWheelHash;
    unsigned int ReplaceSpinnerHash;
    unsigned int ReplaceSpoilerHash;
    unsigned int ReplaceRoofScoopHash;
    unsigned int ReplaceDashSkinHash;
    unsigned int ReplaceHeadlightHash[3];
    unsigned int ReplaceHeadlightGlassHash[3];
    unsigned int ReplaceBrakelightHash[3];
    unsigned int ReplaceBrakelightGlassHash[3];
    unsigned int ReplaceReverselightHash[3];
    unsigned int ShadowHash;
};

CarPartDatabase::CarPartDatabase() {}

int UsedCarTextureAddToTable(unsigned int *table, int num_used, int max_textures, unsigned int texture_hash) {
    if (texture_hash == 0) {
        return 0;
    }

    for (int i = 0; i < num_used; i++) {
        if (table[i] == texture_hash) {
            return 0;
        }
    }

    if (num_used >= max_textures) {
        return num_used;
    }

    table[num_used] = texture_hash;
    return 1;
}

int GetNumCarPartIDNames() {
    return CARPARTID_NUM;
}

int GetNumCarSlotIDNames() {
    return CARSLOTID_NUM;
}

CarPartAttribute *CarPart::GetAttribute(unsigned int namehash, CarPartAttribute *prev_attribute) {
    unsigned short attribute_table_index = *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0xA);

    if (attribute_table_index == 0xFFFF) {
        return 0;
    }

    short *attribute_table = MasterCarPartPackLayout->AttributeTableTable + attribute_table_index;
    CarPartAttributeLayout *attributes = reinterpret_cast<CarPartAttributeLayout *>(MasterCarPartPackLayout->AttributesTable);
    int num_attributes = static_cast<int>(attribute_table[0]);
    int i = 0;

    if (prev_attribute != 0) {
        do {
            if (num_attributes <= i) {
                break;
            }

            i++;
        } while (prev_attribute != reinterpret_cast<CarPartAttribute *>(attributes + attribute_table[i]));

        if (i == num_attributes) {
            return 0;
        }
    }

    while (i < num_attributes) {
        CarPartAttributeLayout *attribute = attributes + attribute_table[i + 1];

        if (namehash == 0 || attribute->NameHash == namehash) {
            return reinterpret_cast<CarPartAttribute *>(attribute);
        }

        i++;
    }

    return 0;
}

CarPartAttribute *CarPart::GetFirstAppliedAttribute(unsigned int namehash) {
    return GetNextAppliedAttribute(namehash, 0);
}

CarPartAttribute *CarPart::GetNextAppliedAttribute(unsigned int namehash, CarPartAttribute *prev_attribute) {
    return GetAttribute(namehash, prev_attribute);
}

char *CarPart::GetName() {
    return const_cast<char *>(CarPartStringTable + *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 8) * 4);
}

unsigned int CarPart::GetCarTypeNameHash() {
    return CarPartTypeNameHashTable[*reinterpret_cast<unsigned char *>(reinterpret_cast<unsigned char *>(this) + 7)];
}

CAR_PART_ID GetCarPartFromSlot(CAR_SLOT_ID slot) {
    return CarPartSlotMap[slot];
}

unsigned int *GetTypesFromSlot(CAR_SLOT_ID slot, CarType car_type) {
    const char *car_type_name = GetCarTypeName(car_type);
    unsigned int car_type_namehash = bStringHash(car_type_name);

    {
        int i = 0;

        if (i < NumSlotTypeOverrides) {
            do {
                SlotTypeOverrideLayout *slot_type_override = &reinterpret_cast<SlotTypeOverrideLayout *>(SlotTypeOverrideTable)[i];

                if (slot_type_override->CarType == car_type_namehash && slot_type_override->SlotId == slot) {
                    return slot_type_override->LookupType;
                }
                i++;
            } while (i < NumSlotTypeOverrides);
        }
    }

    bMemCpy(&TempSlotTable, DefaultSlotTypeNameTable + slot * 2, 8);

    {
        int i = 0;

        do {
            if (TempSlotTable[i] == 0xFFFFFFFF) {
                TempSlotTable[i] = car_type_namehash;
            }
            i++;
        } while (i < 2);
    }

    return TempSlotTable;
}

unsigned char MapCarTypeNameHashToIndex(unsigned int namehash) {
    for (unsigned int i = 0; i < static_cast<unsigned int>(CarPartTypeNameHashTableSize); i++) {
        if (CarPartTypeNameHashTable[i] == namehash) {
            return static_cast<unsigned char>(i);
        }
    }

    return 0xFF;
}

unsigned int CarPart::GetModelNameHash(int model_num, int lod) {
    if (*reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0xC) == 0xFFFF) {
        return 0;
    }

    CarPartModelTable *model_table =
        reinterpret_cast<CarPartModelTable *>(
            reinterpret_cast<char *>(MasterCarPartPackLayout->ModelTable)
            + *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0xC) * 0x18);
    unsigned int base_namehash;

    if (*reinterpret_cast<char *>(reinterpret_cast<unsigned char *>(this) + 6) == 0) {
        base_namehash = 0xFFFFFFFF;
    } else if (*reinterpret_cast<char *>(reinterpret_cast<unsigned char *>(this) + 6) == 1) {
        base_namehash = GetCarTypeNameHash();
    } else {
        base_namehash = GetBrandNameHash();
    }

    return CarPartModelTable_GetModelNameHash(model_table, base_namehash, model_num, lod);
}

int CarPart::HasAppliedAttribute(unsigned int namehash) {
    return GetFirstAppliedAttribute(namehash) != 0;
}

const char *CarPart::GetAppliedAttributeString(unsigned int namehash, const char *default_string) {
    CarPartAttribute *attribute = GetFirstAppliedAttribute(namehash);
    return attribute != 0 ? CarPartStringTable + attribute->GetUParam() * 4 : default_string;
}

int CarPart::GetAppliedAttributeIParam(unsigned int namehash, int default_value) {
    CarPartAttributeLayout *attribute = reinterpret_cast<CarPartAttributeLayout *>(GetFirstAppliedAttribute(namehash));

    if (attribute != 0) {
        return attribute->Params.iParam;
    }

    return default_value;
}

unsigned int CarPart::GetAppliedAttributeUParam(unsigned int namehash, unsigned int default_value) {
    CarPartAttributeLayout *attribute = reinterpret_cast<CarPartAttributeLayout *>(GetFirstAppliedAttribute(namehash));

    if (attribute != 0) {
        return attribute->Params.uParam;
    }

    return default_value;
}

CarType CarPartDatabase::GetCarType(unsigned int car_type_name_hash) {
    int i = 0;

    do {
        CarTypeInfo *car_type_info = reinterpret_cast<CarTypeInfo *>(reinterpret_cast<unsigned char *>(CarTypeInfoArray) + i * sizeof(CarTypeInfo));

        if (bStringHash(car_type_info->BaseModelName) != car_type_name_hash) {
            i++;
        } else {
            return car_type_info->Type;
        }
    } while (i < 0x54);

    return static_cast<CarType>(0x54);
}

int CarPartDatabase::GetPartIndex(CarPart *car_part) {
    CarPartPackLayout *car_part_pack = reinterpret_cast<CarPartPackLayout *>(this->CarPartPackList.Next);
    int index = 0;

    while (true) {
        if (car_part_pack == reinterpret_cast<CarPartPackLayout *>(&this->CarPartPackList)) {
            return -1;
        }

        int part_index =
            static_cast<int>(reinterpret_cast<unsigned char *>(car_part) - reinterpret_cast<unsigned char *>(car_part_pack->PartsTable)) * -0x49249249 >>
            1;

        if (part_index > -1 && part_index < car_part_pack->NumParts) {
            return index + part_index;
        }

        index += car_part_pack->NumParts;
        car_part_pack = car_part_pack->Next;
    }
}

CarPart *CarPartDatabase::GetCarPartByIndex(int index) {
    if (index > -1) {
        CarPartPackLayout *car_part_pack = reinterpret_cast<CarPartPackLayout *>(this->CarPartPackList.Next);

        while (true) {
            if (car_part_pack != reinterpret_cast<CarPartPackLayout *>(&this->CarPartPackList)) {
                if (index < car_part_pack->NumParts) {
                    return reinterpret_cast<CarPart *>(reinterpret_cast<unsigned char *>(car_part_pack->PartsTable) + index * 0xE);
                }

                index -= car_part_pack->NumParts;
                car_part_pack = car_part_pack->Next;
            } else {
                break;
            }
        }
    }

    return 0;
}

const char *GetCarTypeName(CarType car_type) {
    const char *car_type_name = CarTypeInfoArray[car_type].CarTypeName;

    if (car_type_name != 0) {
        return car_type_name;
    }

    return 0;
}

CarTypeInfo *GetCarTypeInfoFromHash(unsigned int car_type_hash) {
    for (int i = 0; i < 0x54; i++) {
        CarTypeInfo *car_type_info = reinterpret_cast<CarTypeInfo *>(i * sizeof(CarTypeInfo) + reinterpret_cast<unsigned int>(CarTypeInfoArray));

        if (car_type_info->CarTypeNameHash == car_type_hash) {
            return car_type_info;
        }
    }

    return 0;
}

int CarInfo_GetMaxCompositingBufferSize() {
    return 0;
}

unsigned int CarInfo_GetResourcePool(bool needs_compositing) {
    if (needs_compositing != 0) {
        return CarLoader_GetMemoryPoolSize(&TheCarLoader) - CarInfo_GetMaxCompositingBufferSize();
    }

    return CarLoader_GetMemoryPoolSize(&TheCarLoader);
}

unsigned int RideInfo::GetSkinNameHash() {
    CarPart *skin_base;

    if (this->IsUsingCompositeSkin() == 0) {
        skin_base = this->GetPart(0x4C);

        if (skin_base == 0) {
            return 0;
        }

        return skin_base->GetTextureNameHash();
    }

    return this->mCompositeSkinHash;
}

void RideInfo::Init(CarType type, CarRenderUsage usage, int has_dash, int can_be_vertex_damaged) {
    this->Type = type;
    this->HasDash = has_dash;
    this->CanBeVertexDamaged = can_be_vertex_damaged;
    this->SkinType = 0;
    this->mSpecialLODBehavior = 0;
    this->InstanceIndex = 0;
    this->mCompositeSkinHash = 0;
    this->mMyCarLoaderHandle = 0;
    this->mMyCarRenderUsage = usage;

    if (usage < 2) {
        this->mMinLodLevel = CARPART_LOD_A;
        this->mMaxLodLevel = CARPART_LOD_A;
        this->mMinFELodLevel = CARPART_LOD_A;
        this->mMaxFELodLevel = CARPART_LOD_A;
    } else {
        this->mMinLodLevel = CARPART_LOD_B;
        this->mMaxLodLevel = CARPART_LOD_D;
        this->mMinFELodLevel = CARPART_LOD_A;
        this->mMaxFELodLevel = CARPART_LOD_D;
    }

    if (this->Type != CARTYPE_NONE && CarTypeInfoArray != 0) {
        CarTypeInfo *info = &CarTypeInfoArray[this->Type];

        if (info != 0 && info->UsageType == CAR_USAGE_TYPE_TRAFFIC) {
            this->mMinFELodLevel = CARPART_LOD_A;
            this->mMaxFELodLevel = CARPART_LOD_D;
            this->mMinLodLevel = CARPART_LOD_B;
            this->mMaxLodLevel = CARPART_LOD_D;
        }
    }

    if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
        this->mMaxLodLevel = CARPART_LOD_B;
        this->mMinLodLevel = CARPART_LOD_B;
    }

    this->mMaxBrakeLodLevel = CARPART_LOD_C;
    this->mMinReflectionLodLevel = CARPART_LOD_D;
    this->mMaxLicenseLodLevel = CARPART_LOD_C;
    this->mMinTrafficDiffuseLodLevel = CARPART_LOD_C;
    this->mMinShadowLodLevel = CARPART_LOD_B;
    this->mMaxShadowLodLevel = CARPART_LOD_C;
    this->mMaxTireLodLevel = CARPART_LOD_C;
    this->mMaxSpoilerLodLevel = CARPART_LOD_D;
    this->mMaxRoofScoopLodLevel = CARPART_LOD_D;

    if (usage == carRenderUsage_NISCar) {
        this->mSpecialLODBehavior = 2;
        this->mMaxTireLodLevel = CARPART_LOD_OFF;
        this->mMinLodLevel = CARPART_LOD_OFF;
        this->mMaxLodLevel = CARPART_LOD_OFF;
    }

    bMemSet(this->mPartsTable, 0, sizeof(this->mPartsTable));
    bMemSet(this->mPartsEnabled, 1, sizeof(this->mPartsEnabled));
    this->PreviewPart = 0;
}

int RideInfo::GetSpecialLODRangeForCarSlot(int slot_id, CARPART_LOD *special_minimum, CARPART_LOD *special_maximum, bool in_front_end) {
    if (slot_id == CARSLOTID_DRIVER || slot_id == CARSLOTID_INTERIOR) {
        if (this->mSpecialLODBehavior == 2) {
            *special_minimum = CARPART_LOD_OFF;
            *special_maximum = CARPART_LOD_OFF;
        } else {
            if (this->mSpecialLODBehavior == 1) {
                *special_minimum = CARPART_LOD_OFF;
            } else {
                *special_minimum = in_front_end ? CARPART_LOD_A : CARPART_LOD_B;
            }

            *special_maximum = slot_id == CARSLOTID_DRIVER ? CARPART_LOD_B : CARPART_LOD_D;
        }

        return 1;
    }

    if (slot_id > 0x47 && slot_id < 0x4C) {
        *special_minimum = CARPART_LOD_OFF;
        *special_maximum = CARPART_LOD_OFF;
        return 1;
    }

    return 0;
}

void RideInfo::SetCompositeNameHash(int skin_number) {
    char temp[64];
    unsigned int dummy_skin_hash;
    unsigned int dummy_wheel_hash;
    unsigned int dummy_spinner_hash;

    if (static_cast<unsigned int>(skin_number - 1) < 4) {
        this->SkinType = 1;
    } else if (static_cast<unsigned int>(skin_number - 5) < 8) {
        this->SkinType = 2;
    } else {
        this->SkinType = 0;
    }

    bSPrintf(temp, "DUMMY_SKIN%d", skin_number);
    dummy_skin_hash = bStringHash(temp);
    this->SetCompositeSkinNameHash(dummy_skin_hash);

    bSPrintf(temp, "DUMMY_WHEEL%d", skin_number);
    dummy_wheel_hash = bStringHash(temp);
    this->SetCompositeWheelNameHash(dummy_wheel_hash);

    bSPrintf(temp, "DUMMY_SPINNER%d", skin_number);
    dummy_spinner_hash = bStringHash(temp);
    this->SetCompositeSpinnerNameHash(dummy_spinner_hash);
}

unsigned int RideInfo::GetCompositeSkinNameHash() {
    return this->mCompositeSkinHash;
}

void RideInfo::SetCompositeSkinNameHash(unsigned int namehash) {
    CarTypeInfo *type_info = &CarTypeInfoArray[this->Type];

    if (type_info->Skinnable != 0) {
        this->mCompositeSkinHash = namehash;
    } else {
        this->mCompositeSkinHash = 0;
    }
}

unsigned int RideInfo::GetCompositeWheelNameHash() {
    return this->mCompositeWheelHash;
}

void RideInfo::SetCompositeWheelNameHash(unsigned int namehash) {
    this->mCompositeWheelHash = namehash;
}

unsigned int RideInfo::GetCompositeSpinnerNameHash() {
    return this->mCompositeSpinnerHash;
}

void RideInfo::SetCompositeSpinnerNameHash(unsigned int namehash) {
    this->mCompositeSpinnerHash = namehash;
}

int RideInfo::IsUsingCompositeSkin() {
    return this->GetCompositeSkinNameHash() != 0;
}

void RideInfo::SetUpgradePart(CAR_SLOT_ID car_slot_id, int upg_level) {
    CAR_PART_ID car_part_id = GetCarPartFromSlot(car_slot_id);
    CarPart *part;

    (void)car_part_id;
    part = FindPartWithLevel(this->Type, car_slot_id, upg_level);
    if (part != 0) {
        this->SetPart(car_slot_id, part, true);
    }
}

CarPart *FindPartWithLevel(CarType car_type, CAR_SLOT_ID slot_id, int level) {
    CarPart *part = CarPartDB.NewGetCarPart(car_type, slot_id, 0, 0, -1);

    while (part != 0) {
        if ((reinterpret_cast<unsigned char *>(part)[5] >> 5) == static_cast<unsigned int>(level)) {
            return part;
        }
        part = CarPartDB.NewGetNextCarPart(part, car_type, slot_id, 0, -1);
    }

    return 0;
}

void RideInfo::SetStockParts() {
    unsigned int stock_vinyl_colours[4];
    unsigned int stock_vinyl_hashes[4];

    for (int car_slot_id = 0; car_slot_id <= CARSLOTID_MISC; car_slot_id++) {
        if (((this->Type != static_cast<CarType>(4)) || (car_slot_id != CARSLOTID_ATTACHMENT6)) && car_slot_id != CARSLOTID_VINYL_LAYER0 &&
            (static_cast<unsigned int>(car_slot_id - CARSLOTID_DECAL_FRONT_WINDOW_TEX0) > 0x2F)) {
            if (car_slot_id == CARSLOTID_HUD_BACKING_COLOUR) {
                goto set_hud_backing_colour;
            } else if (car_slot_id == CARSLOTID_BASE_PAINT) {
                goto set_base_paint;
            } else if (car_slot_id == CARSLOTID_HUD_NEEDLE_COLOUR) {
                goto set_hud_needle_colour;
            } else if (car_slot_id == CARSLOTID_HUD_CHARACTER_COLOUR) {
                goto set_hud_character_colour;
            } else {
                goto set_upgrade_part;
            }
        }

        continue;

    set_base_paint: {
            CarTypeInfo *type_info = &CarTypeInfoArray[this->Type];
            CarPart *paint_part =
                CarPartDB.NewGetCarPart(this->Type, car_slot_id, static_cast<unsigned int>(type_info->DefaultBasePaint), 0, -1);

            if (paint_part != 0) {
                this->SetPart(car_slot_id, paint_part, true);
            }
            continue;
        }

    set_hud_backing_colour: {
            CarPart *hud_part = CarPartDB.NewGetCarPart(this->Type, car_slot_id, bStringHash("ORANGE"), 0, -1);

            if (hud_part != 0) {
                this->SetPart(car_slot_id, hud_part, true);
            } else {
                this->SetUpgradePart(static_cast<CAR_SLOT_ID>(car_slot_id), 0);
            }
            continue;
        }

    set_hud_needle_colour: {
            CarPart *hud_part = CarPartDB.NewGetCarPart(this->Type, car_slot_id, bStringHash("ORANGE"), 0, -1);

            if (hud_part != 0) {
                this->SetPart(car_slot_id, hud_part, true);
            } else {
                this->SetUpgradePart(static_cast<CAR_SLOT_ID>(car_slot_id), 0);
            }
            continue;
        }

    set_hud_character_colour: {
            CarPart *hud_part = CarPartDB.NewGetCarPart(this->Type, car_slot_id, bStringHash("WHITE"), 0, -1);

            if (hud_part != 0) {
                this->SetPart(car_slot_id, hud_part, true);
            } else {
                this->SetUpgradePart(static_cast<CAR_SLOT_ID>(car_slot_id), 0);
            }
            continue;
        }

    set_upgrade_part:
        this->SetUpgradePart(static_cast<CAR_SLOT_ID>(car_slot_id), 0);
    }

    stock_vinyl_hashes[0] = bStringHash("VINYL_L1_COLOR01");
    stock_vinyl_hashes[1] = bStringHash("VINYL_L1_COLOR03");
    stock_vinyl_hashes[2] = bStringHash("VINYL_L2_COLOR11");
    stock_vinyl_hashes[3] = bStringHash("VINYL_L1_COLOR01");
    stock_vinyl_colours[0] = stock_vinyl_hashes[0];
    stock_vinyl_colours[1] = stock_vinyl_hashes[1];
    stock_vinyl_colours[2] = stock_vinyl_hashes[2];
    stock_vinyl_colours[3] = stock_vinyl_hashes[3];

    for (int j = 0; j < 4; j++) {
        int car_slot_id = j + CARSLOTID_VINYL_COLOUR0_0;
        CarPart *vinyl_paint_part = CarPartDB.NewGetCarPart(this->Type, car_slot_id, stock_vinyl_colours[j], 0, -1);

        this->SetPart(car_slot_id, vinyl_paint_part, true);
    }
}

void RideInfo::SetRandomPart(CAR_SLOT_ID slot, int upgrade_level) {
    int num_parts = CarPartDB.NewGetNumCarParts(this->Type, slot, 0, upgrade_level);
    CarPart *part = 0;
    bool foundModel = false;

    for (int attempt = 1; (foundModel == false) && (attempt <= 0xF); attempt++) {
        int part_number = bRandom(num_parts);

        for (int i = 0; i < part_number + 1; i++) {
            part = CarPartDB.NewGetNextCarPart(part, this->Type, slot, 0, upgrade_level);
        }

        if (part != 0) {
            if (slot == CARSLOTID_VINYL_LAYER0) {
                if (part->GetGroupNumber() != 8) {
                    unsigned int brand_name_hash = part->GetBrandNameHash();
                    if (brand_name_hash != bStringHash("CEO") || GetIsCollectorsEdition() != 0) {
                        goto found_part;
                    }
                }
            } else {
            found_part:
                if (slot != CARSLOTID_BASE_PAINT || this->SkinType != 2 || part->GetBrandNameHash() != 0x03797533) {
                    bool part_missing = false;

                    for (int n = 0; n < 0x14A; n++) {
                        MissingCarPart *missing_part = &MissingCarPartTable[n];

                        if (missing_part->CarType == this->Type && missing_part->Slot == slot &&
                            missing_part->PartNameHash == part->GetPartNameHash()) {
                            part_missing = true;
                            break;
                        }
                    }

                    if (!part_missing) {
                        foundModel = true;
                        this->SetPart(slot, part, true);
                    }
                }
            }
        }
    }
}

void RideInfo::SetRandomParts() {
    int randomset;

    this->SetStockParts();
    randomset = bRandom(3);

    if (randomset == 1) {
        this->SetRandomPart(CARSLOTID_BASE_PAINT, -1);
        this->SetRandomPart(CARSLOTID_VINYL_LAYER0, -1);
        this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_0, -1);
        this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_1, -1);
        this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_2, -1);
        this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_3, -1);
    } else if (randomset > 1) {
        if (randomset == 2) {
            this->SetRandomPart(CARSLOTID_BODY, -1);
            this->SetRandomPart(CARSLOTID_SPOILER, -1);
            this->SetRandomPart(CARSLOTID_FRONT_WHEEL, -1);
            this->SetRandomPart(CARSLOTID_PAINT_RIM, -1);
            this->SetRandomPart(CARSLOTID_DECAL_FRONT_WINDOW_TEX0, -1);
            this->SetRandomPart(CARSLOTID_HEADLIGHT, -1);
            this->SetRandomPart(CARSLOTID_BRAKELIGHT, -1);
            this->SetRandomPart(CARSLOTID_BASE_PAINT, -1);
            this->SetRandomPart(CARSLOTID_VINYL_LAYER0, -1);
            this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_0, -1);
            this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_1, -1);
            this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_2, -1);
            this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_3, -1);
        }
    } else if (randomset == 0) {
        this->SetRandomPart(CARSLOTID_BODY, -1);
        this->SetRandomPart(CARSLOTID_SPOILER, -1);
        this->SetRandomPart(CARSLOTID_BASE_PAINT, -1);
        this->SetRandomPart(CARSLOTID_FRONT_WHEEL, -1);
        this->SetRandomPart(CARSLOTID_PAINT_RIM, -1);
    }
}

void RideInfo::SetRandomPaint() {
    int num_paints;
    CarPart *paint_part = 0;
    int paint_number;

    num_paints = CarPartDB.NewGetNumCarParts(this->Type, CARSLOTID_BASE_PAINT, 0, -1);
    paint_number = bRandom(num_paints) + 1;
    for (int i = 0; i < paint_number; i++) {
        paint_part = CarPartDB.NewGetNextCarPart(paint_part, this->Type, CARSLOTID_BASE_PAINT, 0, -1);
    }

    this->SetPart(CARSLOTID_BASE_PAINT, paint_part, true);
    for (int i = CARSLOTID_VINYL_LAYER0; i < CARSLOTID_PAINT_RIM; i++) {
        this->SetPart(i, 0, true);
    }
}

CarPart *RideInfo::GetPart(int car_slot_id) const {
    return this->mPartsTable[car_slot_id];
}

CarPart *RideInfo::SetPart(int car_slot_id, CarPart *car_part, bool update_enabled) {
    CarPart *previous_part;

    if (car_slot_id < 0x34) {
        if (car_slot_id > 0x2D) {
            return this->mPartsTable[car_slot_id];
        }

        if (car_slot_id == CARSLOTID_BODY) {
            if (car_part == 0) {
                this->mPartsTable[CARSLOTID_DAMAGE0_FRONT] = 0;
                this->mPartsTable[CARSLOTID_DAMAGE0_REAR] = 0;
                this->mPartsTable[CARSLOTID_DAMAGE0_FRONTLEFT] = 0;
                this->mPartsTable[CARSLOTID_DAMAGE0_FRONTRIGHT] = 0;
                this->mPartsTable[CARSLOTID_DAMAGE0_REARLEFT] = 0;
                this->mPartsTable[CARSLOTID_DAMAGE0_REARRIGHT] = 0;
                this->mPartsTable[CARSLOTID_DECAL_LEFT_DOOR] = 0;
                this->mPartsTable[CARSLOTID_DECAL_RIGHT_DOOR] = 0;
                this->mPartsTable[CARSLOTID_DECAL_LEFT_QUARTER] = 0;
                this->mPartsTable[CARSLOTID_DECAL_RIGHT_QUARTER] = 0;
            } else {
                int kit_number = car_part->GetAppliedAttributeIParam(0x796C0CB0, 0);
                char buffer[64];
                unsigned int base_hash;

                bSPrintf(buffer, "%s_KIT%02d_", GetCarTypeName(this->Type), kit_number);
                base_hash = bStringHash(buffer);
                this->mPartsTable[CARSLOTID_DAMAGE0_FRONT] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DAMAGE0_FRONT, bStringHash("DAMAGE0_FRONT", base_hash), 0, -1);
                this->mPartsTable[CARSLOTID_DAMAGE0_REAR] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DAMAGE0_REAR, bStringHash("DAMAGE0_REAR", base_hash), 0, -1);
                this->mPartsTable[CARSLOTID_DAMAGE0_FRONTLEFT] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DAMAGE0_FRONTLEFT, bStringHash("DAMAGE0_FRONTLEFT", base_hash), 0, -1);
                this->mPartsTable[CARSLOTID_DAMAGE0_FRONTRIGHT] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DAMAGE0_FRONTRIGHT, bStringHash("DAMAGE0_FRONTRIGHT", base_hash), 0, -1);
                this->mPartsTable[CARSLOTID_DAMAGE0_REARLEFT] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DAMAGE0_REARLEFT, bStringHash("DAMAGE0_REARLEFT", base_hash), 0, -1);
                this->mPartsTable[CARSLOTID_DAMAGE0_REARRIGHT] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DAMAGE0_REARRIGHT, bStringHash("DAMAGE0_REARRIGHT", base_hash), 0, -1);

                kit_number = car_part->GetAppliedAttributeIParam(0x796C0CB0, 0);
                bSPrintf(buffer, "%s_KIT%02d_", GetCarTypeName(this->Type), kit_number);
                base_hash = bStringHash(buffer);
                this->mPartsTable[CARSLOTID_DECAL_LEFT_DOOR] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DECAL_LEFT_DOOR, bStringHash("DECAL_LEFT_DOOR_RECT_MEDIUM", base_hash), 0,
                                  -1);
                this->mPartsTable[CARSLOTID_DECAL_RIGHT_DOOR] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DECAL_RIGHT_DOOR, bStringHash("DECAL_RIGHT_DOOR_RECT_MEDIUM", base_hash), 0,
                                  -1);
                this->mPartsTable[CARSLOTID_DECAL_LEFT_QUARTER] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DECAL_LEFT_QUARTER, bStringHash("DECAL_LEFT_QUARTER_RECT_MEDIUM", base_hash),
                                  0, -1);
                this->mPartsTable[CARSLOTID_DECAL_RIGHT_QUARTER] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DECAL_RIGHT_QUARTER,
                                  bStringHash("DECAL_RIGHT_QUARTER_RECT_MEDIUM", base_hash), 0, -1);
            }
        }
    } else if (car_slot_id == CARSLOTID_REAR_WHEEL || (car_slot_id > 0x47 && car_slot_id < 0x4C)) {
        return this->mPartsTable[car_slot_id];
    }

    previous_part = this->mPartsTable[car_slot_id];
    this->mPartsTable[car_slot_id] = car_part;

    if (update_enabled) {
        this->UpdatePartsEnabled();
    }

    return previous_part;
}

void RideInfo::UpdatePartsEnabled() {
    int num_car_slot_names;

    bMemSet(this->mPartsEnabled, 1, 0x8B);
    num_car_slot_names = GetNumCarSlotIDNames();
    for (int i = 0; i < num_car_slot_names; i++) {
        if (i == CARSLOTID_FRONT_WHEEL) {
            const unsigned int brake_paint_hash = bStringHash("CALIPER");
            bool is_part_brake_paint = false;
            CarPart *part = this->PreviewPart;

            if (part != 0) {
                if (part->GetGroupNumber() == 'L') {
                    is_part_brake_paint = part->GetBrandNameHash() == brake_paint_hash;
                }

                if (part->GetGroupNumber() == 'B' || is_part_brake_paint) {
                    this->mPartsEnabled[i] = 0;
                }
            }
        }
    }
}

int RideInfo::IsPartEnabled(int car_part_id) {
    return this->mPartsEnabled[car_part_id];
}

void RideInfo::DumpForPreset(FECarRecord *car) {
    (void)car;

    for (int i = 0; i < CARSLOTID_NUM; i++) {
        const char *slot_name = GetCarSlotNameFromID(i);
        CarPart *part = this->mPartsTable[i];

        (void)slot_name;
        if (part != 0) {
            const char *display_name = part->GetName();
            (void)display_name;
        }
    }
}

int LoaderFEPresetCars(bChunk *chunk) {
    if (chunk->GetID() == 0x30220) {
        int *chunk_words = reinterpret_cast<int *>(chunk);
        int *preset_words = chunk_words + 2;
        int num_presets = static_cast<unsigned int>(chunk_words[1]) / 0x290;
        int preset_count = num_presets - 1;

        if (num_presets != 0) {
            do {
                preset_count--;

                int j = 0;
                int k;
                do {
                    k = j + 1;
                    bEndianSwap32(preset_words + j + 0x18);
                    j = k;
                } while (k < 0x8B);

                bEndianSwap64(preset_words + 0x14);
                bEndianSwap64(preset_words + 0x12);
                bEndianSwap32(preset_words + 0x17);
                bEndianSwap32(preset_words + 0x16);

                PresetCar *preset = reinterpret_cast<PresetCar *>(preset_words);

                PresetCarList.AddTail(preset);
                preset_words += 0xA4;
            } while (preset_count != -1);
        }

        return 1;
    }

    return 0;
}

int UnloaderFEPresetCars(bChunk *chunk) {
    if (chunk->GetID() == 0x30220) {
        while (!PresetCarList.IsEmpty()) {
            PresetCarList.RemoveHead();
        }

        return 1;
    }

    return 0;
}

int GetNumPresetCars() {
    return PresetCarList.CountElements();
}

PresetCar *GetPresetCarAt(int index) {
    return PresetCarList.GetNode(index);
}

PresetCar *FindFEPresetCar(unsigned int hash) {
    for (PresetCar *p = PresetCarList.GetHead(); p != PresetCarList.EndOfList(); p = p->GetNext()) {
        if (hash == FEHashUpper(p->PresetName)) {
            return p;
        }
    }

    return 0;
}

void RideInfo::FillWithPreset(unsigned int preset_name_hash) {
    PresetCar *preset = FindFEPresetCar(preset_name_hash);

    if (preset != 0) {
        this->SkinType = 1;
        CarTypeInfo *cti = GetCarTypeInfoFromHash(FEHashUpper(preset->CarTypeName));
        CarType type = cti->Type;

        if (type != this->Type) {
            this->Init(type, CarRenderUsage_Player, 0, 0);
            this->SetStockParts();
        }

        for (int i = 0; i < CARSLOTID_NUM; i++) {
            unsigned int part_name_hash = preset->PartNameHashes[i];

            if (part_name_hash != 0) {
                if (part_name_hash != 1) {
                    CarPart *part = CarPartDB.NewGetCarPart(type, i, part_name_hash, 0, -1);
                    this->SetPart(i, part, true);
                }
            } else {
                this->SetPart(i, 0, true);
            }
        }
    }
}

void GetUsedCarTextureInfo(UsedCarTextureInfo *used_texture_info, RideInfo *ride_info, int front_end_only) {
    UsedCarTextureInfoLayout *info = reinterpret_cast<UsedCarTextureInfoLayout *>(used_texture_info);
    CarTypeInfo *car_type_info = &CarTypeInfoArray[ride_info->Type];
    char *car_base_name = car_type_info->BaseModelName;
    int max_perm_textures = 0x57;
    int max_temp_textures = 0x57;
    int num_perm_textures = 0;
    int num_temp_textures;
    int debug_print;
    char buffer[64];
    CarPart *wheel_part;
    unsigned int composite_skin_hash;
    unsigned int composite_wheel_hash;
    unsigned int composite_spinner_hash;
    unsigned int skin2_namehash;
    unsigned int skin3_namehash;
    unsigned int skin4_namehash;
    CarPart *carpart_headlight;
    CarPart *carpart_brakelight;
    unsigned int base_headlight_hash;
    unsigned int base_brakelight_hash;
    unsigned int misc_namehash;
    unsigned int misc_n_namehash;
    unsigned int interior_namehash;
    unsigned int interior_n_namehash;
    unsigned int badging_namehash;
    unsigned int badging_n_namehash;
    unsigned int driver_namehash;
    unsigned int engine_namehash;
    unsigned int badging_eu_namehash;
    unsigned int license_plate_namehash;
    unsigned int tire_n_namehash;
    unsigned int tread_namehash;
    unsigned int tread_n_namehash;
    unsigned int size_hash;
    unsigned int shape_hash;
    unsigned int size_hashes[3];
    unsigned int shape_hashes[3];

    bMemSet(info, 0, sizeof(*info));

    bSPrintf(buffer, "%s_SKIN1", car_base_name);
    info->MappedSkinHash = bStringHash(buffer);
    bSPrintf(buffer, "%s_SKIN1B", car_base_name);
    info->MappedSkinBHash = bStringHash(buffer);
    info->MappedGlobalHash = bStringHash("GLOBAL_SKIN1");
    wheel_part = ride_info->GetPart(0x42);

    if (wheel_part != 0) {
        info->MappedWheelHash = bStringHash("_WHEEL", wheel_part->GetAppliedAttributeUParam(0x10C98090, 0));
        composite_spinner_hash = wheel_part->GetAppliedAttributeUParam(bStringHash("SPINNER_TEXTURE"), 0);
        if (composite_spinner_hash != 0) {
            info->MappedSpinnerHash = composite_spinner_hash;
        } else {
            info->MappedSpinnerHash = 0;
        }
    } else {
        info->MappedSpinnerHash = 0;
        info->MappedWheelHash = 0;
    }

    info->MappedSpoilerHash = bStringHash("SPOILER_SKIN1");
    info->MappedRoofScoopHash = bStringHash("ROOF_SKIN");

    if (ride_info->IsUsingCompositeSkin() != 0) {
        info->ReplaceSkinHash = ride_info->GetSkinNameHash();
        info->ReplaceWheelHash = ride_info->GetCompositeWheelNameHash();
        info->ReplaceSpinnerHash = ride_info->GetCompositeSpinnerNameHash();
        info->ReplaceSpoilerHash = ride_info->GetSkinNameHash();
        info->ReplaceRoofScoopHash = ride_info->GetSkinNameHash();
    } else {
        unsigned int mapped_skin_hash = info->MappedSkinHash;

        info->ReplaceSkinHash = mapped_skin_hash;
        info->ReplaceSpoilerHash = mapped_skin_hash;
        info->ReplaceRoofScoopHash = mapped_skin_hash;
        info->ReplaceWheelHash = 0;
        info->ReplaceSpinnerHash = 0;
    }

    info->ReplaceSkinBHash = 0;
    info->ReplaceGlobalHash = info->ReplaceSkinHash;

    composite_skin_hash = ride_info->GetCompositeSkinNameHash();
    if (composite_skin_hash != 0) {
        num_perm_textures = UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), 0, max_perm_textures, composite_skin_hash);
    } else {
        if (info->ReplaceSkinHash != 0) {
            num_perm_textures =
                UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), 0, max_perm_textures, info->ReplaceSkinHash);
        }
        if (info->ReplaceSkinBHash != 0) {
            num_perm_textures += UsedCarTextureAddToTable(
                reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceSkinBHash);
        }
    }

    composite_wheel_hash = ride_info->GetCompositeWheelNameHash();
    if (composite_wheel_hash != 0) {
        num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, composite_wheel_hash);
    } else if (info->ReplaceWheelHash != 0) {
        num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceWheelHash);
    }

    composite_spinner_hash = ride_info->GetCompositeSpinnerNameHash();
    if (composite_spinner_hash != 0) {
        num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, composite_spinner_hash);
    } else if (info->ReplaceSpinnerHash != 0) {
        num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceSpinnerHash);
    }

    num_temp_textures = GetTempCarSkinTextures(info->TexturesToLoadTemp, 0, max_temp_textures, ride_info);

    bSPrintf(buffer, "%s_SKIN2", car_base_name);
    skin2_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_SKIN3", car_base_name);
    skin3_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_SKIN4", car_base_name);
    skin4_namehash = bStringHash(buffer);

    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, skin2_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, skin3_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, skin4_namehash);

    carpart_headlight = ride_info->GetPart(0x40);
    carpart_brakelight = ride_info->GetPart(0x41);

    bSPrintf(buffer, "%s_KIT00_HEADLIGHT", car_base_name);
    base_headlight_hash = bStringHash(buffer);
    bSPrintf(buffer, "%s_KIT00_BRAKELIGHT", car_base_name);
    base_brakelight_hash = bStringHash(buffer);

    if (carpart_headlight != 0 && carpart_headlight->GetAppliedAttributeUParam(0x10C98090, 0) != 0) {
        base_headlight_hash = carpart_headlight->GetAppliedAttributeUParam(0x10C98090, 0);
    }

    if (carpart_brakelight != 0 && carpart_brakelight->GetAppliedAttributeUParam(0x10C98090, 0) != 0) {
        base_brakelight_hash = carpart_brakelight->GetAppliedAttributeUParam(0x10C98090, 0);
    }

    info->MappedLightHash[0] = bStringHash("HEADLIGHT_LEFT");
    info->MappedLightHash[1] = bStringHash("HEADLIGHT_RIGHT");
    info->MappedLightHash[5] = bStringHash("HEADLIGHT_GLASS_LEFT");
    info->MappedLightHash[6] = bStringHash("HEADLIGHT_GLASS_RIGHT");
    info->MappedLightHash[2] = bStringHash("BRAKELIGHT_LEFT");
    info->MappedLightHash[3] = bStringHash("BRAKELIGHT_RIGHT");
    info->MappedLightHash[4] = bStringHash("BRAKELIGHT_CENTRE");
    info->MappedLightHash[7] = bStringHash("BRAKELIGHT_GLASS_LEFT");
    info->MappedLightHash[8] = bStringHash("BRAKELIGHT_GLASS_RIGHT");
    info->MappedLightHash[9] = bStringHash("BRAKELIGHT_GLASS_CENTRE");
    info->MappedLightHash[10] = 0;

    info->ReplaceHeadlightHash[0] = bStringHash("_OFF", base_headlight_hash);
    info->ReplaceHeadlightHash[1] = bStringHash("_ON", base_headlight_hash);
    info->ReplaceHeadlightHash[2] = bStringHash("_DAMAGE0", base_headlight_hash);
    info->ReplaceHeadlightGlassHash[0] = bStringHash("_GLASS_OFF", base_headlight_hash);
    info->ReplaceHeadlightGlassHash[1] = bStringHash("_GLASS_ON", base_headlight_hash);
    info->ReplaceHeadlightGlassHash[2] = bStringHash("_GLASS_DAMAGE0", base_headlight_hash);
    info->ReplaceBrakelightHash[0] = bStringHash("_OFF", base_brakelight_hash);
    info->ReplaceBrakelightHash[1] = bStringHash("_ON", base_brakelight_hash);
    info->ReplaceBrakelightHash[2] = bStringHash("_DAMAGE0", base_brakelight_hash);
    info->ReplaceBrakelightGlassHash[0] = bStringHash("_GLASS_OFF", base_brakelight_hash);
    info->ReplaceBrakelightGlassHash[1] = bStringHash("_GLASS_ON", base_brakelight_hash);
    info->ReplaceBrakelightGlassHash[2] = bStringHash("_GLASS_DAMAGE0", base_brakelight_hash);
    info->ReplaceReverselightHash[0] = 0;
    info->ReplaceReverselightHash[1] = 0;
    info->ReplaceReverselightHash[2] = 0;

    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceHeadlightHash[0]);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceHeadlightHash[1]);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceHeadlightGlassHash[0]);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceHeadlightGlassHash[1]);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceBrakelightHash[0]);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceBrakelightHash[1]);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceBrakelightGlassHash[0]);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceBrakelightGlassHash[1]);

    bSPrintf(buffer, "%s_MISC", car_base_name);
    misc_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_MISC_N", car_base_name);
    misc_n_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_INTERIOR", car_base_name);
    interior_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_INTERIOR_N", car_base_name);
    interior_n_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_BADGING", car_base_name);
    badging_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_BADGING_N", car_base_name);
    badging_n_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_DRIVER", car_base_name);
    driver_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_ENGINE", car_base_name);
    engine_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_BADGING_EU", car_base_name);
    badging_eu_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_LICENSE_PLATE", car_base_name);
    license_plate_namehash = bStringHash(buffer);
    info->MappedLicensePlateHash = license_plate_namehash;
    info->MappedBadging = badging_namehash;

    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, misc_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, misc_n_namehash);

    if (front_end_only != 0 || strstr(car_base_name, "COP") != 0) {
        num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, interior_namehash);
        num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, interior_n_namehash);
    }

    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, driver_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, engine_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, badging_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, badging_n_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, badging_eu_namehash);

    bSPrintf(buffer, "%s_SIDELIGHT", car_base_name);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, bStringHash(buffer));
    bSPrintf(buffer, "%s_DOOR_HANDLE", car_base_name);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, bStringHash(buffer));
    bSPrintf(buffer, "%s_LOGO", car_base_name);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, bStringHash(buffer));

    if (iRam8047ff04 != 6) {
        num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, bStringHash("AUDIO_SKIN"));
    }

    bSPrintf(buffer, "%s_SHADOW%s", car_base_name, front_end_only != 0 ? "FE" : "IG");
    info->ShadowHash = bStringHash(buffer);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ShadowHash);

    bSPrintf(buffer, "%s_NEON", car_base_name);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, bStringHash(buffer));

    bSPrintf(buffer, "%s_TIRE", car_base_name);
    info->MappedTireHash = bStringHash(buffer);
    bSPrintf(buffer, "%s_TIRE_N", car_base_name);
    tire_n_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_RIM", car_base_name);
    info->MappedRimHash = bStringHash(buffer);
    bSPrintf(buffer, "%s_RIM_BLUR", car_base_name);
    info->MappedRimBlurHash = bStringHash(buffer);
    bSPrintf(buffer, "%s_TREAD", car_base_name);
    tread_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_TREAD_N", car_base_name);
    tread_n_namehash = bStringHash(buffer);

    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->MappedTireHash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->MappedRimHash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->MappedRimBlurHash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, tire_n_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, tread_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, tread_n_namehash);

    size_hash = bStringHash("SIZE");
    shape_hash = bStringHash("SHAPE");
    size_hashes[0] = bStringHash("SQUARE");
    size_hashes[1] = bStringHash("RECT");
    size_hashes[2] = bStringHash("WIDE");
    shape_hashes[0] = size_hashes[0];
    shape_hashes[1] = size_hashes[1];
    shape_hashes[2] = size_hashes[2];

    for (int i = 0x46; i <= 0x4B; i++) {
        CarPart *decal_model_part = ride_info->GetPart(i);

        if (decal_model_part != 0 && decal_model_part->HasAppliedAttribute(size_hash) != 0 &&
            decal_model_part->HasAppliedAttribute(shape_hash) != 0) {
            unsigned int decal_size = decal_model_part->GetAppliedAttributeUParam(size_hash, 0);
            unsigned int decal_shape = decal_model_part->GetAppliedAttributeUParam(shape_hash, 0);
            int num_decal_slots = 8;
            int first_tex_part = i * num_decal_slots - 0x1DD;

            for (int j = 0; j < num_decal_slots; j++) {
                CarPart *decal_texture_part = ride_info->GetPart(first_tex_part + j);

                if (decal_texture_part != 0) {
                    unsigned int texture_hash = decal_texture_part->GetAppliedAttributeUParam(bStringHash("NAME"), 0);

                    if (decal_shape == shape_hashes[0]) {
                        texture_hash = bStringHash("_SQUARE", texture_hash);
                    } else if (decal_shape == shape_hashes[1]) {
                        texture_hash = bStringHash("_RECT", texture_hash);
                    } else if (decal_shape == shape_hashes[2]) {
                        texture_hash = bStringHash("_WIDE", texture_hash);
                    }

                    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures,
                                                                  texture_hash);
                }
            }

            (void)decal_size;
        }
    }

    info->NumTexturesToLoadPerm = num_perm_textures;
    info->NumTexturesToLoadTemp = num_temp_textures;
}

int CarPartDatabase::NewGetNumCarParts(CarType car_type, int car_slot_id, unsigned int car_part_namehash, int upgrade_level) {
    int num_parts = 0;
    CarPart *part = 0;

    while (true) {
        part = this->NewGetCarPart(car_type, car_slot_id, car_part_namehash, part, upgrade_level);
        if (part == 0) {
            break;
        }

        num_parts++;
    }

    return num_parts;
}

CarPart *CarPartDatabase::NewGetCarPart(CarType car_type, int car_slot_id, unsigned int car_part_namehash, CarPart *prev_part, int upgrade_level) {
    unsigned int *car_type_namehashes = GetTypesFromSlot(static_cast<CAR_SLOT_ID>(car_slot_id), car_type);
    int previous_type_index = 0;
    CAR_PART_ID car_part_id = GetCarPartFromSlot(static_cast<CAR_SLOT_ID>(car_slot_id));
    int car_type_index;
    unsigned int car_type_namehash;
    CarPartPackLayout *car_part_pack;
    int num_parts;
    CarPart *part;
    CarPart *end_part;
    unsigned char car_type_key;

    if (prev_part != 0) {
        while (previous_type_index < 2 && prev_part->GetCarTypeNameHash() != car_type_namehashes[previous_type_index]) {
            previous_type_index++;
        }

        if (previous_type_index == 2) {
            return 0;
        }
    }

    for (car_type_index = previous_type_index; car_type_index <= 1; car_type_index++) {
        car_type_namehash = car_type_namehashes[car_type_index];

        if (car_type_namehash != 0) {
            car_part_pack = reinterpret_cast<CarPartPackLayout *>(this->CarPartPackList.Next);
            while (car_part_pack != reinterpret_cast<CarPartPackLayout *>(&this->CarPartPackList)) {
                num_parts = car_part_pack->NumParts;
                part = car_part_pack->PartsTable;
                end_part = reinterpret_cast<CarPart *>(reinterpret_cast<unsigned char *>(part) + num_parts * 0xE);

                if (prev_part != 0 && car_type_index == previous_type_index) {
                    if (prev_part < part || prev_part >= end_part) {
                        car_part_pack = car_part_pack->Next;
                        continue;
                    }

                    part = reinterpret_cast<CarPart *>(reinterpret_cast<unsigned char *>(prev_part) + 0xE);
                } else {
                    car_type_key = MapCarTypeNameHashToIndex(car_type_namehash);
                    part = static_cast<CarPart *>(ScanHashTableKey8(car_type_key, part, num_parts, 7, 0xE));
                    if (part == 0) {
                        car_part_pack = car_part_pack->Next;
                        continue;
                    }
                }

                while (part < end_part) {
                    if (static_cast<int>(static_cast<signed char>(reinterpret_cast<unsigned char *>(part)[4])) == car_part_id &&
                        part->GetCarTypeNameHash() == car_type_namehash &&
                        (car_part_namehash == 0 ||
                         (static_cast<unsigned int>(reinterpret_cast<unsigned short *>(part)[1]) << 16 |
                          static_cast<unsigned int>(reinterpret_cast<unsigned short *>(part)[0])) == car_part_namehash)) {
                        if ((reinterpret_cast<unsigned char *>(part)[5] >> 5) == static_cast<unsigned int>(upgrade_level)) {
                            return part;
                        }

                        if (upgrade_level == -1) {
                            return part;
                        }
                    }

                    part = reinterpret_cast<CarPart *>(reinterpret_cast<unsigned char *>(part) + 0xE);
                }

                car_part_pack = car_part_pack->Next;
            }
        }
    }

    return 0;
}

CarPart *CarPartDatabase::NewGetFirstCarPart(CarType car_type, int car_slot_id, unsigned int car_part_namehash, int upgrade_level) {
    return this->NewGetCarPart(car_type, car_slot_id, car_part_namehash, 0, upgrade_level);
}

CarPart *CarPartDatabase::NewGetNextCarPart(CarPart *car_part, CarType car_type, int car_slot_id, unsigned int car_part_namehash,
                                            int upgrade_level) {
    return this->NewGetCarPart(car_type, car_slot_id, car_part_namehash, car_part, upgrade_level);
}

bool CarInfo_IsSkinned(CarType type) {
    CarTypeInfo *info = &CarTypeInfoArray[type];

    if (info != 0) {
        Attrib::Gen::ecar ecar(Attrib::StringToLowerCaseKey(info->GetCarTypeName()), 0, 0);

        return ecar.IsSkinned();
    }

    return false;
}

void GenerateMissingCarParts() {
    char stack_space[0xE8];
    (void)stack_space;
}
