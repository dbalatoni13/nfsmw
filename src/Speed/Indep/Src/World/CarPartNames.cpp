#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "string.h"

struct CarPartIDName {
    int PartID;
    const char *Name;
    unsigned int NameHash;
};

struct CarSlotIDName {
    int SlotID;
    const char *Name;
};

CarPartIDName CarPartIDNames[CARPARTID_NUM];
CarPartIDName CarPartIDOldNames[1];
extern CarSlotIDName CarSlotIDNames[] asm("CarSlotIDNames");

#define INIT_CAR_PART_NAME(id, name_literal) \
    memset(&CarPartIDNames[id], 0, sizeof(CarPartIDNames[id])); \
    CarPartIDNames[id].PartID = id; \
    CarPartIDNames[id].Name = name_literal; \
    CarPartIDNames[id].NameHash = stringhash32(name_literal)

struct CarPartIDNamesStartup {
        CarPartIDNamesStartup() {
        memset(CarPartIDNames, 0, sizeof(CarPartIDNames));

        memset(&CarPartIDNames[0], 0, sizeof(CarPartIDNames[0]));
        CarPartIDNames[0].Name = "BASE";
        CarPartIDNames[0].NameHash = stringhash32("BASE");

        INIT_CAR_PART_NAME(1, "DAMAGE_FRONT_WINDOW");
        INIT_CAR_PART_NAME(2, "DAMAGE_BODY");
        INIT_CAR_PART_NAME(3, "DAMAGE_COP_LIGHTS");
        INIT_CAR_PART_NAME(4, "DAMAGE_COP_SPOILER");
        INIT_CAR_PART_NAME(5, "DAMAGE_FRONT_WHEEL");
        INIT_CAR_PART_NAME(6, "DAMAGE_LEFT_BRAKELIGHT");
        INIT_CAR_PART_NAME(7, "DAMAGE_RIGHT_BRAKELIGHT");
        INIT_CAR_PART_NAME(8, "DAMAGE_LEFT_HEADLIGHT");
        INIT_CAR_PART_NAME(9, "DAMAGE_RIGHT_HEADLIGHT");
        INIT_CAR_PART_NAME(10, "DAMAGE_HOOD");
        INIT_CAR_PART_NAME(11, "DAMAGE_BUSHGUARD");
        INIT_CAR_PART_NAME(12, "DAMAGE_FRONT_BUMPER");
        INIT_CAR_PART_NAME(13, "DAMAGE_RIGHT_DOOR");
        INIT_CAR_PART_NAME(14, "DAMAGE_RIGHT_REAR_DOOR");
        INIT_CAR_PART_NAME(15, "DAMAGE_TRUNK");
        INIT_CAR_PART_NAME(16, "DAMAGE_REAR_BUMPER");
        INIT_CAR_PART_NAME(17, "DAMAGE_REAR_LEFT_WINDOW");
        INIT_CAR_PART_NAME(18, "DAMAGE_FRONT_LEFT_WINDOW");
        INIT_CAR_PART_NAME(19, "DAMAGE_FRONT_RIGHT_WINDOW");
        INIT_CAR_PART_NAME(20, "DAMAGE_REAR_RIGHT_WINDOW");
        INIT_CAR_PART_NAME(21, "DAMAGE_LEFT_DOOR");
        INIT_CAR_PART_NAME(22, "DAMAGE_LEFT_REAR_DOOR");
        INIT_CAR_PART_NAME(23, "BODY");
        INIT_CAR_PART_NAME(24, "FRONT_BRAKE");
        INIT_CAR_PART_NAME(25, "FRONT_LEFT_WINDOW");
        INIT_CAR_PART_NAME(26, "FRONT_RIGHT_WINDOW");
        INIT_CAR_PART_NAME(27, "FRONT_WINDOW");
        INIT_CAR_PART_NAME(28, "INTERIOR");
        INIT_CAR_PART_NAME(29, "LEFT_BRAKELIGHT");
        INIT_CAR_PART_NAME(30, "LEFT_BRAKELIGHT_GLASS");
        INIT_CAR_PART_NAME(31, "LEFT_HEADLIGHT");
        INIT_CAR_PART_NAME(32, "LEFT_HEADLIGHT_GLASS");
        INIT_CAR_PART_NAME(33, "LEFT_SIDE_MIRROR");
        INIT_CAR_PART_NAME(34, "REAR_BRAKE");
        INIT_CAR_PART_NAME(35, "REAR_LEFT_WINDOW");
        INIT_CAR_PART_NAME(36, "REAR_RIGHT_WINDOW");
        INIT_CAR_PART_NAME(37, "REAR_WINDOW");
        INIT_CAR_PART_NAME(38, "RIGHT_BRAKELIGHT");
        INIT_CAR_PART_NAME(39, "RIGHT_BRAKELIGHT_GLASS");
        INIT_CAR_PART_NAME(40, "RIGHT_HEADLIGHT");
        INIT_CAR_PART_NAME(41, "RIGHT_HEADLIGHT_GLASS");
        INIT_CAR_PART_NAME(42, "RIGHT_SIDE_MIRROR");
        INIT_CAR_PART_NAME(43, "DRIVER");
        INIT_CAR_PART_NAME(44, "SPOILER");
        INIT_CAR_PART_NAME(45, "UNIVERSAL_SPOILER_BASE");
        INIT_CAR_PART_NAME(46, "DAMAGE0_FRONT");
        INIT_CAR_PART_NAME(47, "DAMAGE0_FRONTLEFT");
        INIT_CAR_PART_NAME(48, "DAMAGE0_FRONTRIGHT");
        INIT_CAR_PART_NAME(49, "DAMAGE0_REAR");
        INIT_CAR_PART_NAME(50, "DAMAGE0_REARLEFT");
        INIT_CAR_PART_NAME(51, "DAMAGE0_REARRIGHT");
        INIT_CAR_PART_NAME(52, "ATTACHMENT0");
        INIT_CAR_PART_NAME(53, "ATTACHMENT1");
        INIT_CAR_PART_NAME(54, "ATTACHMENT2");
        INIT_CAR_PART_NAME(55, "ATTACHMENT3");
        INIT_CAR_PART_NAME(56, "ATTACHMENT4");
        INIT_CAR_PART_NAME(57, "ATTACHMENT5");
        INIT_CAR_PART_NAME(58, "ATTACHMENT6");
        INIT_CAR_PART_NAME(59, "ATTACHMENT7");
        INIT_CAR_PART_NAME(60, "ATTACHMENT8");
        INIT_CAR_PART_NAME(61, "ATTACHMENT9");
        INIT_CAR_PART_NAME(62, "ROOF");
        INIT_CAR_PART_NAME(63, "HOOD");
        INIT_CAR_PART_NAME(64, "HEADLIGHT");
        INIT_CAR_PART_NAME(65, "BRAKELIGHT");
        INIT_CAR_PART_NAME(66, "BRAKE");
        INIT_CAR_PART_NAME(67, "WHEEL");
        INIT_CAR_PART_NAME(68, "SPINNER");
        INIT_CAR_PART_NAME(69, "LICENSE_PLATE");
        INIT_CAR_PART_NAME(70, "DECAL_FRONT_WINDOW");
        INIT_CAR_PART_NAME(71, "DECAL_REAR_WINDOW");
        INIT_CAR_PART_NAME(72, "DECAL_LEFT_DOOR");
        INIT_CAR_PART_NAME(73, "DECAL_RIGHT_DOOR");
        INIT_CAR_PART_NAME(74, "DECAL_LEFT_QUARTER");
        INIT_CAR_PART_NAME(75, "DECAL_RIGHT_QUARTER");
        INIT_CAR_PART_NAME(76, "PAINT");
        INIT_CAR_PART_NAME(77, "VINYL_PAINT");
        INIT_CAR_PART_NAME(78, "RIM_PAINT");
        INIT_CAR_PART_NAME(79, "VINYL");
        INIT_CAR_PART_NAME(80, "DECAL_TEXTURE");
        INIT_CAR_PART_NAME(81, "WINDOW_TINT");
        INIT_CAR_PART_NAME(82, "CUSTOM_HUD");
        INIT_CAR_PART_NAME(83, "CUSTOM_HUD_PAINT");
        INIT_CAR_PART_NAME(84, "CV");
        INIT_CAR_PART_NAME(85, "WHEEL_MANUFACTURER");
        INIT_CAR_PART_NAME(86, "MISC");

        memset(CarPartIDOldNames, 0, sizeof(CarPartIDOldNames));
        CarPartIDOldNames[0].PartID = CARPARTID_BRAKE;
        CarPartIDOldNames[0].Name = "BRAKE";
        CarPartIDOldNames[0].NameHash = stringhash32("BRAKE");
    }
};

