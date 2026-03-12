#include "ControlScenario.hpp"

void bMemSet(void *dest, unsigned char pattern, unsigned int size);

void CleanControlScenarios(IControlScenario **arr_ptrs);

void InitAnimControlScenarios(IControlScenario **arr_ptrs) {
    bool bad_game_flow = false;
    for (int i = 0; i < eCST_NUM_TYPES; i++) {
        bad_game_flow = arr_ptrs[i] != nullptr;
    }
    if (bad_game_flow) {
        CleanControlScenarios(arr_ptrs);
    }
    bMemSet(arr_ptrs, '\0', 0xC);
    for (int i = 0; i < eCST_NUM_TYPES; i++) {
        if (i != eCST_ERROR) {
            if (i == eCST_NIS_Generic) {
                arr_ptrs[eCST_NIS_Generic] = new GenericNISControlScenario();
            }
        }
    }
}

void CleanControlScenarios(IControlScenario **arr_ptrs) {
    for (int i = 0; i < eCST_NUM_TYPES; i++) {
        IControlScenario *p = arr_ptrs[i];
        if (p != nullptr) {
            delete p;
        }
        arr_ptrs[i] = nullptr;
    }
}
