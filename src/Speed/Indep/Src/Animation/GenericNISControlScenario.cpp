#include "GenericNISControlScenario.hpp"

static unsigned int WAM_START_TRIGGER = bStringHash("WAM_START_TRIGGER");
static unsigned int WAM_STOP_TRIGGER = bStringHash("WAM_STOP_TRIGGER");
static unsigned int WAM_FIRST_FRAME = bStringHash("WAM_FIRST_FRAME");
static unsigned int WAM_LAST_FRAME = bStringHash("WAM_LAST_FRAME");
static unsigned int WAM_SOUND_TRIGGER_START = bStringHash("WAM_SOUND_TRIGGER_START");
static unsigned int WAM_SOUND_TRIGGER_STOP = bStringHash("WAM_SOUND_TRIGGER_STOP");
static unsigned int WAM_NIS_GENERIC_CONTROL_MSG = bStringHash("WAM_NIS_GENERIC_CONTROL_MSG");
static unsigned int WAM_FWD_REV_TRACK_CONTROL_MSG = bStringHash("WAM_FWD_REV_TRACK_CONTROL_MSG");

void GenericNISControlScenario::HandleEventMessage(CWorldAnimEntityTree *entity_tree, uint32 wa_msg, void *data) {
    if (wa_msg != WAM_NIS_GENERIC_CONTROL_MSG) {
        return;
    }

    GenericNISControlScenarioInfo *gcsi = static_cast<GenericNISControlScenarioInfo *>(data);
    bool set_to_pos_immediately = __builtin_fabs(gcsi->len) < 0.001;
    int open;
    if (__builtin_fabs(gcsi->pos - 1.0f) >= 0.001) {
        open = 0;
        if (__builtin_fabs(gcsi->pos) >= 0.001) {
            open = -1;
        }
    } else {
        open = 1;
    }
    float frame_len = gcsi->len;

    for (bPNode *node = entity_tree->instantiated_world_anim_entities.GetHead(); node != entity_tree->instantiated_world_anim_entities.EndOfList();
         node = node->GetNext()) {
        CWorldAnimEntity *entity = reinterpret_cast<CWorldAnimEntity *>(node->GetObject());
        if (gcsi->entity_hash == entity->GetInstanceNameHash()) {
            CWorldAnimCtrl *anim_ctrl = entity->GetAnimCtrl();
            if (anim_ctrl != nullptr) {
                if (set_to_pos_immediately) {
                    if (open == 1) {
                        anim_ctrl->JumpToEndForTrigger();
                    } else if (open == 0) {
                        anim_ctrl->JumpToBeginForTrigger();
                    } else {
                        anim_ctrl->SetEvalTime(gcsi->pos * anim_ctrl->GetAnimLength());
                    }
                } else {
                    anim_ctrl->SetTimeScale(1.0f);
                    anim_ctrl->ClearFlags(0x180);
                    anim_ctrl->ApplySpeedModifier(anim_ctrl->GetAnimLength() / frame_len);
                    anim_ctrl->Play();
                }
            }
        }
    }
}
