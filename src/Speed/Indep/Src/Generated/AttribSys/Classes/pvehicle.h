#ifndef ATTRIBSYS_CLASSES_PVEHICLE_H
#define ATTRIBSYS_CLASSES_PVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Physics/Behaviors/Effects.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// TODO move
namespace Csis {

typedef enum Type_car_type {
    Type_car_type_McLaren = 1,
    Type_car_type_Porsche = 2,
    Type_car_type_FordGT = 4,
    Type_car_type_Viper = 8,
    Type_car_type_AstonMartin = 16,
    Type_car_type_Corvette = 32,
    Type_car_type_BMW = 64,
    Type_car_type_Mercedes = 128,
    Type_car_type_Audi = 256,
    Type_car_type_Mitsubishi = 512,
    Type_car_type_Mustang = 1024,
    Type_car_type_Lotus = 2048,
    Type_car_type_Subaru = 4096,
    Type_car_type_Camaro = 8192,
    Type_car_type_GTO = 16384,
    Type_car_type_Nissan = 32768,
    Type_car_type_Mazda = 65536,
    Type_car_type_Renault = 131072,
    Type_car_type_Lexus = 262144,
    Type_car_type_Mini = 524288,
    Type_car_type_Volkswagen = 1048576,
    Type_car_type_SUV = 2097152,
    Type_car_type_Pickup = 4194304,
    Type_car_type_Pagani = 8388608,
    Type_car_type_Lamborghini = 16777216,
    Type_car_type_Chrysler = 33554432,
    Type_car_type_Opel = 67108864,
    Type_car_type_Sedan = 134217728,
    Type_car_type_SportsCar = 268435456,
    Type_car_type_Supra = 536870912,
    Type_car_type_Cadillac = 1073741824
} Type_car_type;

}

namespace Attrib {
namespace Gen {

struct pvehicle : Instance {
    struct _LayoutStruct {
        UMath::Vector4 TENSOR_SCALE;    // offset 0x0, size 0x10
        Attrib::StringKey MODEL;        // offset 0x10, size 0x10
        char DefaultPresetRide[4];      // offset 0x20, size 0x4
        char CollectionName[4];         // offset 0x24, size 0x4
        int engine_upgrades;            // offset 0x28, size 0x4
        int transmission_upgrades;      // offset 0x2c, size 0x4
        int nos_upgrades;               // offset 0x30, size 0x4
        int brakes_upgrades;            // offset 0x34, size 0x4
        float MASS;                     // offset 0x38, size 0x4
        int tires_upgrades;             // offset 0x3c, size 0x4
        Csis::Type_car_type VerbalType; // offset 0x40, size 0x4
        int induction_upgrades;         // offset 0x44, size 0x4
        int chassis_upgrades;           // offset 0x48, size 0x4
        unsigned char HornType;         // offset 0x4c, size 0x1
        unsigned char TrafficEngType;   // offset 0x4d, size 0x1
    };

    void *operator new(size_t bytes) {
        return Attrib::Alloc(bytes, "pvehicle");
    }

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "pvehicle");
    }

