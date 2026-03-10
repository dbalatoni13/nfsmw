#include "ControlScenario.hpp"
#include "GenericNISControlScenario.hpp"

extern const unsigned int WAM_NIS_GENERIC_CONTROL_MSG;

GenericNISControlScenario::~GenericNISControlScenario() {}

void GenericNISControlScenario::HandleEventMessage(CWorldAnimEntityTree *entity_tree, unsigned int wa_msg, void *data) {
    if (wa_msg != WAM_NIS_GENERIC_CONTROL_MSG) {
        return;
    }

    GenericNISControlScenarioInfo *gcsi = static_cast<GenericNISControlScenarioInfo *>(data);
    bool set_to_pos_immediately = bAbs(gcsi->len) < 0.001;
    int open;
    if (bAbs(gcsi->pos - 1.0f) >= 0.001) {
        open = 0;
        if (bAbs(gcsi->pos) >= 0.001) {
            open = -1;
        }
    } else {
        open = 1;
    }
    float frame_len = gcsi->len;

    for (bPNode *node = entity_tree->instantiated_world_anim_entities.GetHead();
         node != entity_tree->instantiated_world_anim_entities.EndOfList();
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
