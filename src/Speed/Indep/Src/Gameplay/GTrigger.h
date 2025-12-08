#ifndef GAMEPLAY_GTRIGGER_H
#define GAMEPLAY_GTRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GRuntimeInstance.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

// // total size: 0xCC
// class GTrigger : public GRuntimeInstance {
//   private:
//     WTrigger mWorldTrigger;                                          // offset 0x28, size 0x40
//     UMath::Vector3 mDirection;                                       // offset 0x68, size 0xC
//     unsigned int mTriggerEnabled;                                    // offset 0x74, size 0x4
//     UTL::Std::vector<ISimable *, _type_ID_SimObjList> mSimObjInside; // offset 0x78, size 0x10
//     struct EventList mEventList;                                     // offset 0x88, size 0x10
//     struct EventStaticData mEventStaticData;                         // offset 0x98, size 0x10
//     unsigned char mTriggerEventData[16];                             // offset 0xA8, size 0x10
//     EmitterGroup *mParticleEffect[2];                                // offset 0xB8, size 0x8
//     struct GIcon *mIcon;                                             // offset 0xC0, size 0x4
//     bool mEnabled;                                                   // offset 0xC4, size 0x1
//     int mActivationReferences;                                       // offset 0xC8, size 0x4
// };

#endif
