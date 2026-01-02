#ifndef SUPPORT_MISC_STRINGHASH_H
#define SUPPORT_MISC_STRINGHASH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Generated/Hashes/zAI.h"
#include "Speed/Indep/Src/Generated/Hashes/zAnim.h"
#include "Speed/Indep/Src/Generated/Hashes/zAttribSys.h"
#include "Speed/Indep/Src/Generated/Hashes/zBWare.h"
#include "Speed/Indep/Src/Generated/Hashes/zCamera.h"
#include "Speed/Indep/Src/Generated/Hashes/zComms.h"
#include "Speed/Indep/Src/Generated/Hashes/zDebug.h"
#include "Speed/Indep/Src/Generated/Hashes/zDynamics.h"
#include "Speed/Indep/Src/Generated/Hashes/zEAXSound.h"
#include "Speed/Indep/Src/Generated/Hashes/zEAXSound2.h"
#include "Speed/Indep/Src/Generated/Hashes/zEagl4Anim.h"
#include "Speed/Indep/Src/Generated/Hashes/zEcstasy.h"
#include "Speed/Indep/Src/Generated/Hashes/zFEng.h"
#include "Speed/Indep/Src/Generated/Hashes/zFe.h"
#include "Speed/Indep/Src/Generated/Hashes/zFe2.h"
#include "Speed/Indep/Src/Generated/Hashes/zFeOverlay.h"
#include "Speed/Indep/Src/Generated/Hashes/zFoundation.h"
#include "Speed/Indep/Src/Generated/Hashes/zGameModes.h"
#include "Speed/Indep/Src/Generated/Hashes/zGameplay.h"
#include "Speed/Indep/Src/Generated/Hashes/zLua.h"
#include "Speed/Indep/Src/Generated/Hashes/zMain.h"
#include "Speed/Indep/Src/Generated/Hashes/zMisc.h"
#include "Speed/Indep/Src/Generated/Hashes/zMiscSmall.h"
#include "Speed/Indep/Src/Generated/Hashes/zMission.h"
#include "Speed/Indep/Src/Generated/Hashes/zOnline.h"
#include "Speed/Indep/Src/Generated/Hashes/zPhysics.h"
#include "Speed/Indep/Src/Generated/Hashes/zPhysicsBehaviors.h"
#include "Speed/Indep/Src/Generated/Hashes/zPlatform.h"
#include "Speed/Indep/Src/Generated/Hashes/zRender.h"
#include "Speed/Indep/Src/Generated/Hashes/zSim.h"
#include "Speed/Indep/Src/Generated/Hashes/zSpeech.h"
#include "Speed/Indep/Src/Generated/Hashes/zTrack.h"
#include "Speed/Indep/Src/Generated/Hashes/zWorld.h"
#include "Speed/Indep/Src/Generated/Hashes/zWorld2.h"

#define BINHASH(str) BINHASH_##str

unsigned int stringhash32(const char *k);

inline unsigned int stringhash(const char *k) {
    return stringhash32(k);
}

#endif
