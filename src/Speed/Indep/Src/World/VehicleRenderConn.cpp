#include "./VehicleRenderConn.h"

UTL::Collections::Listable<VehicleRenderConn, 10>::List UTL::Collections::Listable<VehicleRenderConn, 10>::_mTable;

int SkinSlotToMask(int slot) {
    return 1 << ((slot - 1U) & 0x3F);
}

void VehicleRenderConn::OnClose() {
    delete this;
}

bool VehicleRenderConn::CanRender() const {
    return this->mHide == false && this->mState == S_Updated;
}

VehicleRenderConn *VehicleRenderConn::Find(unsigned int worldid) {
    const UTL::Collections::Listable<VehicleRenderConn, 10>::List &list =
        UTL::Collections::Listable<VehicleRenderConn, 10>::GetList();

    for (UTL::Collections::Listable<VehicleRenderConn, 10>::List::const_iterator iter = list.begin(); iter != list.end(); ++iter) {
        VehicleRenderConn *vehicle_render_conn = *iter;

        if (vehicle_render_conn->mWorldRef.GetWorldID() == worldid) {
            return vehicle_render_conn;
        }
    }

    return 0;
}

unsigned int VehicleRenderConn::FindPart(CAR_PART_ID slot) {
    if (slot < 0) {
        return 0;
    }

    if (this->mRenderInfo != 0) {
        return this->mRenderInfo->FindCarPart(slot);
    }

    return 0;
}

unsigned int VehicleRenderConn::HidePart(CAR_PART_ID slot) {
    if (slot < 0) {
        return 0;
    }

    if (this->mRenderInfo != 0) {
        return this->mRenderInfo->HideCarPart(slot, true);
    }

    return 0;
}

void VehicleRenderConn::ShowPart(CAR_PART_ID slot) {
    if (this->mRenderInfo != 0) {
        this->mRenderInfo->HideCarPart(slot, false);
    }
}

bool VehicleRenderConn::CanUpdate() const {
    return this->mState > S_Loading;
}

void VehicleRenderConn::RefreshRenderInfo() {
    this->mRenderInfo->Refresh();
}