CarPartIDNamesStartup _CarPartIDNamesStartup;

#undef INIT_CAR_PART_NAME

int GetNumCarPartIDNames();
int GetNumCarSlotIDNames();

const char *GetCarPartNameFromID(int car_part_id) {
    int num_car_part_names = GetNumCarPartIDNames();

    if (car_part_id >= 0 && car_part_id < num_car_part_names) {
        if (CarPartIDNames[car_part_id].PartID == car_part_id) {
            return CarPartIDNames[car_part_id].Name;
        }

        for (int i = 0; i < num_car_part_names; i++) {
            if (CarPartIDNames[i].PartID == car_part_id) {
                return CarPartIDNames[i].Name;
            }
        }
    }

    return 0;
}

const char *GetCarSlotNameFromID(int car_slot_id) {
    int num_car_slot_names = GetNumCarSlotIDNames();

    if (car_slot_id >= 0 && car_slot_id < num_car_slot_names) {
        if (CarSlotIDNames[car_slot_id].SlotID == car_slot_id) {
            return CarSlotIDNames[car_slot_id].Name;
        }

        for (int i = 0; i < num_car_slot_names; i++) {
            if (CarSlotIDNames[i].SlotID == car_slot_id) {
                return CarSlotIDNames[i].Name;
            }
        }
    }

    return 0;
}

int GetCarPartIDFromCrc(UCrc32 crc) {
    int num_car_part_names = GetNumCarPartIDNames();

    for (int i = 0; i < num_car_part_names; i++) {
        if (crc.GetValue() == CarPartIDNames[i].NameHash) {
            return CarPartIDNames[i].PartID;
        }
    }

    for (unsigned int j = 0; j < 1; j++) {
        if (crc.GetValue() == CarPartIDOldNames[j].NameHash) {
            return CarPartIDOldNames[j].PartID;
        }
    }

    return -1;
}
