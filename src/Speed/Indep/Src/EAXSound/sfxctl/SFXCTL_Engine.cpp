#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"

SndBase::TypeInfo *SFXCTL_Engine::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Engine::GetTypeName() const { return s_TypeInfo.typeName; }

float SFXCTL_Engine::GetEngRPM() { return m_fEng_RPM; }

float SFXCTL_Engine::GetSmoothedEngRPM() { return m_fSmoothedEng_RPM; }

float SFXCTL_Engine::GetEngTorque() { return m_fEng_Trq; }

float SFXCTL_Engine::GetSmoothedEngTorque() { return m_fSmoothedEng_Trq; }
