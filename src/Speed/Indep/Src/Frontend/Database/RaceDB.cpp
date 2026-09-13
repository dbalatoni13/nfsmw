#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#include "types.h"

#include <string.h>

void FixDot(char *buf, int size) {
    char *pFixDot = buf;
    for (int i = 0; i < size; i++) {
        if (pFixDot[i] == '.') {
            pFixDot[i] = '_';
        }
    }
}

uint32 CalcLanguageHash(const char *prefix, GRaceParameters *pRaceParams) {
    char buffer[64];
    FEngSNPrintf(buffer, sizeof(buffer), "%s%s", prefix, pRaceParams->GetEventID());
    FixDot(buffer, sizeof(buffer));
    return FEHashUpper(buffer);
}

void RaceSettings::Default() {
    NumOpponents = 3;
    AISkill = 1;
    CopDensity = 1;
    TrafficDensity = 1;
    CatchUp = true;
    NumLaps = 2;
    CopsOn = false;
    TrackDirection = 0;
    IsLapKO = false;
    for (int i = 0; i < 2; i++) {
        SelectedCar[i] = 0;
    }
    RegionFilterBits = 3;
}

void HighScoresDatabase::CommitHighScoresPauseQuit() {
    TotalLosses++;
}

void HighScoresDatabase::CommitPursuitInfo(IPursuit *iPursuit, uint32 car_FEKey, int32 bounty, unsigned int num_infractions) {
    PreviouslyPursuedCarFEKey = car_FEKey;

    int cost_to_state = iPursuit->CalcTotalCostToState();

    if (!iPursuit->IsPerpBusted()) {
        CostToStateDetails.mNumCopCarsDeployed += iPursuit->GetNumCopCarsDeployed();
        CostToStateDetails.mNumHeliSpikeStripsDeployed += iPursuit->GetNumHeliSpikeStripDeployed();
        CostToStateDetails.mNumPropertiesDamaged += iPursuit->GetNumPropertyDamaged();
        CostToStateDetails.mNumRoadblocksDeployed += iPursuit->GetNumRoadblocksDeployed();
        CostToStateDetails.mNumSpikeStripsDeployed += iPursuit->GetNumSpikeStripsDeployed();
        CostToStateDetails.mNumSupportVehiclesDeployed += iPursuit->GetNumSupportVehiclesDeployed();
        CostToStateDetails.mNumTrafficCarsHit += iPursuit->GetNumTrafficCarsHit();
        CostToStateDetails.mPropertyDamageValue += iPursuit->GetValueOfPropertyDamaged();

        CareerPursuitDetails.IncValue(PD_PURUSIT_LENGTH, Timer(iPursuit->GetPursuitDuration()).GetPackedTime());
        CareerPursuitDetails.IncValue(PD_COPS_INVOLVED, iPursuit->GetTotalNumCopsInvolved());
        CareerPursuitDetails.IncValue(PD_COPS_DAMAGED, iPursuit->GetNumCopsDamaged());
        CareerPursuitDetails.IncValue(PD_COPS_DESTROYED, iPursuit->GetNumCopsDestroyed());
        CareerPursuitDetails.IncValue(PD_SPIKESTRIPS_DODGED, iPursuit->GetNumSpikeStripsDodged());
        CareerPursuitDetails.IncValue(PD_ROADBLOCKS_DODGED, iPursuit->GetNumRoadblocksDodged());
        CareerPursuitDetails.IncValue(PD_HELICOPTERS_INVOLVED, iPursuit->GetNumHeliSpawns());
        CareerPursuitDetails.IncValue(PD_COST_TO_STATE, cost_to_state);

        if ((Timer(iPursuit->GetPursuitDuration()) > Timer(BestPursuitRankings[PD_PURUSIT_LENGTH].Value)) != 0) {
            BestPursuitRankings[PD_PURUSIT_LENGTH].Value = Timer(iPursuit->GetPursuitDuration()).GetPackedTime();
            BestPursuitRankings[PD_PURUSIT_LENGTH].CarFEKey = car_FEKey;
        }

        if (iPursuit->GetTotalNumCopsInvolved() > BestPursuitRankings[PD_COPS_INVOLVED].Value) {
            BestPursuitRankings[PD_COPS_INVOLVED].Value = iPursuit->GetTotalNumCopsInvolved();
            BestPursuitRankings[PD_COPS_INVOLVED].CarFEKey = car_FEKey;
        }

        if (iPursuit->GetNumCopsDamaged() > BestPursuitRankings[PD_COPS_DAMAGED].Value) {
            BestPursuitRankings[PD_COPS_DAMAGED].Value = iPursuit->GetNumCopsDamaged();
            BestPursuitRankings[PD_COPS_DAMAGED].CarFEKey = car_FEKey;
        }

        if (iPursuit->GetNumCopsDestroyed() > BestPursuitRankings[PD_COPS_DESTROYED].Value) {
            BestPursuitRankings[PD_COPS_DESTROYED].Value = iPursuit->GetNumCopsDestroyed();
            BestPursuitRankings[PD_COPS_DESTROYED].CarFEKey = car_FEKey;
        }

        if (iPursuit->GetNumSpikeStripsDodged() > BestPursuitRankings[PD_SPIKESTRIPS_DODGED].Value) {
            BestPursuitRankings[PD_SPIKESTRIPS_DODGED].Value = iPursuit->GetNumSpikeStripsDodged();
            BestPursuitRankings[PD_SPIKESTRIPS_DODGED].CarFEKey = car_FEKey;
        }

        if (iPursuit->GetNumRoadblocksDodged() > BestPursuitRankings[PD_ROADBLOCKS_DODGED].Value) {
            BestPursuitRankings[PD_ROADBLOCKS_DODGED].Value = iPursuit->GetNumRoadblocksDodged();
            BestPursuitRankings[PD_ROADBLOCKS_DODGED].CarFEKey = car_FEKey;
        }

        if (iPursuit->GetNumHeliSpawns() > BestPursuitRankings[PD_HELICOPTERS_INVOLVED].Value) {
            BestPursuitRankings[PD_HELICOPTERS_INVOLVED].Value = iPursuit->GetNumHeliSpawns();
            BestPursuitRankings[PD_HELICOPTERS_INVOLVED].CarFEKey = car_FEKey;
        }

        if (static_cast<int>(num_infractions) > BestPursuitRankings[PD_NUM_INFRACTIONS].Value) {
            BestPursuitRankings[PD_NUM_INFRACTIONS].Value = num_infractions;
            BestPursuitRankings[PD_NUM_INFRACTIONS].CarFEKey = car_FEKey;
        }

        if (cost_to_state > BestPursuitRankings[PD_COST_TO_STATE].Value) {
            BestPursuitRankings[PD_COST_TO_STATE].Value = cost_to_state;
            BestPursuitRankings[PD_COST_TO_STATE].CarFEKey = car_FEKey;
        }

        if (bounty > BestPursuitRankings[PD_BOUNTY].Value) {
            BestPursuitRankings[PD_BOUNTY].Value = bounty;
            BestPursuitRankings[PD_BOUNTY].CarFEKey = car_FEKey;
        }

        int pos = 5;
        for (int i = 0; i < 5; i++) {
            if (bounty > TopEvadedPursuitScores[i].Bounty) {
                pos = i;
                break;
            }
        }
        for (int j = 4; j > pos; j--) {
            TopEvadedPursuitScores[j] = TopEvadedPursuitScores[j - 1];
        }

        if (pos != 5) {
            TopEvadedPursuitScores[pos].GeneratePursuitID();
            TopEvadedPursuitScores[pos].Bounty = bounty;
            TopEvadedPursuitScores[pos].CarFEKey = car_FEKey;
            TopEvadedPursuitScores[pos].Length = Timer(iPursuit->GetPursuitDuration()).GetPackedTime();
            TopEvadedPursuitScores[pos].NumCops = iPursuit->GetTotalNumCopsInvolved();
            TopEvadedPursuitScores[pos].NumCopsDamaged = iPursuit->GetNumCopsDamaged();
            TopEvadedPursuitScores[pos].NumCopsDestroyed = iPursuit->GetNumCopsDestroyed();
            TopEvadedPursuitScores[pos].NumHelicopters = iPursuit->GetNumHeliSpawns();
            TopEvadedPursuitScores[pos].NumInfractions = num_infractions;
            TopEvadedPursuitScores[pos].NumRoadblocksDodged = iPursuit->GetNumRoadblocksDodged();
            TopEvadedPursuitScores[pos].NumSpikeStripsDodged = iPursuit->GetNumSpikeStripsDodged();
            TopEvadedPursuitScores[pos].TotalCostToState = cost_to_state;
        }
    }
}

