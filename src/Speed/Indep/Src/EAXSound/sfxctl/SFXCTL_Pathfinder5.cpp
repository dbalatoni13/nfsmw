#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Pathfinder5.hpp"

SFXCTL_Pathfinder *g_pSFXCTL_Pathfinder;

SndBase::TypeInfo *SFXCTL_Pathfinder::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Pathfinder::GetTypeName() const { return s_TypeInfo.typeName; }

int SFXCTL_Pathfinder::GetController(int Index) { return -1; }

void SFXCTL_Pathfinder::AttachController(SFXCTL *psfxctl) {}

void SFXCTL_Pathfinder::UpdateMixerOutputs() {}

void SFXCTL_Pathfinder::SetupSFX(CSTATE_Base *_StateBase) { g_pSFXCTL_Pathfinder = this; }

void SFXCTL_Pathfinder::EventReleaseCallback(void *eventID, PATHEVENTRESULT result) {}
