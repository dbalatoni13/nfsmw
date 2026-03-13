#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include "types.h"

#include <string.h>

extern int g_MaximumMaximumTimesBusted;

namespace {

unsigned short GetInfractionValue(const FEInfractionsData &data, GInfractionManager::InfractionType type) {
    return data.GetValue(type);
}

unsigned int GetInfractionCount(const FEInfractionsData &data) {
    return data.NumInfractions();
}

void ClearInfractions(FEInfractionsData &data) {
    bMemSet(&data, 0, sizeof(data));
}

void AddInfractions(FEInfractionsData &dst, const FEInfractionsData &src) {
    dst += src;
}

void DefaultCarRecord(FECarRecord &record) {
    record.Handle = 0xFFFFFFFF;
}

void DefaultCustomizationRecord(FECustomizationRecord &record) {
    record.Handle = 0xFF;
}

bool IsCareerRecordValid(const FECareerRecord &record) {
    return record.Handle != 0xFF;
}

} // namespace

FEPlayerCarDB::FEPlayerCarDB() {
    Default();
}

FEPlayerCarDB::~FEPlayerCarDB() {}

void FECarRecord::Default() {
    Customization = 0xFF;
    FilterBits = 0;
    CareerHandle = 0xFF;
    if (Handle == 0xFFFFFFFF) {
        return;
    }

    Attrib::Gen::pvehicle vehicle(VehicleKey, 0, 0);
    Attrib::Gen::frontend frontend(FEKey, 0, 0);
    const unsigned char *frontendLayout = reinterpret_cast< const unsigned char * >(frontend.GetLayoutPointer());
    int region = *reinterpret_cast< const int * >(frontendLayout + 0x38);
    int isCustomizable = *reinterpret_cast< const int * >(frontendLayout + 0x58);

    if (region == 1) {
        FilterBits |= 0x20000;
    }
    else if (region == 0) {
        FilterBits |= 0x10000;
    }
    else if (region == 2) {
        FilterBits |= 0x40000;
    }
    else if (region == 3) {
        FilterBits |= 0x80000;
    }

    if (region == 3 || isCustomizable == 0) {
        FilterBits |= 0x20;
    }
    else {
        FilterBits |= 1;
    }
}

bool FECarRecord::MatchesFilter(int theFilter) {
    return (FilterBits & static_cast< unsigned int >(theFilter)) == static_cast< unsigned int >(theFilter);
}

unsigned int FECarRecord::GetNameHash() {
    return FEKey;
}

void FECustomizationRecord::Default() {
    for (int i = 0; i < 139; i++) {
        InstalledPartIndices[i] = -1;
    }

    bMemSet(&InstalledPhysics, 0, 0x20);
    for (int i = 0; i < 3; i++) {
        bMemSet(&Tunings[i], 0, sizeof(Tunings[i]));
    }
    Preset = 0;
    ActiveTuning = static_cast< Physics::eCustomTuningType >(0);
}

void FEImpoundData::Default() {
    Pad1 = 0;
    MaxBusted = 3;
    EvadeCount = 0;
    TimesBusted = 0;
    ImpoundedState = 0;
    DaysBeforeRelease = 0;
    Pad2 = 0;
}

void FEImpoundData::BecomeImpounded(eImpoundReasons reason) {
    DaysBeforeRelease = 5;
    ImpoundedState = reason;
    TimesBusted = MaxBusted;
}

void FEImpoundData::NotifyPlayerPaidToRelease() {
    DaysBeforeRelease = 0;
    TimesBusted = 0;
    ImpoundedState = 0;
}

void FEImpoundData::NotifyPlayerUsedMarkerToRelease() {
    NotifyPlayerPaidToRelease();
}

bool FEImpoundData::NotifyWin() {
    if (ImpoundedState != 0 && ((DaysBeforeRelease == 0 || --DaysBeforeRelease == 0) && ImpoundedState != IMPOUND_RELEASED)) {
        ImpoundedState = IMPOUND_RELEASED;
        return true;
    }
    return false;
}

bool FEImpoundData::NotifyBusted() {
    char timesBusted = TimesBusted;

    EvadeCount = 0;
    TimesBusted = timesBusted + 1;
    return MaxBusted <= static_cast< unsigned char >(timesBusted + 1);
}

bool FEImpoundData::NotifyEvade() {
    bool impounded = ImpoundedState != 0;

    if (!impounded) {
        char evadeCount = EvadeCount + 1;
        EvadeCount = evadeCount;
        if (evadeCount > 2) {
            EvadeCount = 0;
            TimesBusted--;
        }
        if (TimesBusted < 0) {
            TimesBusted = 0;
        }
    }

    return NotifyWin();
}