int HighScoresDatabase::CalcPursuitRank(ePursuitDetailTypes type, bool career_rank) {
    Attrib::Key type_key;

    switch (type) {
        case PD_PURUSIT_LENGTH:
            if (career_rank) {
                type_key = Attrib::StringToKey("pursuit_length");
            } else {
                type_key = Attrib::StringToKey("pursuit_length_in_pursuit");
            }
            break;
        case PD_COPS_INVOLVED:
            if (career_rank) {
                type_key = Attrib::StringToKey("cops_involved");
            } else {
                type_key = Attrib::StringToKey("cops_involved_in_pursuit");
            }
            break;
        case PD_COPS_DAMAGED:
            if (career_rank) {
                type_key = Attrib::StringToKey("cops_damaged");
            } else {
                type_key = Attrib::StringToKey("cops_damaged_in_pursuit");
            }
            break;
        case PD_COPS_DESTROYED:
            if (career_rank) {
                type_key = Attrib::StringToKey("cops_destroyed");
            } else {
                type_key = Attrib::StringToKey("cops_destroyed_in_pursuit");
            }
            break;
        case PD_SPIKESTRIPS_DODGED:
            if (career_rank) {
                type_key = Attrib::StringToKey("tire_spikes_dodged");
            } else {
                type_key = Attrib::StringToKey("tire_spikes_dodged_in_pursuit");
            }
            break;
        case PD_ROADBLOCKS_DODGED:
            if (career_rank) {
                type_key = Attrib::StringToKey("roadblocks_dodged");
            } else {
                type_key = Attrib::StringToKey("roadblocks_dodged_in_pursuit");
            }
            break;
        case PD_HELICOPTERS_INVOLVED:
            if (career_rank) {
                type_key = Attrib::StringToKey("helis_involved");
            } else {
                type_key = Attrib::StringToKey("helis_involved_in_pursuit");
            }
            break;
        case PD_NUM_INFRACTIONS:
            if (career_rank) {
                type_key = Attrib::StringToKey("total_infractions");
            } else {
                type_key = Attrib::StringToKey("total_infractions_in_pursuit");
            }
            break;
        case PD_COST_TO_STATE:
            if (career_rank) {
                type_key = Attrib::StringToKey("cost_to_state");
            } else {
                type_key = Attrib::StringToKey("cost_to_state_in_pursuit");
            }
            break;
        case PD_BOUNTY:
            if (career_rank) {
                type_key = Attrib::StringToKey("bounty");
            } else {
                type_key = Attrib::StringToKey("bounty_in_pursuit");
            }
            break;
        default:
            type_key = 0;
            break;
    }

    Attrib::Gen::frontend rapsheet(type_key, 0, nullptr);
    int rank = 0x10;

    if (rapsheet.IsValid() && rapsheet.Num_RapSheetRanks() == 15) {
        int player_value;
        if (career_rank) {
            player_value = CareerPursuitDetails.GetValue(type);
        } else {
            player_value = BestPursuitRankings[type].Value;
        }
        bool is_time;
        is_time = type == PD_PURUSIT_LENGTH;
        bool rank_found;

        for (int i = 0; i < static_cast<int>(rapsheet.Num_RapSheetRanks()); i++) {
            if (is_time) {
                rank_found = ((Timer(player_value) >= Timer(rapsheet.RapSheetRanks(i))) != 0);
            } else {
                rank_found = player_value >= static_cast<int>(rapsheet.RapSheetRanks(i));
            }

            if (rank_found) {
                rank = i + 1;
                break;
            }
        }
    }

    return rank;
}

