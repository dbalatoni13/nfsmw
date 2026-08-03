#include "OnlineManager.hpp"

OnlineManager TheOnlineManager;

OnlineManager::OnlineManager()
    : RestartingRace(false) //
    , RestartRequested(false) {
    State = OLS_DISCONNECTED;
    bMemSet(PersonaMap, 0, sizeof(PersonaMap));
    InitQuantizers();

    pLocalRacer = nullptr;
    NumRacers = 0;
    bOnlineRace = false;
    mMasterTime = 0;
    mServerTime = 0;
    CountdownSyncAnims.UnSet();
    CountdownSendDataCRC.UnSet();
    LastSyncedWTTimeStamp.UnSet();
    LastAntiCheatWorldTimer.UnSet();
    LastAntiCheatRealTimer.UnSet();
    TimeRaceFinished.UnSet();
    RaceTimeup = false;

    for (int i = 3; i >= 0; --i) {
        pRacers[i] = nullptr;
    }
    ClearAnimWorldObjects();
}

void OnlineManager::InitQuantizers() {
    float min_value;
    float max_value;

    PositionQuantizerX.Init("PositionQuantizerX", -4800.0f, 1792.0f, 0.01f);
    PositionQuantizerY.Init("PositionQuantizerY", -15.0f, 300.0f, 0.01f);
    max_value = 1.0f;
    PositionQuantizerZ.Init("PositionQuantizerZ", -1664.0f, 4224.0f, 0.01f);
    min_value = 0.0f;
    VelocityQuantizer.Init("VelocityQuantizer", -125.0f, 125.0f, 0.1f);
    MatrixQuantizer.Init("MatrixQuantizer", -1.0f, max_value, 0.01f);
    AngleQuantizer.Init("AngleQuantizer", -0.5f, 0.5f, 0.0025f);
    AccelerationQuantizer.Init("AccelerationQuantizer", -25.0f, 25.0f, 0.1f);
    AVelocityQuantizer.Init("AVelocityQuantizer", -5.0f, 5.0f, 0.01f);
    ControlQuantizer.Init("ControlQuantizer", min_value, max_value, 0.15f);
    QuantFloatSimTime.Init("QuantFloatSimTime", min_value, 5400.0f, 0.001f);
    QuantCarSlotID.Init("QuantCarSlotID", 0, 0x8b);
    QuantCarPartIndex.Init("QuantPartIndex", 0, 16000);
    QuantCarType.Init("QuantCarType", 0, 0x4e);
    QuantInt2Bit.Init("QuantInt2Bit", 0, 3);
    QuantInt3Bit.Init("QuantInt3Bit", 0, 7);
    QuantInt4Bit.Init("QuantInt4Bit", 0, 0xf);
    QuantFloatTime.Init("QuantFloatTime", min_value, 14400.0f, 0x18);
}
