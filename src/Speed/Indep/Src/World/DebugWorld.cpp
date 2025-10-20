#include "./DebugWorld.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Interfaces/IServiceable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/bWare/Inc/SpeedScript.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"

void DebugWorld::Init() {
    if (!this->mThis) {
        this->mThis = ::new (__FILE__, __LINE__) DebugWorld();
    }
}

DebugWorld::DebugWorld() {
    this->mOnOff = true;
}

void DebugWorld::Service() {
    this->HandleSaveHotPosition();
    this->HandleJumpToHotPosition();
}

int ReadHotPositionScript(const char *filename, SavedHotPosition *hot_positions, int max_hot_positions) {
    int num_hot_positions = 0;
    bMemSet(hot_positions, 0, max_hot_positions * 0x18);
    if (bFileExists(filename) != 0) {
        SpeedScript script = SpeedScript(filename, 1);
        while (num_hot_positions < max_hot_positions && script.GetNextCommand("HOTPOSITION:")) {
            SavedHotPosition *hot_position = &hot_positions[num_hot_positions++];
            hot_position->Position = script.GetNextArgumentVector3();
            
            script.GetNextArgumentString();
            hot_position->Angle = script.GetNextArgumentShort();
            hot_position->Speed = 0.0f;

            if (script.IsAnotherArgument()) {
                script.GetNextArgumentString();
                hot_position->Speed = script.GetNextArgumentFloat();
            }
        }
    }

    return num_hot_positions;
}

void SaveHotPositionScript(const char *filename, SavedHotPosition *hot_positions, int num_hot_positions) {
    bFile *file = bOpen(filename, 0x6, 0x1);
    if (!file) return;

    bFPrintf(file, "//\r\n// HotPosition - %s\r\n//\r\n\r\n", filename);

    for (int n = 0; n < num_hot_positions; n++) {
        SavedHotPosition *hot_position = &hot_positions[n];
        bFPrintf(file, "HOTPOSITION: %8.2f,%8.2f,%8.2f  Angle = 0x%04x  Speed = %.2f\r\n",
            hot_position->Position.x, hot_position->Position.y, hot_position->Position.z,
            hot_position->Angle,
            hot_position->Speed
        );
    }

    bClose(file);
}

int SaveHotPosition = 0;

void DebugWorld::HandleSaveHotPosition() {
    if (SaveHotPosition == 0) return;

    IVehicle *iplayer = UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::First(VEHICLE_PLAYERS);
    if (!iplayer) return;
    
    ICollisionBody *ibody = reinterpret_cast<ICollisionBody *>(iplayer);
    if (!ibody->QueryInterface(&ibody)) return;

    char filename[80];
    bSPrintf(filename, "TRACKS\\HotPosition%s.HOT", LoadedTrackInfo->GetLoadedTrackInfo());

    SavedHotPosition hot_positions[5];
    int num_hot_positions = ReadHotPositionScript(filename, hot_positions, 5);

    if (SaveHotPosition > 5) {
        SaveHotPosition = 5;
    }
    if (SaveHotPosition > num_hot_positions) {
        num_hot_positions = SaveHotPosition;
    }

    SavedHotPosition *hot_position = &hot_positions[SaveHotPosition - 1];
    bVector3 direction;
    eSwizzleWorldVector(ibody->GetForwardVector(), direction);
    hot_position->Angle = bATan(direction.x, direction.y);

    eSwizzleWorldVector(iplayer->GetPosition(), hot_position->Position);
    
    hot_position->Speed = iplayer->GetSpeed();

    SaveHotPositionScript(filename, hot_positions, num_hot_positions);
    SaveHotPosition = 0;

    bRefreshTweaker();
}

int JumpToHotPosition = 0;

void DebugWorld::HandleJumpToHotPosition() {
    if (SaveHotPosition == 0) return;

    IVehicle *iplayer = UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::First(VEHICLE_PLAYERS);
    if (!iplayer) return;

    char filename[80];
    bSPrintf(filename, "TRACKS\\HotPosition%s.HOT", LoadedTrackInfo->GetLoadedTrackInfo());

    SavedHotPosition hot_positions[5];
    int num_hot_positions = ReadHotPositionScript(filename, hot_positions, 5);

    if (JumpToHotPosition <= num_hot_positions) {
        SavedHotPosition *hot_position = &hot_positions[JumpToHotPosition - 1];
        
        bVector3 dir = bVector3(bCos(hot_position->Angle), bSin(hot_position->Angle), 0.0f);
        bVector3 sim_pos;
        bVector3 sim_dir;

        eUnSwizzleWorldVector(hot_position->Position, sim_pos);
        eUnSwizzleWorldVector(dir, sim_dir);

        iplayer->SetVehicleOnGround(*reinterpret_cast<UMath::Vector3 *>(&sim_pos), *reinterpret_cast<UMath::Vector3 *>(&sim_dir));
        Sim::SetStream(*reinterpret_cast<UMath::Vector3 *>(&sim_pos), true);
        iplayer->SetSpeed(hot_position->Speed);
    }

    JumpToHotPosition = 0;
    bRefreshTweaker();
}