uint32 HighScoresDatabase::GetPreviouslyPursuedCarNameHash() const {
    return GetFECarNameHashFromFEKey(PreviouslyPursuedCarFEKey);
}

void HighScoresDatabase::GetCareerCST(RAP_CTS_ITEM item, int32 &quantity, uint32 &value) const {
    switch (item) {
        case RAP_CTS_HELI_SPAWN:
            quantity = GetCareerPursuitScore(PD_HELICOPTERS_INVOLVED);
            value = quantity * 2000;
            return;
        case RAP_CTS_SUPPORT_VEHICLE_DEPLOYED:
            quantity = CostToStateDetails.mNumSupportVehiclesDeployed;
            value = quantity * 450;
            return;
        case RAP_CTS_COP_CAR_DEPLOYED:
            quantity = CostToStateDetails.mNumCopCarsDeployed;
            value = quantity * 250;
            return;
        case RAP_CTS_COP_DESTROYED:
            quantity = GetCareerPursuitScore(PD_COPS_DESTROYED);
            value = quantity * 5000;
            return;
        case RAP_CTS_COP_DAMAGED:
            quantity = GetCareerPursuitScore(PD_COPS_DAMAGED);
            value = quantity * 250;
            return;
        case RAP_CTS_ROADBLOCK_DEPLOYED:
            quantity = CostToStateDetails.mNumRoadblocksDeployed;
            value = quantity * 500;
            return;
        case RAP_CTS_SPIKE_STRIP_DEPLOYED:
            quantity = CostToStateDetails.mNumSpikeStripsDeployed;
            value = quantity * 250;
            return;
        case RAP_CTS_HELI_SPIKE_STRIP_DEPLOYED:
            quantity = CostToStateDetails.mNumHeliSpikeStripsDeployed;
            value = quantity * 225;
            return;
        case RAP_CTS_TRAFFIC_CAR_HIT:
            quantity = CostToStateDetails.mNumTrafficCarsHit;
            value = quantity * 500;
            return;
        case RAP_CTS_PROPERTY_DAMAGE:
            quantity = CostToStateDetails.mNumPropertiesDamaged;
            value = CostToStateDetails.mPropertyDamageValue;
            return;
    }
}

