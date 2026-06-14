#include "./VehicleFragmentConn.h"
#include "./CarInfo.hpp"
#include "./WorldModel.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Render/RenderConn.h"
#include "Speed/Indep/Src/Sim/SimServer.h"
#include "Speed/Indep/Src/World/VehicleFragmentConn.h"
#include "VehicleRenderConn.h"

#ifdef CLANGD_DAMNIT
#include "CarRender.cpp"
#endif

void OrthoInverse(UMath::Matrix4 &m);

BIND_SIM_CONN(VehicleFragmentConn);
typedef struct bTList<VehicleFragmentConn> VehcileFrags;
VehcileFrags TheVehcileFrags;

VehicleFragmentConn::VehicleFragmentConn(const Sim::ConnectionData &data)
    : Sim::Connection(data),             //
      mTarget(0),                        //
      mColName(),                        //
      mModelHash(0),                     //
      mModel(nullptr),                   //
      mReplacementTextureTable(nullptr), //
      mLightMaterial(nullptr) {
    TheVehcileFrags.AddTail(this);

    RenderConn::Pkt_VehicleFragment_Open *oc = static_cast<RenderConn::Pkt_VehicleFragment_Open *>(data.pkt);
    mTarget.Set(oc->mWorldID);
    mVehicleWorldID = oc->mVehicleWorldID;
    mPartSlot = static_cast<CAR_PART_ID>(GetCarPartIDFromCrc(oc->mPartName));
    mColName = oc->mColName;

    bIdentity(&mModelOffset);
}

void VehicleFragmentConn::UpdateModel() {
    if (this->mPartSlot == CARPARTID_INVALID) {
        return;
    }

    if (!this->mTarget.IsValid() || this->mVehicleWorldID == 0) {
        return;
    }

    if (this->mModelHash == 0) {
        VehicleRenderConn *vrc = VehicleRenderConn::Find(this->mVehicleWorldID);

        if (vrc == nullptr || !vrc->IsLoaded()) {
            return;
        } else {
            CarRenderInfo *car_render_info = vrc->GetRenderInfo();

            if (this->mReplacementTextureTable == nullptr) {
                // TODO
                this->mReplacementTextureTable = new ("", 0) eReplacementTextureTable[CarRenderInfo::REPLACETEX_NUM];
                bMemCpy(this->mReplacementTextureTable, car_render_info->MasterReplacementTextureTable,
                        sizeof(car_render_info->MasterReplacementTextureTable));
            }
            if (this->mLightMaterial == nullptr) {
                this->mLightMaterial = car_render_info->LightMaterial_CarSkin;
            }

            this->mModelHash = vrc->FindPart(this->mPartSlot);

            const CollisionGeometry::Collection *col = reinterpret_cast<const CollisionGeometry::Collection *>(
                CollisionGeometry::Lookup(UCrc32(GetCarTypeInfo(vrc->GetCarType())->GetName())));
            if (col != nullptr) {
                const CollisionGeometry::Bounds *root = col->GetBounds(this->mColName);
                if (root != nullptr) {
                    UMath::Matrix4 tmp;
                    UMath::Vector4 root_orientation;
                    UMath::Vector3 root_pivot;

                    root->GetOrientation(root_orientation);
                    root->GetPivot(root_pivot);
                    UMath::QuaternionToMatrix4(root_orientation, tmp);
                    tmp.v3 = UMath::Vector4Make(root_pivot, 1.0f);
#ifdef EA_BUILD_A124
                    UMath::OrthoInverse(tmp);
#else
                    OrthoInverse(tmp);
#endif
                    eSwizzleWorldMatrix(reinterpret_cast<const bMatrix4 &>(tmp), this->mModelOffset);
                }
            }
        }

        if (this->mModelHash == 0) {
            this->mPartSlot = CARPARTID_INVALID;
            return;
        }
    }

    this->mRenderMatrix = *this->mTarget.GetMatrix();
    bMatrix4 matrix;
    eMulMatrix(&matrix, &this->mModelOffset, &this->mRenderMatrix);

    if (this->mModel == nullptr) {
        this->mModel = new WorldModel(this->mModelHash, &matrix, true);
        if (this->mReplacementTextureTable != nullptr) {
            this->mModel->AttachReplacementTextureTable(this->mReplacementTextureTable, CarRenderInfo::REPLACETEX_NUM);
        }
        if (this->mLightMaterial != nullptr) {
            this->mModel->AttachLightMaterial(this->mLightMaterial, STRINGHASH_CARSKIN);
        }
    } else {
        this->mModel->SetEnabledFlag(true);
        this->mModel->SetMatrix(&matrix);
    }
}

VehicleFragmentConn::~VehicleFragmentConn() {
    TheVehcileFrags.Remove(this);
    if (this->mPartSlot != CARPARTID_INVALID) {
        this->mPartSlot = CARPARTID_INVALID;
    }
    if (this->mReplacementTextureTable != nullptr) {
        delete[] this->mReplacementTextureTable;
    }
    if (this->mModel != nullptr) {
        delete this->mModel;
        this->mModel = nullptr;
    }
}

void VehicleFragmentConn::Update(float dT) {
    bool inview = false;

    if (mModel != nullptr) {
        if (mModel->GetLastVisibleFrame() >= mModel->GetLastRenderFrame() && mModel->GetLastRenderFrame() != 0) {
            inview = true;
        }
    }

    float disttoview;
    if (mModel != nullptr) {
        disttoview = mModel->DistanceToGameView();
    } else {
        disttoview = 0.0f;
    }

    RenderConn::Pkt_VehicleFragment_Service pkt(inview, disttoview);
    if (Service(&pkt)) {
        this->UpdateModel();
    } else if (mModel != nullptr) {
        mModel->SetEnabledFlag(false);
    }
}

void VehicleFragmentConn::FetchData(float dT) {
    for (VehcileFrags::iterator iter = TheVehcileFrags.begin(); iter != TheVehcileFrags.end(); iter++) {
        (*iter)->Update(dT);
    }
}
