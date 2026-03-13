#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#include "types.h"

#include <string.h>

unsigned int GetFECarNameHashFromFEKey(unsigned int fekey);

namespace {

const char *GetPursuitRankAttribName(ePursuitDetailTypes type, bool career_rank) {
    switch (static_cast< int >(type)) {
    case 0:
        return career_rank ? "rap_sheet_cost_to_state_career" : "rap_sheet_cost_to_state_all";
    case 1:
        return career_rank ? "rap_sheet_bounty_career" : "rap_sheet_bounty_all";
    case 2:
        return career_rank ? "rap_sheet_infractions_career" : "rap_sheet_infractions_all";
    case 3:
        return career_rank ? "rap_sheet_speeding_career" : "rap_sheet_speeding_all";
    case 4:
        return career_rank ? "rap_sheet_roadblocks_career" : "rap_sheet_roadblocks_all";
    case 5:
        return career_rank ? "rap_sheet_cops_disabled_career" : "rap_sheet_cops_disabled_all";
    case 6:
        return career_rank ? "rap_sheet_spike_strips_career" : "rap_sheet_spike_strips_all";
    case 7:
        return career_rank ? "rap_sheet_cops_deployed_career" : "rap_sheet_cops_deployed_all";
    case 8:
        return career_rank ? "rap_sheet_helicopters_career" : "rap_sheet_helicopters_all";
    case 9:
        return "rap_sheet_pursuit_length";
    default:
        return nullptr;
    }
}

}

int CareerPursuitScores::GetValue(ePursuitDetailTypes type) const {
    return Value[type];
}

void CareerPursuitScores::IncValue(ePursuitDetailTypes type, int amount) {
    if (type == 0) {
        Value[0] += amount;
    } else {
        Value[type] += amount;
    }
}

void TopEvadedPursuitDetail::GeneratePursuitID() {
    char *id = reinterpret_cast< char * >(this);
    char *it = id + 3;
    int i = 0;

    id[0] = 'M';
    id[1] = 'W';
    id[2] = '-';

    do {
        char c;

        if ((i & 1) != 0) {
            c = static_cast< char >(bRandom(0x1A) + 'A');
        } else {
            c = static_cast< char >(bRandom(10) + '0');
        }

        *it = c;
        i++;
        it++;
    } while (i <= 10);

    id[11] = '\0';
}

void HighScoresDatabase::Default() {
    memset(this, 0, sizeof(*this));
}

int HighScoresDatabase::CalcPursuitRank(ePursuitDetailTypes type, bool career_rank) {
    const char *attrib_name = GetPursuitRankAttribName(type, career_rank);
    Attrib::Key key = attrib_name ? Attrib::StringToKey(attrib_name) : 0;
    Attrib::Gen::frontend rankingsData(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), key), 0, nullptr);
    int player_value = 0;
    int rank = 0x10;

    if (!rankingsData.IsValid()) {
        return rank;
    }

    if (rankingsData.Num_RapSheetRanks() != 15) {
        return rank;
    }

    if (career_rank) {
        player_value = CareerPursuitDetails.GetValue(type);
    } else {
        player_value = BestPursuitRankings[type].Value;
    }

    for (int i = 0; i < static_cast< int >(rankingsData.Num_RapSheetRanks()); i++) {
        int rank_value;

        if (type == 0) {
            Timer t;
            t.SetTime(rankingsData.RapSheetRanks(static_cast< unsigned int >(i)));
            rank_value = t.GetPackedTime();
        } else {
            rank_value = static_cast< int >(rankingsData.RapSheetRanks(static_cast< unsigned int >(i)));
        }

        if (rank_value <= player_value) {
            rank = i + 1;
            break;
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
        quantity = CareerPursuitDetails.GetValue(static_cast< ePursuitDetailTypes >(6));
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
        quantity = CareerPursuitDetails.GetValue(static_cast< ePursuitDetailTypes >(3));
        value = quantity * 5000;
        return;
    case RAP_CTS_COP_DAMAGED:
        quantity = CareerPursuitDetails.GetValue(static_cast< ePursuitDetailTypes >(2));
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

void HighScoresDatabase::CommitHighScoresPauseQuit() {}

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