void HighScoresDatabase::Default() {
    bMemSet(this, 0, sizeof(*this));
}

void TopEvadedPursuitDetail::GeneratePursuitID() {
    PursuitName[0] = 'M';
    PursuitName[1] = 'W';
    PursuitName[2] = '-';

    char *c = PursuitName + 3;

#ifdef FIX_BUGS // BUG: buffer overrun
    for (int i = 3; i < NUM_ELEMENTS(PursuitName) - 1; i++) {
#else
    for (int i = 0; i < NUM_ELEMENTS(PursuitName) - 1; i++) {
#endif
        if ((i & 1) != 0) {
            int r = bRandom(26);
            *c = 'A' + r;
        } else {
            int r = bRandom(10);
            *c = '0' + r;
        }
        c++;
    }

    PursuitName[NUM_ELEMENTS(PursuitName) - 1] = '\0';
}

void CareerPursuitScores::IncValue(ePursuitDetailTypes type, int amount) {
    if (type == PD_PURUSIT_LENGTH) {
        Value[type] = (Timer(Value[type]) + Timer(amount)).GetPackedTime();
    } else {
        Value[type] += amount;
    }
}

int CareerPursuitScores::GetValue(ePursuitDetailTypes type) const {
    int val;
    switch (type) {
        case PD_NUM_INFRACTIONS: {
            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            val = stable->GetTotalNumInfractions(true) + stable->GetTotalNumInfractions(false);
            break;
        }
        case PD_BOUNTY: {
            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            val = stable->GetTotalBounty();
            break;
        }
        default:
            val = Value[type];
            break;
    }
    return val;
}
