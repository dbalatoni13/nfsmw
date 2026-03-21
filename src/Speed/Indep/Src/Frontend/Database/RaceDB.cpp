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

unsigned int GetFECarNameHashFromFEKey(unsigned int fekey);

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

void CareerPursuitScores::IncValue(ePursuitDetailTypes type, int amount) {
    if (type == 0) {
        Value[0] += amount;
    } else {
        Value[type] += amount;
    }
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
                *c = static_cast< char >(r + 'A');
            } else {
                int r = bRandom(10);
                *c = static_cast< char >(r + '0');
            }
            i++;
            c++;
        } while (i <= 10);
    }

    PursuitName[11] = '\0';
}

void HighScoresDatabase::Default() {
    memset(this, 0, sizeof(*this));
}

int HighScoresDatabase::CalcPursuitRank(ePursuitDetailTypes type, bool career_rank) {
    const char *attrib_name;
    Attrib::Key key;
    int player_value;
    int rank;

    if (type == static_cast< ePursuitDetailTypes >(4)) {
        if (!career_rank) {
            attrib_name = "tire_spikes_dodged_in_pursuit";
        } else {
            attrib_name = "tire_spikes_dodged";
        }
    } else if (type < static_cast< ePursuitDetailTypes >(5)) {
        if (type == static_cast< ePursuitDetailTypes >(1)) {
            if (!career_rank) {
                attrib_name = "cops_involved_in_pursuit";
            } else {
                attrib_name = "cops_involved";
            }
        } else if (type < static_cast< ePursuitDetailTypes >(2)) {
            if (type != static_cast< ePursuitDetailTypes >(0)) {
                key = 0;
                goto GotAttribKey;
            }
            if (!career_rank) {
                attrib_name = "pursuit_length_in_pursuit";
            } else {
                attrib_name = "pursuit_length";
            }
        } else if (type == static_cast< ePursuitDetailTypes >(2)) {
            if (!career_rank) {
                attrib_name = "cops_damaged_in_pursuit";
            } else {
                attrib_name = "cops_damaged";
            }
        } else {
            if (type != static_cast< ePursuitDetailTypes >(3)) {
                key = 0;
                goto GotAttribKey;
            }
            if (!career_rank) {
                attrib_name = "cops_destroyed_in_pursuit";
            } else {
                attrib_name = "cops_destroyed";
            }
        }
    } else if (type == static_cast< ePursuitDetailTypes >(7)) {
        if (!career_rank) {
            attrib_name = "cost_to_state_in_pursuit";
        } else {
            attrib_name = "cost_to_state";
        }
    } else if (type < static_cast< ePursuitDetailTypes >(8)) {
        if (type == static_cast< ePursuitDetailTypes >(5)) {
            if (!career_rank) {
                attrib_name = "roadblocks_dodged_in_pursuit";
            } else {
                attrib_name = "roadblocks_dodged";
            }
        } else {
            if (type != static_cast< ePursuitDetailTypes >(6)) {
                key = 0;
                goto GotAttribKey;
            }
            if (!career_rank) {
                attrib_name = "helis_involved_in_pursuit";
            } else {
                attrib_name = "helis_involved";
            }
        }
    } else if (type == static_cast< ePursuitDetailTypes >(8)) {
        if (!career_rank) {
            attrib_name = "total_infractions_in_pursuit";
        } else {
            attrib_name = "total_infractions";
        }
    } else {
        if (type != static_cast< ePursuitDetailTypes >(9)) {
            key = 0;
            goto GotAttribKey;
        }
        if (!career_rank) {
            attrib_name = "bounty_in_pursuit";
        } else {
            attrib_name = "bounty";
        }
    }

    key = Attrib::StringToKey(attrib_name);

GotAttribKey:
    Attrib::Gen::frontend rankingsData(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), key), 0, nullptr);
    rank = 0x10;

    if (rankingsData.IsValid()) {
        if (rankingsData.Num_RapSheetRanks() == 15) {
            if (career_rank) {
                player_value = CareerPursuitDetails.GetValue(type);
            } else {
                player_value = BestPursuitRankings[type].Value;
            }

            for (int i = 0; i < static_cast< int >(rankingsData.Num_RapSheetRanks()); i++) {
                int rank_value;

                if (type == 0) {
                    Timer t;
                    float rank_time = rankingsData.RapSheetRanks(static_cast< unsigned int >(i));
                    t.SetTime(rank_time);
                    rank_value = t.GetPackedTime();
                } else {
                    rank_value = static_cast< int >(rankingsData.RapSheetRanks(static_cast< unsigned int >(i)));
                }

                if (player_value >= rank_value) {
                    rank = i + 1;
                    break;
                }
            }
        }
    }

    return rank;
}