bool FEImpoundData::CanAddMaxBusted() {
    if (static_cast< int >(MaxBusted) < g_MaximumMaximumTimesBusted && ImpoundedState == 0) {
        return true;
    }
    return false;
}

void FEImpoundData::AddMaxBusted() {
    unsigned char maxBusted = MaxBusted;

    MaxBusted = maxBusted + 1;
    if (static_cast< int >(static_cast< unsigned char >(maxBusted + 1)) > g_MaximumMaximumTimesBusted) {
        MaxBusted = static_cast< unsigned char >(g_MaximumMaximumTimesBusted);
    }
}

FEInfractionsData::FEInfractionsData(unsigned int infractions) {
    bMemSet(this, 0, sizeof(*this));
    if ((infractions & 8) != 0) {
        Assault++;
    }
    if ((infractions & 0x20) != 0) {
        Damage++;
    }
    if ((infractions & 0x10) != 0) {
        HitAndRun++;
    }
    if ((infractions & 0x80) != 0) {
        OffRoad++;
    }
    if ((infractions & 2) != 0) {
        Racing++;
    }
    if ((infractions & 4) != 0) {
        Reckless++;
    }
    if ((infractions & 0x40) != 0) {
        Resist++;
    }
    if ((infractions & 1) != 0) {
        Speeding++;
    }
}

void FEInfractionsData::operator+=(const FEInfractionsData &rhs) {
    Speeding += rhs.Speeding;
    Racing += rhs.Racing;
    Reckless += rhs.Reckless;
    Assault += rhs.Assault;
    HitAndRun += rhs.HitAndRun;
    Damage += rhs.Damage;
    Resist += rhs.Resist;
    OffRoad += rhs.OffRoad;
}

unsigned short FEInfractionsData::GetValue(GInfractionManager::InfractionType type) const {
    if (type == GInfractionManager::kInfraction_Assault) {
        return Assault;
    }
    if (type < GInfractionManager::kInfraction_HitAndRun) {
        if (type == GInfractionManager::kInfraction_Racing) {
            return Racing;
        }
        if (type < GInfractionManager::kInfraction_Reckless) {
            if (type == GInfractionManager::kInfraction_Speeding) {
                return Speeding;
            }
        }
        else if (type == GInfractionManager::kInfraction_Reckless) {
            return Reckless;
        }
    }
    else {
        if (type == GInfractionManager::kInfraction_Damage) {
            return Damage;
        }
        if (type < GInfractionManager::kInfraction_Resist) {
            if (type == GInfractionManager::kInfraction_HitAndRun) {
                return HitAndRun;
            }
        }
        else {
            if (type == GInfractionManager::kInfraction_Resist) {
                return Resist;
            }
            if (type == GInfractionManager::kInfraction_OffRoad) {
                return OffRoad;
            }
        }
    }
    return 0;
}

unsigned short FEInfractionsData::NumInfractions() const {
    return OffRoad + Resist + Damage + HitAndRun + Assault + Reckless + Speeding + Racing;
}

void FECareerRecord::Default() {
    Handle = 0xFF;
    TheImpoundData.Default();
    VehicleHeat = 1.0f;
    Bounty = 0;
    NumEvadedPursuits = 0;
    NumBustedPursuits = 0;
    FEInfractionsData unserved(0);
    FEInfractionsData served(0);

    UnservedInfractions = unserved;
    ServedInfractions = served;
}

void FECareerRecord::SetVehicleHeat(float h) {
    VehicleHeat = h;
}

float FECareerRecord::GetVehicleHeat() {
    return VehicleHeat;
}

void FECareerRecord::CommitPursuitCarData(unsigned int infractions, unsigned int accumulated_bounty, bool pursuit_evaded) {
    FEInfractionsData infractionData(infractions);

    UnservedInfractions += infractionData;
    if (pursuit_evaded) {
        Bounty += accumulated_bounty;
        NumEvadedPursuits++;
    }
    else {
        NumBustedPursuits++;
    }
}

