#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#include "types.h"

#include <string.h>

void FixDot(char *buf, int size) {
    for (int i = 0; i < size; i++) {
        if (buf[i] == '.') {
            buf[i] = '_';
        }
    }
}

uint32 CalcLanguageHash(const char *prefix, GRaceParameters *pRaceParams) {
    char buffer[64];
    FEngSNPrintf(buffer, 0x40, "%s%s", prefix, pRaceParams->GetEventID());
    FixDot(buffer, 0x40);
    return FEHashUpper(buffer);
}

void RaceSettings::Default() {
    NumOpponents = 3;
    AISkill = 1;
    CopDensity = 1;
    TrafficDensity = 1;
    CatchUp = true;
    CopsOn = false;
    TrackDirection = 0;
    NumLaps = 2;
    IsLapKO = false;
    for (int i = 0; i < 2; i++) {
        SelectedCar[i] = 0;
    }
    RegionFilterBits = 3;
}

void HighScoresDatabase::CommitHighScoresPauseQuit() {
    ++TotalLosses;
}

void HighScoresDatabase::CommitPursuitInfo(IPursuit *iPursuit, uint32 car_name_hash, int32 bounty, unsigned int num_infractions) {
    PreviouslyPursuedCarFEKey = car_name_hash;

    int cost_to_state = iPursuit->CalcTotalCostToState();

    if (iPursuit->IsPerpBusted()) {
        return;
    }

    CostToStateDetails.mNumCopCarsDeployed += iPursuit->GetNumCopCarsDeployed();
    CostToStateDetails.mNumHeliSpikeStripsDeployed += iPursuit->GetNumHeliSpikeStripDeployed();
    CostToStateDetails.mNumPropertiesDamaged += iPursuit->GetNumPropertyDamaged();
    CostToStateDetails.mNumRoadblocksDeployed += iPursuit->GetNumRoadblocksDeployed();
    CostToStateDetails.mNumSpikeStripsDeployed += iPursuit->GetNumSpikeStripsDeployed();
    CostToStateDetails.mNumSupportVehiclesDeployed += iPursuit->GetNumSupportVehiclesDeployed();
    CostToStateDetails.mNumTrafficCarsHit += iPursuit->GetNumTrafficCarsHit();
    CostToStateDetails.mPropertyDamageValue += iPursuit->GetValueOfPropertyDamaged();

    CareerPursuitDetails.IncValue(static_cast<ePursuitDetailTypes>(0), Timer(iPursuit->GetPursuitDuration()).GetPackedTime());
    CareerPursuitDetails.IncValue(static_cast<ePursuitDetailTypes>(1), iPursuit->GetTotalNumCopsInvolved());
    CareerPursuitDetails.IncValue(static_cast<ePursuitDetailTypes>(2), iPursuit->GetNumCopsDamaged());
    CareerPursuitDetails.IncValue(static_cast<ePursuitDetailTypes>(3), iPursuit->GetNumCopsDestroyed());
    CareerPursuitDetails.IncValue(static_cast<ePursuitDetailTypes>(4), iPursuit->GetNumSpikeStripsDodged());
    CareerPursuitDetails.IncValue(static_cast<ePursuitDetailTypes>(5), iPursuit->GetNumRoadblocksDodged());
    CareerPursuitDetails.IncValue(static_cast<ePursuitDetailTypes>(6), iPursuit->GetNumHeliSpawns());
    CareerPursuitDetails.IncValue(static_cast<ePursuitDetailTypes>(7), cost_to_state);

    if (Timer(iPursuit->GetPursuitDuration()) > Timer(BestPursuitRankings[0].Value)) {
        BestPursuitRankings[0].Value = Timer(iPursuit->GetPursuitDuration()).GetPackedTime();
        BestPursuitRankings[0].CarFEKey = car_name_hash;
    }

    if (iPursuit->GetTotalNumCopsInvolved() > BestPursuitRankings[1].Value) {
        BestPursuitRankings[1].Value = iPursuit->GetTotalNumCopsInvolved();
        BestPursuitRankings[1].CarFEKey = car_name_hash;
    }

    if (iPursuit->GetNumCopsDamaged() > BestPursuitRankings[2].Value) {
        BestPursuitRankings[2].Value = iPursuit->GetNumCopsDamaged();
        BestPursuitRankings[2].CarFEKey = car_name_hash;
    }

    if (iPursuit->GetNumCopsDestroyed() > BestPursuitRankings[3].Value) {
        BestPursuitRankings[3].Value = iPursuit->GetNumCopsDestroyed();
        BestPursuitRankings[3].CarFEKey = car_name_hash;
    }

    if (iPursuit->GetNumSpikeStripsDodged() > BestPursuitRankings[4].Value) {
        BestPursuitRankings[4].Value = iPursuit->GetNumSpikeStripsDodged();
        BestPursuitRankings[4].CarFEKey = car_name_hash;
    }

    if (iPursuit->GetNumRoadblocksDodged() > BestPursuitRankings[5].Value) {
        BestPursuitRankings[5].Value = iPursuit->GetNumRoadblocksDodged();
        BestPursuitRankings[5].CarFEKey = car_name_hash;
    }

    if (iPursuit->GetNumHeliSpawns() > BestPursuitRankings[6].Value) {
        BestPursuitRankings[6].Value = iPursuit->GetNumHeliSpawns();
        BestPursuitRankings[6].CarFEKey = car_name_hash;
    }

    if (static_cast<int>(num_infractions) > BestPursuitRankings[8].Value) {
        BestPursuitRankings[8].Value = num_infractions;
        BestPursuitRankings[8].CarFEKey = car_name_hash;
    }

    if (cost_to_state > BestPursuitRankings[7].Value) {
        BestPursuitRankings[7].Value = cost_to_state;
        BestPursuitRankings[7].CarFEKey = car_name_hash;
    }

    if (bounty > BestPursuitRankings[9].Value) {
        BestPursuitRankings[9].Value = bounty;
        BestPursuitRankings[9].CarFEKey = car_name_hash;
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
        TopEvadedPursuitScores[pos].CarFEKey = car_name_hash;
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

int HighScoresDatabase::CalcPursuitRank(ePursuitDetailTypes type, bool career_rank) {
    Attrib::Key key;
    int rank;

    switch (type) {
        case PD_PURUSIT_LENGTH:
            if (career_rank) {
                key = Attrib::StringToKey("pursuit_length");
            } else {
                key = Attrib::StringToKey("pursuit_length_in_pursuit");
            }
            break;
        case PD_COPS_INVOLVED:
            if (career_rank) {
                key = Attrib::StringToKey("cops_involved");
            } else {
                key = Attrib::StringToKey("cops_involved_in_pursuit");
            }
            break;
        case PD_COPS_DAMAGED:
            if (career_rank) {
                key = Attrib::StringToKey("cops_damaged");
            } else {
                key = Attrib::StringToKey("cops_damaged_in_pursuit");
            }
            break;
        case PD_COPS_DESTROYED:
            if (career_rank) {
                key = Attrib::StringToKey("cops_destroyed");
            } else {
                key = Attrib::StringToKey("cops_destroyed_in_pursuit");
            }
            break;
        case PD_SPIKESTRIPS_DODGED:
            if (career_rank) {
                key = Attrib::StringToKey("tire_spikes_dodged");
            } else {
                key = Attrib::StringToKey("tire_spikes_dodged_in_pursuit");
            }
            break;
        case PD_ROADBLOCKS_DODGED:
            if (career_rank) {
                key = Attrib::StringToKey("roadblocks_dodged");
            } else {
                key = Attrib::StringToKey("roadblocks_dodged_in_pursuit");
            }
            break;
        case PD_HELICOPTERS_INVOLVED:
            if (career_rank) {
                key = Attrib::StringToKey("helis_involved");
            } else {
                key = Attrib::StringToKey("helis_involved_in_pursuit");
            }
            break;
        case PD_NUM_INFRACTIONS:
            if (career_rank) {
                key = Attrib::StringToKey("total_infractions");
            } else {
                key = Attrib::StringToKey("total_infractions_in_pursuit");
            }
            break;
        case PD_COST_TO_STATE:
            if (career_rank) {
                key = Attrib::StringToKey("cost_to_state");
            } else {
                key = Attrib::StringToKey("cost_to_state_in_pursuit");
            }
            break;
        case PD_BOUNTY:
            if (career_rank) {
                key = Attrib::StringToKey("bounty");
            } else {
                key = Attrib::StringToKey("bounty_in_pursuit");
            }
            break;
        default:
            key = 0;
            goto GotAttribKey;
    }

GotAttribKey:
    Attrib::Gen::frontend rankingsData(key, 0, nullptr);
    rank = 0x10;

    if (rankingsData.IsValid()) {
        if (rankingsData.Num_RapSheetRanks() == 15) {
            int player_value;
            bool is_time;
            bool rank_found;

            if (career_rank) {
                player_value = CareerPursuitDetails.GetValue(type);
            } else {
                player_value = BestPursuitRankings[type].Value;
            }

            is_time = type == 0;

            for (int i = 0; i < static_cast<int>(rankingsData.Num_RapSheetRanks()); i++) {
                if (is_time) {
                    Timer rank_time(rankingsData.RapSheetRanks(static_cast<unsigned int>(i)));
                    rank_found = Timer(player_value) >= rank_time;
                } else {
                    rank_found = player_value >= static_cast<int>(rankingsData.RapSheetRanks(static_cast<unsigned int>(i)));
                }

                if (rank_found) {
                    rank = i + 1;
                    break;
                }
            }
        }
    }

    return rank;
}

uint32 HighScoresDatabase::GetPreviouslyPursuedCarNameHash() const {
    return GetFECarNameHashFromFEKey(PreviouslyPursuedCarFEKey);
}

void HighScoresDatabase::GetCareerCST(RAP_CTS_ITEM item, int &quantity, unsigned int &value) const {
    switch (item) {
        case RAP_CTS_HELI_SPAWN:
            quantity = GetCareerPursuitScore(static_cast<ePursuitDetailTypes>(6));
            value = quantity * 2000;
            return;
        case RAP_CTS_SUPPORT_VEHICLE_DEPLOYED:
            quantity = CostToStateDetails.mNumSupportVehiclesDeployed;
            value = quantity * 0x1C2;
            return;
        case RAP_CTS_COP_CAR_DEPLOYED:
            quantity = CostToStateDetails.mNumCopCarsDeployed;
            value = quantity * 0xFA;
            return;
        case RAP_CTS_COP_DESTROYED:
            quantity = GetCareerPursuitScore(static_cast<ePursuitDetailTypes>(3));
            value = quantity * 5000;
            return;
        case RAP_CTS_COP_DAMAGED:
            quantity = GetCareerPursuitScore(static_cast<ePursuitDetailTypes>(2));
            value = quantity * 0xFA;
            return;
        case RAP_CTS_ROADBLOCK_DEPLOYED:
            quantity = CostToStateDetails.mNumRoadblocksDeployed;
            value = quantity * 500;
            return;
        case RAP_CTS_SPIKE_STRIP_DEPLOYED:
            quantity = CostToStateDetails.mNumSpikeStripsDeployed;
            value = quantity * 0xFA;
            return;
        case RAP_CTS_HELI_SPIKE_STRIP_DEPLOYED:
            quantity = CostToStateDetails.mNumHeliSpikeStripsDeployed;
            value = quantity * 0xE1;
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
    char *c = PursuitName + 3;

    PursuitName[0] = 'M';
    PursuitName[1] = 'W';
    PursuitName[2] = '-';

    {
        int i = 0;

        do {
            if ((i & 1) != 0) {
                int r = bRandom(0x1A);
                *c = static_cast<char>(r + 'A');
            } else {
                int r = bRandom(10);
                *c = static_cast<char>(r + '0');
            }
            i++;
            c++;
        } while (i <= 10);
    }

    PursuitName[11] = '\0';
}

void CareerPursuitScores::IncValue(ePursuitDetailTypes type, int amount) {
    if (type == 0) {
        Value[0] += amount;
    } else {
        Value[type] += amount;
    }
}

int CareerPursuitScores::GetValue(ePursuitDetailTypes type) const {
    int val;
    switch (static_cast<int>(type)) {
        case 8: {
            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            val = stable->GetTotalNumInfractions(true) + stable->GetTotalNumInfractions(false);
            break;
        }
        case 9: {
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
