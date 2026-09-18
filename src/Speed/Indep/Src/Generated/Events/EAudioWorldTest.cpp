#include "EAudioWorldTest.hpp"

#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/Generated/Messages/MAudioReflection.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/Gameflow.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"

EAudioWorldTest::EAudioWorldTest() : Event(0x10) {
}

EAudioWorldTest::~EAudioWorldTest() {
    const float TEST_DURATION = 0.4f;
    const float TEST_MAX_DISTANCE = 20.0f;

    if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_RACING) {

        if (IsSoundEnabled) {

            for (int n = 0; n < SndCamera::NumPlayers; n++) {

                UMath::Vector4 originToBarrier[2];
                WCollisionMgr::WorldCollisionInfo cInfo;
                float fTestDist;

                originToBarrier[0].z = SndCamera::GetWorldCarPos3(n)->x;
                originToBarrier[0].x = -SndCamera::GetWorldCarPos3(n)->y;
                originToBarrier[0].y = SndCamera::GetWorldCarPos3(n)->z;

                originToBarrier[1].z = SndCamera::GetNormCarDir3(n)->x;
                originToBarrier[1].x = -SndCamera::GetNormCarDir3(n)->y;
                originToBarrier[1].y = SndCamera::GetNormCarDir3(n)->z;

                fTestDist = bAbs(SndCamera::GetWorldCarVel(n)) * TEST_DURATION;

                if (fTestDist > TEST_MAX_DISTANCE) {
                    fTestDist = TEST_MAX_DISTANCE;
                }

                if (fTestDist < 4.0f) {
                    continue;
                }

                VU0_v4scaleadd(originToBarrier[1], fTestDist, originToBarrier[0], originToBarrier[1]);

                if (WCollisionMgr(0, 3).CheckHitWorld(originToBarrier, cInfo, 2)) {

                    UMath::Vector4 cardir;

                    cardir.z = SndCamera::GetNormCarDir3(n)->x;
                    cardir.x = -SndCamera::GetNormCarDir3(n)->y;
                    cardir.y = SndCamera::GetNormCarDir3(n)->z;

                    if (bAbs(UMath::Dot(cardir, cInfo.fNormal)) >= 0.2f) {

                        MAudioReflection(n, cInfo.fDist, false).Send("FRONT_BARRIER");
                    }
                }
            }
        }
    }
}

const char *EAudioWorldTest::GetEventName() const {
    return "EAudioWorldTest";
}

void EAudioWorldTest_MakeEvent_Callback(const void *staticData) {
    new EAudioWorldTest();
}