void FECareerRecord::WaiveIncractions(unsigned int infractions) {
    if ((infractions & GInfractionManager::kInfraction_Assault) != 0) {
        UnservedInfractions.Assault--;
    }
    if ((infractions & GInfractionManager::kInfraction_Damage) != 0) {
        UnservedInfractions.Damage--;
    }
    if ((infractions & GInfractionManager::kInfraction_HitAndRun) != 0) {
        UnservedInfractions.HitAndRun--;
    }
    if ((infractions & GInfractionManager::kInfraction_OffRoad) != 0) {
        UnservedInfractions.OffRoad--;
    }
    if ((infractions & GInfractionManager::kInfraction_Racing) != 0) {
        UnservedInfractions.Racing--;
    }
    if ((infractions & GInfractionManager::kInfraction_Reckless) != 0) {
        UnservedInfractions.Reckless--;
    }
    if ((infractions & GInfractionManager::kInfraction_Resist) != 0) {
        UnservedInfractions.Resist--;
    }
    if ((infractions & GInfractionManager::kInfraction_Speeding) != 0) {
        UnservedInfractions.Speeding--;
    }
    NumBustedPursuits--;
}

void FECareerRecord::ServeAllIncractions() {
    FEInfractionsData cleared(0);

    ServedInfractions += UnservedInfractions;
    UnservedInfractions = cleared;
}

unsigned int FECareerRecord::GetNumInfraction(GInfractionManager::InfractionType type, bool get_unserved) const {
    return GetInfractions(get_unserved).GetValue(type);
}

FECarRecord *FEPlayerCarDB::GetCarRecordByHandle(unsigned int handle) {
    for (int i = 0; i < 200; i++) {
        if (CarTable[i].Handle == handle) {
            return &CarTable[i];
        }
    }
    return nullptr;
}

FECustomizationRecord *FEPlayerCarDB::GetCustomizationRecordByHandle(unsigned char handle) {
    for (int i = 0; i < 75; i++) {
        if (Customizations[i].Handle == handle) {
            return &Customizations[i];
        }
    }
    return nullptr;
}

FECarRecord *FEPlayerCarDB::GetCarByIndex(int index) {
    if (index > 199) {
        return nullptr;
    }
    return &CarTable[index];
}

bool FEPlayerCarDB::CanCreateNewCarRecord() {
    for (int i = 0; i < 200; i++) {
        if (!CarTable[i].IsValid()) {
            return true;
        }
    }
    return false;
}

bool FEPlayerCarDB::CanCreateNewCustomizationRecord() {
    for (int i = 0; i < 75; i++) {
        if (Customizations[i].Handle == 0xFF) {
            return true;
        }
    }
    return false;
}

FECarRecord *FEPlayerCarDB::CreateNewCarRecord() {
    for (int i = 0; i < 200; i++) {
        if (CarTable[i].Handle == 0xFFFFFFFF) {
            FECarRecord *record = &CarTable[i];

            record->Default();
            record->Handle = i;
            return record;
        }
    }
    return nullptr;
}

FECustomizationRecord *FEPlayerCarDB::CreateNewCustomizationRecord() {
    for (int i = 0; i < 75; i++) {
        if (Customizations[i].Handle == 0xFF) {
            FECustomizationRecord *record = &Customizations[i];

            record->Default();
            record->Handle = i;
            return record;
        }
    }
    return nullptr;
}

FECareerRecord *FEPlayerCarDB::CreateNewCareerRecord() {
    for (int i = 0; i < 25; i++) {
        if (!IsCareerRecordValid(CareerRecords[i])) {
            CareerRecords[i].Default();
            CareerRecords[i].Handle = i;
            return &CareerRecords[i];
        }
    }
    return nullptr;
}

unsigned short FEPlayerCarDB::GetNumInfraction(GInfractionManager::InfractionType type, bool get_unserved) {
    unsigned short total = 0;

    for (int i = 0; i < 25; i++) {
        if (IsCareerRecordValid(CareerRecords[i])) {
            total += static_cast< unsigned short >(CareerRecords[i].GetNumInfraction(type, get_unserved));
        }
    }

    total += GetInfractionValue(get_unserved ? SoldHistoryUnservedInfractions : SoldHistoryServedInfractions, type);
    return total;
}

unsigned int FEPlayerCarDB::GetTotalNumInfractions(bool get_unserved) {
    unsigned int total = 0;

    for (int i = 0; i < 25; i++) {
        if (IsCareerRecordValid(CareerRecords[i])) {
            total += GetInfractionCount(get_unserved ? CareerRecords[i].GetInfractions(true) : CareerRecords[i].GetInfractions(false));
        }
    }

    total += GetInfractionCount(get_unserved ? SoldHistoryUnservedInfractions : SoldHistoryServedInfractions);
    return total;
}