    pvehicle(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    pvehicle(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    pvehicle(const Instance &src) : Instance(src) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    pvehicle(const pvehicle &src) : Instance(src) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~pvehicle() {}

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    static Key ClassKey() {
        return 0x4a97ec8f;
    }

    const RefSpec &transmission(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x07a7a3e5, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    unsigned int Num_transmission() const {
        return Get(0x07a7a3e5).GetLength();
    }

    // const UpgradeSpecs &TurboSND(unsigned int index) const {
    //     const UpgradeSpecs *resultptr = reinterpret_cast<const UpgradeSpecs *>(GetAttributePointer(0x0e9bfb66, index));
    //     if (!resultptr) {
    //         resultptr = reinterpret_cast<const UpgradeSpecs *>(DefaultDataArea(sizeof(UpgradeSpecs)));
    //     }
    //     return *resultptr;
    // }

    unsigned int Num_TurboSND() const {
        return Get(0x0e9bfb66).GetLength();
    }

    const Attrib::StringKey &CLASS() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x0ef6ddf2, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const Attrib::StringKey &BEHAVIOR_ORDER(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x104e9d16, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    unsigned int Num_BEHAVIOR_ORDER() const {
        return Get(0x104e9d16).GetLength();
    }

    const int &transmission_current() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x170d5554, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const RefSpec &junkman() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x171737e9, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const EffectLinkageRecord &OnHitObject(unsigned int index) const {
        const EffectLinkageRecord *resultptr = reinterpret_cast<const EffectLinkageRecord *>(GetAttributePointer(0x18915735, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EffectLinkageRecord *>(DefaultDataArea(sizeof(EffectLinkageRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_OnHitObject() const {
        return Get(0x18915735).GetLength();
    }

    const RefSpec &aivehicle() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x22515733, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const EffectLinkageRecord &OnBottomOut(unsigned int index) const {
        const EffectLinkageRecord *resultptr = reinterpret_cast<const EffectLinkageRecord *>(GetAttributePointer(0x31047ebc, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EffectLinkageRecord *>(DefaultDataArea(sizeof(EffectLinkageRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_OnBottomOut() const {
        return Get(0x31047ebc).GetLength();
    }

    const int &tires_current() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x34e73f83, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const RefSpec &brakes(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x36350867, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    unsigned int Num_brakes() const {
        return Get(0x36350867).GetLength();
    }

    const RefSpec &engineaudio(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x50eab0e6, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    unsigned int Num_engineaudio() const {
        return Get(0x50eab0e6).GetLength();
    }

    const Attrib::StringKey &BEHAVIOR_MECHANIC_SUSPENSION() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x511abd7b, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const int &brakes_current() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x552c3d22, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const Attrib::StringKey &BEHAVIOR_MECHANIC_EFFECTS() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x59c2beb1, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const Attrib::StringKey &EventSequencer() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x5aab860f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const RefSpec &chopperspecs() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x5d898ee7, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const bool &RandomOpponent() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x6df0abfe, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const EffectLinkageRecord &OnScrapeWorld(unsigned int index) const {
        const EffectLinkageRecord *resultptr = reinterpret_cast<const EffectLinkageRecord *>(GetAttributePointer(0x7100960c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EffectLinkageRecord *>(DefaultDataArea(sizeof(EffectLinkageRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_OnScrapeWorld() const {
        return Get(0x7100960c).GetLength();
    }

    const int &chassis_current() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x7845f9bc, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const RefSpec &rigidbodyspecs() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x7c90bb38, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    // const eDRIVE_BY_TYPE &WooshType() const {
    //     const eDRIVE_BY_TYPE *resultptr = reinterpret_cast<const eDRIVE_BY_TYPE *>(GetAttributePointer(0x7e744600, 0));
    //     if (!resultptr) {
    //         resultptr = reinterpret_cast<const eDRIVE_BY_TYPE *>(DefaultDataArea(sizeof(eDRIVE_BY_TYPE)));
    //     }
    //     return *resultptr;
    // }

    const int &induction_current() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x7f440672, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const Attrib::StringKey &BEHAVIOR_MECHANIC_RESET() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x8013456f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const RefSpec &frontend() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x85885722, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const Attrib::StringKey &BEHAVIOR_MECHANIC_DAMAGE() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x858ed6e3, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const UMath::Vector4 &TENSOR_SCALE() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->TENSOR_SCALE;
    }

    // const UpgradeSpecs &ShiftSND(unsigned int index) const {
    //     const UpgradeSpecs *resultptr = reinterpret_cast<const UpgradeSpecs *>(GetAttributePointer(0x8ae8bee0, index));
    //     if (!resultptr) {
    //         resultptr = reinterpret_cast<const UpgradeSpecs *>(DefaultDataArea(sizeof(UpgradeSpecs)));
    //     }
    //     return *resultptr;
    // }

    unsigned int Num_ShiftSND() const {
        return Get(0x8ae8bee0).GetLength();
    }

    const Attrib::StringKey &BEHAVIOR_MECHANIC_RIGIDBODY() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x8ba55001, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const int &nos_current() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x8f37beae, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const RefSpec &Trailer() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x9a5537fe, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const Attrib::StringKey &BEHAVIOR_MECHANIC_ENGINE() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0xa3e13328, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const float &HandlingRating(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xaacbe2e7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    unsigned int Num_HandlingRating() const {
        return Get(0xaacbe2e7).GetLength();
    }

    const RefSpec &chassis(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0xafa210f0, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    unsigned int Num_chassis() const {
        return Get(0xafa210f0).GetLength();
    }

    const int &engine_current() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xb12ccb69, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const RefSpec &nos(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0xb1669f64, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    unsigned int Num_nos() const {
        return Get(0xb1669f64).GetLength();
    }

    const Attrib::StringKey &BEHAVIOR_MECHANIC_DRAW() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0xb230ade1, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const RefSpec &tires(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0xbd38d1ca, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    unsigned int Num_tires() const {
        return Get(0xbd38d1ca).GetLength();
    }

    const RefSpec &damagespecs() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0xc1f0b434, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const Attrib::StringKey &BEHAVIOR_MECHANIC_INPUT() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0xc3fa0cc4, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const RefSpec &induction(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0xc92a0142, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    unsigned int Num_induction() const {
        return Get(0xc92a0142).GetLength();
    }

    const int &junkman_current() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xcdc136e8, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const RefSpec &OnTireBlow() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0xd42e792f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const EffectLinkageRecord &OnHitGround(unsigned int index) const {
        const EffectLinkageRecord *resultptr = reinterpret_cast<const EffectLinkageRecord *>(GetAttributePointer(0xd9c6cdfd, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EffectLinkageRecord *>(DefaultDataArea(sizeof(EffectLinkageRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_OnHitGround() const {
        return Get(0xd9c6cdfd).GetLength();
    }

    const EffectLinkageRecord &OnScrapeObject(unsigned int index) const {
        const EffectLinkageRecord *resultptr = reinterpret_cast<const EffectLinkageRecord *>(GetAttributePointer(0xdacb1c11, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EffectLinkageRecord *>(DefaultDataArea(sizeof(EffectLinkageRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_OnScrapeObject() const {
        return Get(0xdacb1c11).GetLength();
    }

    const EffectLinkageRecord &OnScrapeGround(unsigned int index) const {
        const EffectLinkageRecord *resultptr = reinterpret_cast<const EffectLinkageRecord *>(GetAttributePointer(0xdb823931, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EffectLinkageRecord *>(DefaultDataArea(sizeof(EffectLinkageRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_OnScrapeGround() const {
        return Get(0xdb823931).GetLength();
    }

    const EffectLinkageRecord &OnHitWorld(unsigned int index) const {
        const EffectLinkageRecord *resultptr = reinterpret_cast<const EffectLinkageRecord *>(GetAttributePointer(0xe3167336, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EffectLinkageRecord *>(DefaultDataArea(sizeof(EffectLinkageRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_OnHitWorld() const {
        return Get(0xe3167336).GetLength();
    }

    const bool &TruckSndFX() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xeacb7696, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const bool &PlayerUsable() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xf099b6ac, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const RefSpec &engine(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0xf1f5fbc7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    unsigned int Num_engine() const {
        return Get(0xf1f5fbc7).GetLength();
    }

    const Attrib::StringKey &BEHAVIOR_MECHANIC_AUDIO() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0xfb0b5be9, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const EffectLinkageRecord &OnBottomScrape(unsigned int index) const {
        const EffectLinkageRecord *resultptr = reinterpret_cast<const EffectLinkageRecord *>(GetAttributePointer(0xfc03ad6a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EffectLinkageRecord *>(DefaultDataArea(sizeof(EffectLinkageRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_OnBottomScrape() const {
        return Get(0xfc03ad6a).GetLength();
    }

    const Attrib::StringKey &MODEL() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MODEL;
    }

    const char *DefaultPresetRide() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->DefaultPresetRide;
    }

    const char *CollectionName() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->CollectionName;
    }

    const int &engine_upgrades() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->engine_upgrades;
    }

    const int &transmission_upgrades() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->transmission_upgrades;
    }

    const int &nos_upgrades() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->nos_upgrades;
    }

    const int &brakes_upgrades() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->brakes_upgrades;
    }

    const float &MASS() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MASS;
    }

    const int &tires_upgrades() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->tires_upgrades;
    }

    const Csis::Type_car_type &VerbalType() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->VerbalType;
    }

    const int &induction_upgrades() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->induction_upgrades;
    }

    const int &chassis_upgrades() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->chassis_upgrades;
    }

    const unsigned char &HornType() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->HornType;
    }

    const unsigned char &TrafficEngType() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->TrafficEngType;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
