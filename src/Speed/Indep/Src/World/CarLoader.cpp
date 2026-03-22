#include "./CarLoader.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

struct RideInfoLayout {
    CarType Type;
    char InstanceIndex;
    char HasDash;
    char CanBeVertexDamaged;
    char SkinType;
    CARPART_LOD mMinLodLevel;
    CARPART_LOD mMaxLodLevel;
    CARPART_LOD mMinFELodLevel;
    CARPART_LOD mMaxFELodLevel;
};

LoadedWheel::LoadedWheel(RideInfo *ride_info, bool in_fe) {
    RideInfoLayout *ride_layout = reinterpret_cast<RideInfoLayout *>(ride_info);

    this->pCarPart = 0;
    this->LoadState = CARLOADSTATE_QUEUED;
    this->LoadStateSkinPerm = CARLOADSTATE_QUEUED;
    this->LoadStateSkinTemp = CARLOADSTATE_QUEUED;
    this->PartNameHash = 0;
    this->TextureBaseNameHash = 0;

    if (!in_fe) {
        this->mMinLodLevel = ride_layout->mMinLodLevel;
        this->mMaxLodLevel = ride_layout->mMaxLodLevel;
    } else {
        this->mMinLodLevel = ride_layout->mMinFELodLevel;
        this->mMaxLodLevel = ride_layout->mMaxFELodLevel;
    }

    bMemSet(this->ModelNameHashes, 0, sizeof(this->ModelNameHashes));
    bMemSet(this->SkinNameHashesPerm, 0, sizeof(this->SkinNameHashesPerm));
    bMemSet(this->SkinNameHashesTemp, 0, sizeof(this->SkinNameHashesTemp));
    bMemSet(this->LoadedSkinLayersPerm, 0, sizeof(this->LoadedSkinLayersPerm));
    bMemSet(this->LoadedSkinLayersTemp, 0, sizeof(this->LoadedSkinLayersTemp));

    CarPart *car_part = ride_info->GetPart(0x42);
    if (car_part != 0) {
        this->pCarPart = car_part;
        this->PartNameHash = car_part->GetPartNameHash();
        this->TextureBaseNameHash = car_part->GetAppliedAttributeUParam(0x10C98090, 0);

        if (car_part->GetCarTypeNameHash() == bStringHash("WHEELS")) {
            for (int model = 0; model < 1; model++) {
                for (int lod = this->mMinLodLevel; lod <= this->mMaxLodLevel; lod++) {
                    reinterpret_cast<unsigned int *>(this->ModelNameHashes)[lod + model * 5] = car_part->GetModelNameHash(model, lod);
                }
            }

            if (this->TextureBaseNameHash != 0) {
                this->SkinNameHashesPerm[0] = bStringHash("_WHEEL", this->TextureBaseNameHash);
            }
            return;
        }
    }

    this->LoadStateSkinTemp = CARLOADSTATE_LOADED;
    this->LoadState = CARLOADSTATE_LOADED;
    this->LoadStateSkinPerm = CARLOADSTATE_LOADED;
}

LoadedSkin::LoadedSkin(RideInfo *ride_info, int in_front_end, int is_player_skin) {
    this->pRideInfo = ride_info;
    this->InFrontEnd = in_front_end;
    this->pLoadedTexturesPack = 0;
    this->pLoadedVinylsPack = 0;
    this->NumLoadedSkinLayersPerm = 0;
    this->IsPlayerSkin = is_player_skin;
    this->DoneComposite = 0;
    this->LoadStatePerm = 0;
    this->LoadStateTemp = 0;
    bMemSet(this->LoadedSkinLayersPerm, 0, sizeof(this->LoadedSkinLayersPerm));
    this->NumLoadedSkinLayersTemp = 0;
    bMemSet(this->LoadedSkinLayersTemp, 0, sizeof(this->LoadedSkinLayersTemp));
}

LoadedCar::LoadedCar(RideInfo *ride_info, int in_front_end, int is_two_player) {
    this->pRideInfo = ride_info;
    this->LoadState = CARLOADSTATE_QUEUED;
    this->Type = ride_info->Type;
    this->InFrontEnd = in_front_end;
    this->IsTwoPlayer = is_two_player;
    this->pLoadedSolidPack = 0;
}

int LoadedRideInfo::sNextID;

LoadedRideInfo::LoadedRideInfo(RideInfo *ride_info, int in_front_end, int is_two_player, int is_player_car)
    : TheRideInfo(*ride_info),            //
      TheLoadedCar(&TheRideInfo, in_front_end, is_two_player), //
      TheLoadedWheel(&TheRideInfo, in_front_end != 0),         //
      TheLoadedSkin(&TheRideInfo, in_front_end, is_player_car) {
    this->HighPriority = 0;
    this->NumInstances = 0;
    this->LoadState = CARLOADSTATE_QUEUED;
    this->PrintedLoading = 0;
    this->ID = sNextID;
    sNextID++;
    this->pCarTypeInfo = &CarTypeInfoArray[ride_info->Type];
    this->pLoadedCar = &this->TheLoadedCar;
    this->pLoadedWheel = &this->TheLoadedWheel;
    this->pLoadedSkin = &this->TheLoadedSkin;
    bSPrintf(this->Name, "%s(%d)", this->pCarTypeInfo->CarTypeName, this->ID);
}