unsigned short FEPlayerCarDB::GetNumInfractionsOnCar(unsigned int car_handle, bool get_unserved) {
    FECarRecord *carRecord = GetCarRecordByHandle(car_handle);
    if (carRecord == nullptr) {
        return 0;
    }

    FECareerRecord *careerRecord = GetCareerRecordByHandle(carRecord->CareerHandle);
    if (careerRecord == nullptr) {
        return 0;
    }

    return static_cast< unsigned short >(GetInfractionCount(get_unserved ? careerRecord->GetInfractions(true)
                                                                         : careerRecord->GetInfractions(false)));
}

unsigned int FEPlayerCarDB::GetTotalBounty() {
    unsigned int total = SoldHistoryBounty;

    for (int i = 0; i < 25; i++) {
        if (IsCareerRecordValid(CareerRecords[i])) {
            total += CareerRecords[i].GetBounty();
        }
    }

    return total;
}

unsigned int FEPlayerCarDB::GetTotalEvadedPursuits() {
    unsigned int total = SoldHistoryNumEvadedPursuits;

    for (int i = 0; i < 25; i++) {
        if (IsCareerRecordValid(CareerRecords[i])) {
            total += CareerRecords[i].GetNumEvadedPursuits();
        }
    }

    return total;
}

unsigned int FEPlayerCarDB::GetTotalBustedPursuits() {
    unsigned int total = SoldHistoryNumBustedPursuits;

    for (int i = 0; i < 25; i++) {
        if (IsCareerRecordValid(CareerRecords[i])) {
            total += CareerRecords[i].GetNumBustedPursuits();
        }
    }

    return total;
}

unsigned int FEPlayerCarDB::GetNumImpoundedCars() {
    unsigned int total = 0;

    for (int i = 0; i < 25; i++) {
        if (IsCareerRecordValid(CareerRecords[i]) && CareerRecords[i].TheImpoundData.IsImpounded()) {
            total++;
        }
    }

    return total;
}

unsigned int FEPlayerCarDB::GetNumCareerCarsWithARecord() {
    unsigned int total = 0;

    for (int i = 0; i < 200; i++) {
        if (CarTable[i].IsValid() && CarTable[i].CareerHandle != 0xFF) {
            total++;
        }
    }

    return total;
}

void FEPlayerCarDB::BackupSoldCarHistory(unsigned char sold_car) {
    FECareerRecord *careerRecord = GetCareerRecordByHandle(sold_car);
    if (careerRecord == nullptr) {
        return;
    }

    SoldHistoryBounty += careerRecord->GetBounty();
    SoldHistoryNumEvadedPursuits += careerRecord->GetNumEvadedPursuits();
    SoldHistoryNumBustedPursuits += careerRecord->GetNumBustedPursuits();
    AddInfractions(SoldHistoryUnservedInfractions, careerRecord->GetInfractions(true));
    AddInfractions(SoldHistoryServedInfractions, careerRecord->GetInfractions(false));
}

int FEPlayerCarDB::GetNumCars(unsigned int filter) {
    int total = 0;

    for (int i = 0; i < 200; i++) {
        if (CarTable[i].IsValid() && CarTable[i].MatchesFilter(filter)) {
            total++;
        }
    }

    return total;
}

void FEPlayerCarDB::DeleteAllCars() {
    for (int i = 0; i < 200; i++) {
        DefaultCarRecord(CarTable[i]);
    }
}

void FEPlayerCarDB::DeleteAllCustomizations() {
    for (int i = 0; i < 75; i++) {
        DefaultCustomizationRecord(Customizations[i]);
    }
}

void FEPlayerCarDB::DeleteAllCareerRecords() {
    for (int i = 0; i < 25; i++) {
        CareerRecords[i].Handle = 0xFF;
    }
}

void FEPlayerCarDB::Default() {
    DeleteAllCars();
    DeleteAllCustomizations();
    DeleteAllCareerRecords();

    SoldHistoryBounty = 0;
    SoldHistoryNumEvadedPursuits = 0;
    SoldHistoryNumBustedPursuits = 0;
    ClearInfractions(SoldHistoryUnservedInfractions);
    ClearInfractions(SoldHistoryServedInfractions);
}

FECareerRecord *FEPlayerCarDB::GetCareerRecordByHandle(unsigned char handle) {
    for (int i = 0; i < 25; i++) {
        if (CareerRecords[i].Handle == handle) {
            return &CareerRecords[i];
        }
    }
    return nullptr;
}
