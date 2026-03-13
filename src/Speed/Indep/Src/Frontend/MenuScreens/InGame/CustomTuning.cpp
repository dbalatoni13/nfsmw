#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/CustomTuning.hpp"

bool CustomTuningScreen::IsTuningAvailable(FEPlayerCarDB *stable, FECarRecord *record, Physics::Tunings::Path path) {
    if (record == nullptr) {
        return false;
    }

    FECustomizationRecord *custom = stable->GetCustomizationRecordByHandle(record->Customization);
    if (custom == nullptr) {
        return false;
    }

    switch (path) {
    case Physics::Tunings::STEERING:
    case Physics::Tunings::HANDLING:
    case Physics::Tunings::RIDEHEIGHT:
        return custom->InstalledPhysics.Part[2] > 0;
    case Physics::Tunings::BRAKES:
        return custom->InstalledPhysics.Part[1] > 0;
    case Physics::Tunings::AERODYNAMICS: {
        RideInfo ride;

        ride.Init(static_cast< CarType >(-1), CarRenderUsage_Player, 0, 0);
        stable->BuildRideForPlayer(record->Handle, 0, &ride);
        ride.SetStockParts();

        struct CarPart *stock = ride.GetPart(0x2C);
        struct CarPart *installed = custom->GetInstalledPart(record->GetType(), 0x2C);
        if (installed != nullptr && installed != stock) {
            return true;
        }

        stock = ride.GetPart(0x17);
        installed = custom->GetInstalledPart(record->GetType(), 0x17);
        return installed != nullptr && installed != stock;
    }
    case Physics::Tunings::NOS:
        return custom->InstalledPhysics.Part[6] > 0;
    case Physics::Tunings::INDUCTION:
        return custom->InstalledPhysics.Part[5] > 0;
    default:
        return false;
    }
}

unsigned int CustomTuningScreen::GetNameForPath(Physics::Tunings::Path path, bool turbo) {
    switch (path) {
    case Physics::Tunings::STEERING:
        return 0xC56C5B36;
    case Physics::Tunings::HANDLING:
        return 0xFCEEBE1A;
    case Physics::Tunings::BRAKES:
        return 0x2EE2A74D;
    case Physics::Tunings::RIDEHEIGHT:
        return 0xCF6215D1;
    case Physics::Tunings::AERODYNAMICS:
        return 0x7196ACB4;
    case Physics::Tunings::NOS:
        return 0x934FCFA9;
    case Physics::Tunings::INDUCTION:
        return turbo ? 0x86945521 : 0xE3A932E0;
    default:
        return 0;
    }
}

unsigned int CustomTuningScreen::GetHelpForPath(Physics::Tunings::Path path, bool active, bool turbo) {
    if (active) {
        switch (path) {
        case Physics::Tunings::STEERING:
            return 0x4A1F18BE;
        case Physics::Tunings::HANDLING:
            return 0x81A17BA2;
        case Physics::Tunings::BRAKES:
            return 0x473862D5;
        case Physics::Tunings::RIDEHEIGHT:
            return 0xD6C24659;
        case Physics::Tunings::AERODYNAMICS:
            return 0x64FCEE3C;
        case Physics::Tunings::NOS:
            return 0xB65CFC31;
        case Physics::Tunings::INDUCTION:
            return turbo ? 0xB5DCBFA9 : 0xD70F7468;
        default:
            return 0;
        }
    }

    switch (path) {
    case Physics::Tunings::STEERING:
        return 0x221D7E85;
    case Physics::Tunings::HANDLING:
        return 0x18C12069;
    case Physics::Tunings::BRAKES:
        return 0xC213A6DC;
    case Physics::Tunings::RIDEHEIGHT:
        return 0xB6D02C60;
    case Physics::Tunings::AERODYNAMICS:
        return 0xC6A99483;
    case Physics::Tunings::NOS:
        return 0xB8124038;
    case Physics::Tunings::INDUCTION:
        return turbo ? 0xB7D6F7B0 : 0xE3F577AF;
    default:
        return 0;
    }
}
