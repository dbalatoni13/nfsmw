#include "./VehicleFragmentConn.h"
#include "./CarInfo.hpp"
#include "./WorldModel.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"

int GetCarPartIDFromCrc(UCrc32 part_name);
const CollisionGeometry::Bounds *CollisionGeometry_Collection_GetBounds(const CollisionGeometry::Collection *collection, UCrc32 name_hash)
    asm("GetBounds__CQ217CollisionGeometry10CollectionG6UCrc32");
void OrthoInverse(UMath::Matrix4 &m);
extern CarTypeInfo *CarTypeInfoArray;

Sim::Connection *VehicleFragmentConn::Construct(const Sim::ConnectionData &data) {
    return new VehicleFragmentConn(data);
}

VehicleFragmentConn::VehicleFragmentConn(const Sim::ConnectionData &data)
    : Sim::Connection(data), //
      mTarget(0), //
      mColName(), //
      mModelHash(0), //
      mModel(0), //
      mReplacementTextureTable(0), //
      mLightMaterial(0) {
    mList.AddTail(this);

    RenderConn::Pkt_VehicleFragment_Open *oc =
        static_cast<RenderConn::Pkt_VehicleFragment_Open *>(data.pkt);
    mTarget.Set(oc->mWorldID);
    mVehicleWorldID = oc->mVehicleWorldID;
    mPartSlot = static_cast<CAR_PART_ID>(GetCarPartIDFromCrc(oc->mPartName));
    mColName = oc->mColName;

    bIdentity(&mModelOffset);
}

VehicleFragmentConn::~VehicleFragmentConn() {
    mList.Remove(this);
    if (mPartSlot != CARPARTID_INVALID) {
        mPartSlot = CARPARTID_INVALID;
    }
    if (mReplacementTextureTable) {
        delete[] mReplacementTextureTable;
    }
    if (mModel) {
        delete mModel;
        mModel = nullptr;
    }
}

void VehicleFragmentConn::OnClose() {
    delete this;
}

Sim::ConnStatus VehicleFragmentConn::OnStatusCheck() {
    return Sim::CONNSTATUS_READY;
}

void VehicleFragmentConn::UpdateModel() {
    if (this->mPartSlot == CARPARTID_INVALID) {
        return;
    }

    bool has_target_matrix = this->mTarget.GetMatrix() != 0;
    if (!has_target_matrix || this->mVehicleWorldID == 0) {
        return;
    }

    if (this->mModelHash == 0) {
        VehicleRenderConn *vehicle_render_conn = VehicleRenderConn::Find(this->mVehicleWorldID);
        if (vehicle_render_conn == 0 || vehicle_render_conn->mState <= VehicleRenderConn::S_Loading) {
            return;
        }

        CarRenderInfo *car_render_info = vehicle_render_conn->GetRenderInfo();
        if (this->mReplacementTextureTable == 0) {
            this->mReplacementTextureTable = new eReplacementTextureTable[CarRenderInfo::REPLACETEX_NUM];
            bMemCpy(this->mReplacementTextureTable, car_render_info->MasterReplacementTextureTable, sizeof(car_render_info->MasterReplacementTextureTable));
        }
        if (this->mLightMaterial == 0) {
            this->mLightMaterial = car_render_info->LightMaterial_CarSkin;
        }

        this->mModelHash = vehicle_render_conn->FindPart(this->mPartSlot);

        CarTypeInfo *car_type_info = CarTypeInfoArray + vehicle_render_conn->GetCarType();
        const CollisionGeometry::Collection *col =
            reinterpret_cast<const CollisionGeometry::Collection *>(CollisionGeometry::Lookup(UCrc32(car_type_info->GetName())));
        if (col != 0) {
            const CollisionGeometry::Bounds *root = CollisionGeometry_Collection_GetBounds(col, this->mColName);
            if (root != 0) {
                UMath::Matrix4 tmp;
                UMath::Vector4 root_orientation;
                UMath::Vector3 root_pivot;

                root_orientation.x = static_cast<float>(root->fOrientation.x) * 3.051851e-05f;
                root_orientation.y = static_cast<float>(root->fOrientation.y) * 3.051851e-05f;
                root_orientation.z = static_cast<float>(root->fOrientation.z) * 3.051851e-05f;
                root_orientation.w = static_cast<float>(root->fOrientation.w) * 3.051851e-05f;
                root_pivot.x = static_cast<float>(root->fPivot.x) * 0.001f;
                root_pivot.y = static_cast<float>(root->fPivot.y) * 0.001f;
                root_pivot.z = static_cast<float>(root->fPivot.z) * 0.001f;

                UMath::QuaternionToMatrix4(root_orientation, tmp);
                tmp.v3 = UMath::Vector4Make(root_pivot, 1.0f);
                OrthoInverse(tmp);
                eSwizzleWorldMatrix(reinterpret_cast<const bMatrix4 &>(tmp), this->mModelOffset);
            }
        }

        if (this->mModelHash == 0) {
            this->mPartSlot = CARPARTID_INVALID;
            return;
        }
    }

    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(this->mTarget.GetMatrix()), *reinterpret_cast<Mtx44 *>(&this->mRenderMatrix));

    bMatrix4 matrix;
    eMulMatrix(&matrix, &this->mModelOffset, &this->mRenderMatrix);

    if (this->mModel == 0) {
        this->mModel = new WorldModel(this->mModelHash, &matrix, true);
        if (this->mReplacementTextureTable != 0) {
            this->mModel->AttachReplacementTextureTable(this->mReplacementTextureTable, CarRenderInfo::REPLACETEX_NUM);
        }
        if (this->mLightMaterial != 0) {
            this->mModel->AttachLightMaterial(this->mLightMaterial, 0xD6D6080A);
        }
    } else {
        this->mModel->SetEnabledFlag(true);
        this->mModel->SetMatrix(&matrix);
    }
}

void VehicleFragmentConn::Update(float dT) {
    bool inview = false;

    if (mModel) {
        if (mModel->GetLastVisibleFrame() >= mModel->GetLastRenderFrame() && mModel->GetLastRenderFrame() != 0) {
            inview = true;
        }
    }

    float disttoview;
    if (mModel) {
        disttoview = mModel->DistanceToGameView();
    } else {
        disttoview = 0.0f;
    }

    RenderConn::Pkt_VehicleFragment_Service pkt(inview, disttoview);
    if (Service(&pkt)) {
        UpdateModel();
    } else {
        if (mModel) {
            mModel->SetEnabledFlag(false);
        }
    }
}

void VehicleFragmentConn::FetchData(float dT) {
    for (VehicleFragmentConn *conn = mList.GetHead(); conn != mList.EndOfList(); conn = conn->GetNext()) {
        conn->Update(dT);
    }
}

bTList<VehicleFragmentConn> VehicleFragmentConn::mList asm("TheVehcileFrags");

UTL::COM::Factory<const Sim::ConnectionData &, Sim::Connection, UCrc32>::Prototype _VehicleFragmentConn("VehicleFragmentConn",
                                                                                                          VehicleFragmentConn::Construct);
