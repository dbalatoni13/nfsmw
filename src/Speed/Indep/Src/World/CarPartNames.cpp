#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

struct CarPartIDName {
    int PartID;
    const char *Name;
    UCrc32 NameHash;
};

struct CarSlotIDName {
    int SlotID;
    const char *Name;
};

extern CarPartIDName CarPartIDNames[] asm("CarPartIDNames");
extern CarPartIDName CarPartIDOldNames[] asm("CarPartIDOldNames");
extern CarSlotIDName CarSlotIDNames[] asm("CarSlotIDNames");

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
        if (crc == CarPartIDNames[i].NameHash) {
            return CarPartIDNames[i].PartID;
        }
    }

    for (unsigned int j = 0; j < 1; j++) {
        if (crc == CarPartIDOldNames[j].NameHash) {
            return CarPartIDOldNames[j].PartID;
        }
    }

    return -1;
}
