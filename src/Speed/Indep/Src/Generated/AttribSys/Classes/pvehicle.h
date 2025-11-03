#ifndef ATTRIBSYS_CLASSES_PVEHICLE_H
#define ATTRIBSYS_CLASSES_PVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
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

    const UMath::Vector4 &TENSOR_SCALE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->TENSOR_SCALE;
    }

    const Attrib::StringKey &MODEL() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MODEL;
    }

    const char *DefaultPresetRide() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DefaultPresetRide;
    }

    const char *CollectionName() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CollectionName;
    }

    const int &engine_upgrades() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->engine_upgrades;
    }

    const int &transmission_upgrades() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->transmission_upgrades;
    }

    const int &nos_upgrades() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->nos_upgrades;
    }

    const int &brakes_upgrades() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->brakes_upgrades;
    }

    const float &MASS() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MASS;
    }

    const int &tires_upgrades() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->tires_upgrades;
    }

    const Csis::Type_car_type &VerbalType() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->VerbalType;
    }

    const int &induction_upgrades() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->induction_upgrades;
    }

    const int &chassis_upgrades() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->chassis_upgrades;
    }

    const unsigned char &HornType() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->HornType;
    }

    const unsigned char &TrafficEngType() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->TrafficEngType;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
