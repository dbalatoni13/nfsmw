#include "./CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

struct CarPart {
    unsigned int GetTextureNameHash();
    unsigned int GetAppliedAttributeUParam(unsigned int namehash, unsigned int default_value);
};

CarTypeInfo *CarTypeInfoArray;

unsigned int RideInfo::GetSkinNameHash() {
    CarPart *skin_base;
    unsigned int skin_name_hash;

    skin_name_hash = 0;

    if (this->IsUsingCompositeSkin() != 0) {
        skin_name_hash = this->mCompositeSkinHash;
    } else {
        skin_base = this->GetPart(0x4C);
        if (skin_base != 0) {
            skin_name_hash = skin_base->GetAppliedAttributeUParam(0x10C98090, 0);
        }
    }

    return skin_name_hash;
}

void RideInfo::SetCompositeNameHash(int skin_number) {
    char temp[64];
    unsigned int dummy_skin_hash;
    unsigned int dummy_wheel_hash;
    unsigned int dummy_spinner_hash;

    if (static_cast<unsigned int>(skin_number - 1) < 4) {
        this->SkinType = 1;
    } else if (static_cast<unsigned int>(skin_number - 5) < 8) {
        this->SkinType = 2;
    } else {
        this->SkinType = 0;
    }

    bSPrintf(temp, "DUMMY_SKIN%d", skin_number);
    dummy_skin_hash = bStringHash(temp);
    this->SetCompositeSkinNameHash(dummy_skin_hash);

    bSPrintf(temp, "DUMMY_WHEEL%d", skin_number);
    dummy_wheel_hash = bStringHash(temp);
    this->SetCompositeWheelNameHash(dummy_wheel_hash);

    bSPrintf(temp, "DUMMY_SPINNER%d", skin_number);
    dummy_spinner_hash = bStringHash(temp);
    this->SetCompositeSpinnerNameHash(dummy_spinner_hash);
}

unsigned int RideInfo::GetCompositeSkinNameHash() {
    return this->mCompositeSkinHash;
}

void RideInfo::SetCompositeSkinNameHash(unsigned int namehash) {
    CarTypeInfo *type_info = &CarTypeInfoArray[this->Type];

    if (type_info->Skinnable != 0) {
        this->mCompositeSkinHash = namehash;
    } else {
        this->mCompositeSkinHash = 0;
    }
}

unsigned int RideInfo::GetCompositeWheelNameHash() {
    return this->mCompositeWheelHash;
}

void RideInfo::SetCompositeWheelNameHash(unsigned int namehash) {
    this->mCompositeWheelHash = namehash;
}

unsigned int RideInfo::GetCompositeSpinnerNameHash() {
    return this->mCompositeSpinnerHash;
}

void RideInfo::SetCompositeSpinnerNameHash(unsigned int namehash) {
    this->mCompositeSpinnerHash = namehash;
}

int RideInfo::IsUsingCompositeSkin() {
    return this->GetCompositeSkinNameHash() != 0;
}