unsigned int HighScoresDatabase::GetPreviouslyPursuedCarNameHash() const {
    return GetFECarNameHashFromFEKey(PreviouslyPursuedCarFEKey);
}

void HighScoresDatabase::GetCareerCST(RAP_CTS_ITEM item, int &quantity, unsigned int &value) const {
    switch (item) {
    case RAP_CTS_HELI_SPAWN:
        quantity = GetCareerPursuitScore(static_cast< ePursuitDetailTypes >(6));
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
        quantity = GetCareerPursuitScore(static_cast< ePursuitDetailTypes >(3));
        value = quantity * 5000;
        return;
    case RAP_CTS_COP_DAMAGED:
        quantity = GetCareerPursuitScore(static_cast< ePursuitDetailTypes >(2));
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

void HighScoresDatabase::CommitHighScoresPauseQuit() {
    ++TotalLosses;
}

void HighScoresDatabase::CommitPursuitInfo(IPursuit *iPursuit, unsigned int car_FEKey, int bounty, unsigned int num_infractions) {
    int cost_to_state = iPursuit->CalcTotalCostToState();

    PreviouslyPursuedCarFEKey = car_FEKey;

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

    Timer pursuit_length;
    pursuit_length.SetTime(iPursuit->GetPursuitDuration());
    CareerPursuitDetails.IncValue(static_cast< ePursuitDetailTypes >(0), pursuit_length.GetPackedTime());
    CareerPursuitDetails.IncValue(static_cast< ePursuitDetailTypes >(1), iPursuit->GetTotalNumCopsInvolved());
    CareerPursuitDetails.IncValue(static_cast< ePursuitDetailTypes >(2), iPursuit->GetNumCopsDamaged());
    CareerPursuitDetails.IncValue(static_cast< ePursuitDetailTypes >(3), iPursuit->GetNumCopsDestroyed());
    CareerPursuitDetails.IncValue(static_cast< ePursuitDetailTypes >(4), iPursuit->GetNumSpikeStripsDodged());
    CareerPursuitDetails.IncValue(static_cast< ePursuitDetailTypes >(5), iPursuit->GetNumRoadblocksDodged());
    CareerPursuitDetails.IncValue(static_cast< ePursuitDetailTypes >(6), iPursuit->GetNumHeliSpawns());
    CareerPursuitDetails.IncValue(static_cast< ePursuitDetailTypes >(7), cost_to_state);

    if (BestPursuitRankings[0].Value < pursuit_length.GetPackedTime()) {
        Timer best_pursuit_length;
        best_pursuit_length.SetTime(iPursuit->GetPursuitDuration());
        BestPursuitRankings[0].CarFEKey = car_FEKey;
        BestPursuitRankings[0].Value = best_pursuit_length.GetPackedTime();
    }

    if (BestPursuitRankings[1].Value < iPursuit->GetTotalNumCopsInvolved()) {
        BestPursuitRankings[1].Value = iPursuit->GetTotalNumCopsInvolved();
        BestPursuitRankings[1].CarFEKey = car_FEKey;
    }

    if (BestPursuitRankings[2].Value < iPursuit->GetNumCopsDamaged()) {
        BestPursuitRankings[2].Value = iPursuit->GetNumCopsDamaged();
        BestPursuitRankings[2].CarFEKey = car_FEKey;
    }

    if (BestPursuitRankings[3].Value < iPursuit->GetNumCopsDestroyed()) {
        BestPursuitRankings[3].Value = iPursuit->GetNumCopsDestroyed();
        BestPursuitRankings[3].CarFEKey = car_FEKey;
    }

    if (BestPursuitRankings[4].Value < iPursuit->GetNumSpikeStripsDodged()) {
        BestPursuitRankings[4].Value = iPursuit->GetNumSpikeStripsDodged();
        BestPursuitRankings[4].CarFEKey = car_FEKey;
    }

    if (BestPursuitRankings[5].Value < iPursuit->GetNumRoadblocksDodged()) {
        BestPursuitRankings[5].Value = iPursuit->GetNumRoadblocksDodged();
        BestPursuitRankings[5].CarFEKey = car_FEKey;
    }

    if (BestPursuitRankings[6].Value < iPursuit->GetNumHeliSpawns()) {
        BestPursuitRankings[6].Value = iPursuit->GetNumHeliSpawns();
        BestPursuitRankings[6].CarFEKey = car_FEKey;
    }

    if (BestPursuitRankings[8].Value < static_cast< int >(num_infractions)) {
        BestPursuitRankings[8].Value = num_infractions;
        BestPursuitRankings[8].CarFEKey = car_FEKey;
    }

    if (BestPursuitRankings[7].Value < cost_to_state) {
        BestPursuitRankings[7].Value = cost_to_state;
        BestPursuitRankings[7].CarFEKey = car_FEKey;
    }

    if (BestPursuitRankings[9].Value < bounty) {
        BestPursuitRankings[9].Value = bounty;
        BestPursuitRankings[9].CarFEKey = car_FEKey;
    }

    int pos = 5;
    if (TopEvadedPursuitScores[0].Bounty < bounty) {
        pos = 0;
    } else {
        for (int i = 1; i < 5; i++) {
            if (bounty > TopEvadedPursuitScores[i].Bounty) {
                pos = i;
                break;
            }
        }
    }

    if (pos < 4) {
        for (int i = 4; i > pos; i--) {
            TopEvadedPursuitScores[i] = TopEvadedPursuitScores[i - 1];
        }
    }

    if (pos != 5) {
        TopEvadedPursuitDetail &detail = TopEvadedPursuitScores[pos];
        Timer detail_length;

        detail.GeneratePursuitID();
        detail.CarFEKey = car_FEKey;
        detail.Bounty = bounty;
        detail_length.SetTime(iPursuit->GetPursuitDuration());
        detail.Length = detail_length.GetPackedTime();
        detail.NumCops = iPursuit->GetTotalNumCopsInvolved();
        detail.NumCopsDamaged = iPursuit->GetNumCopsDamaged();
        detail.NumCopsDestroyed = iPursuit->GetNumCopsDestroyed();
        detail.NumRoadblocksDodged = iPursuit->GetNumRoadblocksDodged();
        detail.NumSpikeStripsDodged = iPursuit->GetNumSpikeStripsDodged();
        detail.TotalCostToState = cost_to_state;
        detail.NumInfractions = num_infractions;
        detail.NumHelicopters = iPursuit->GetNumHeliSpawns();
    }
}

void UserProfile::CommitHighScoresPauseQuit() {
    HighScores.CommitHighScoresPauseQuit();
}

void UserProfile::CommitPursuitInfo(IPursuit *iPursuit, unsigned int car_FEKey, unsigned int bounty, unsigned int num_infractions) {
    HighScores.CommitPursuitInfo(iPursuit, car_FEKey, bounty, num_infractions);
}
